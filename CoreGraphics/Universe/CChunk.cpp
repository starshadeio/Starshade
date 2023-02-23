//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunk.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CChunk.h"
#include "../Application/CSceneManager.h"
#include "../Graphics/CMeshRenderer_.h"
#include "../Graphics/CMaterial.h"
#include "../Graphics/CGraphicsAPI.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include "../Utilities/CJobSystem.h"
#include <Application/CCommandManager.h>
#include <Math/CMathFNV.h>
#include <Utilities/CMemoryFree.h>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <queue>

namespace Universe
{
	CChunk::CChunk(const CVObject* pObject) :
		CVComponent(pObject),
#if _DEBUG
		m_bInitialized(false),
#endif
		m_bDirty(false),
		m_bAwaitingRebuild(false),
		m_meshIndex(0),
		m_chunkSize(0),
		m_meshData{ pObject, pObject },
		m_meshContainer(pObject),
		m_meshContainerWire(pObject),
		m_pMeshRendererList{ nullptr, nullptr },
		//m_pMeshRendererListWire{ nullptr, nullptr },
		m_pMaterial(nullptr),
		m_pMaterialWire(nullptr),
		m_pBlockList(nullptr)
	{
		memset(m_pChunkAdj, 0, sizeof(m_pChunkAdj));
	}

	CChunk::~CChunk()
	{
	}
	
	void CChunk::Initialize()
	{
#if _DEBUG
		m_bInitialized = true;
#endif

		{ // Build initial chunk ids.
			m_chunkSize = m_data.width * m_data.height * m_data.length;
			m_pBlockList = new Block[m_chunkSize];

			u32 index = 0;
			for(u32 i = 0; i < m_data.width; ++i)
			{
				for(u32 k = 0; k < m_data.length; ++k)
				{
					for(u32 j = 0; j < m_data.height; ++j)
					{
						m_pBlockList[index++] = { 0,
							(m_data.offset.y + (int)j) >= -60,
							0 };
					}
				}
			}
		}

		{ // Create mesh container.
			Graphics::CMeshContainer_::Data data { };
			data.onPreRender = std::bind(&CChunk::PreRender, this);
			data.pMeshRenderer = nullptr;
			data.pMaterial = m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matHash));
			m_meshContainer.SetData(data);
			m_meshContainer.Initialize();
			
			data.onPreRender = [](){};//std::bind(&CChunk::PreRenderWire, this);
			data.pMaterial = m_pMaterialWire = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matWireHash));
			m_meshContainerWire.SetData(data);
			m_meshContainerWire.Initialize();
		}

		RebuildMesh();
	}
	
	void CChunk::PreRender()
	{
		if(m_meshFuture[0].Ready() && m_meshFuture[1].Ready())
		{
			if(m_bDirty)
			{
				const u8 nextIndex = (m_meshIndex + 1) & 0x1;
				auto pMesh = m_pMeshRendererList[nextIndex];
				//auto pMeshWire = m_pMeshRendererListWire[nextIndex];
				if(pMesh)
				{
					App::CSceneManager::Instance().Garbage().Dispose([pMesh/*, pMeshWire*/](){ 
						pMesh->Release();
						//pMeshWire->Release();
						delete pMesh;
						//delete pMeshWire;
					});

					m_pMeshRendererList[nextIndex] = nullptr;
					//m_pMeshRendererListWire[nextIndex] = nullptr;
				}

				m_meshContainer.SetMeshRenderer(m_pMeshRendererList[m_meshIndex]);
				//m_meshContainerWire.SetMeshRenderer(m_pMeshRendererListWire[m_meshIndex]);
				m_bDirty = false;

				if(m_bAwaitingRebuild)
				{
					m_bAwaitingRebuild = false;
					RebuildMesh();
				}
			}

			if(!m_blockUpdateMap.empty())
			{
				u8 adjUpdates = 0;

				{ // Update blocks and the indices provided.
					std::lock_guard<std::shared_mutex> lk(m_mutex);

					for(const auto& elem : m_blockUpdateMap)
					{
						u32 i, j, k;
						internalGetCoordsFromIndex(elem.first, i, j, k);
						if(i == 0 && m_pChunkAdj[SIDE_LEFT] && !m_pChunkAdj[SIDE_LEFT]->GetBlock(m_pChunkAdj[SIDE_LEFT]->internalGetIndexRight(j, k)).bEmpty) adjUpdates |= SIDE_FLAG_LEFT;
						if(i == m_data.width - 1 && m_pChunkAdj[SIDE_RIGHT] && !m_pChunkAdj[SIDE_RIGHT]->GetBlock(m_pChunkAdj[SIDE_RIGHT]->internalGetIndexLeft(j, k)).bEmpty) adjUpdates |= SIDE_FLAG_RIGHT;
						if(j == 0 && m_pChunkAdj[SIDE_BOTTOM] && !m_pChunkAdj[SIDE_BOTTOM]->GetBlock(m_pChunkAdj[SIDE_BOTTOM]->internalGetIndexTop(i, k)).bEmpty) adjUpdates |= SIDE_FLAG_BOTTOM;
						if(j == m_data.height - 1 && m_pChunkAdj[SIDE_TOP] && !m_pChunkAdj[SIDE_TOP]->GetBlock(m_pChunkAdj[SIDE_TOP]->internalGetIndexBottom(i, k)).bEmpty) adjUpdates |= SIDE_FLAG_TOP;
						if(k == 0 && m_pChunkAdj[SIDE_BACK] && !m_pChunkAdj[SIDE_BACK]->GetBlock(m_pChunkAdj[SIDE_BACK]->internalGetIndexFront(i, j)).bEmpty) adjUpdates |= SIDE_FLAG_BACK;
						if(k == m_data.length - 1 && m_pChunkAdj[SIDE_FRONT] && !m_pChunkAdj[SIDE_FRONT]->GetBlock(m_pChunkAdj[SIDE_FRONT]->internalGetIndexBack(i, j)).bEmpty) adjUpdates |= SIDE_FLAG_FRONT;

						m_pBlockList[elem.first].bEmpty = elem.second > 0xFF;
						if(!m_pBlockList[elem.first].bEmpty)
						{
							m_pBlockList[elem.first].id = static_cast<u8>(elem.second);
						}
					}

					m_blockUpdateMap.clear();
				}

				// Update the mesh after block updates.
				RebuildMesh();
				u32 index = 0;
				while(adjUpdates)
				{
					if(adjUpdates & 0x1)
					{
						m_pChunkAdj[index]->RebuildMesh();
					}

					adjUpdates >>= 1;
					++index;
				}
			}
		}
	}

	void CChunk::Release()
	{
		m_meshContainer.Release();
		m_meshContainerWire.Release();
		//SAFE_RELEASE_DELETE(m_pMeshRendererListWire[1]);
		//SAFE_RELEASE_DELETE(m_pMeshRendererListWire[0]);
		SAFE_RELEASE_DELETE(m_pMeshRendererList[1]);
		SAFE_RELEASE_DELETE(m_pMeshRendererList[0]);
		
		SAFE_DELETE_ARRAY(m_pBlockList);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Mesh methods.
	//-----------------------------------------------------------------------------------------------
	
	void CChunk::BuildMesh(u8 meshIndex, bool bOptimize)
	{
		Graphics::CMeshData_::Data data { };
		data.topology = Graphics::PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		data.vertexStride = sizeof(Vertex);
		data.indexStride = sizeof(Index);
		
		uintptr_t mutexList[7] = {
			reinterpret_cast<uintptr_t>(&m_mutex),
			reinterpret_cast<uintptr_t>(m_pChunkAdj[0] ? &m_pChunkAdj[0]->m_mutex : nullptr),
			reinterpret_cast<uintptr_t>(m_pChunkAdj[1] ? &m_pChunkAdj[1]->m_mutex : nullptr),
			reinterpret_cast<uintptr_t>(m_pChunkAdj[2] ? &m_pChunkAdj[2]->m_mutex : nullptr),
			reinterpret_cast<uintptr_t>(m_pChunkAdj[3] ? &m_pChunkAdj[3]->m_mutex : nullptr),
			reinterpret_cast<uintptr_t>(m_pChunkAdj[4] ? &m_pChunkAdj[4]->m_mutex : nullptr),
			reinterpret_cast<uintptr_t>(m_pChunkAdj[5] ? &m_pChunkAdj[5]->m_mutex : nullptr)
		};

		std::sort(mutexList, mutexList + 7, [](uintptr_t a, uintptr_t b){ return a > b; });

		{ // Count quads.
			std::unique_lock<std::shared_mutex> lk[7];

			for(int i = 0; i < 7 && mutexList[i] != 0; ++i)
			{
				lk[i] = std::unique_lock<std::shared_mutex>(*reinterpret_cast<std::shared_mutex*>(mutexList[i]));
			}

			u32 index = 0;
			u32 quadCount = 0;

			auto AddQuad = [&](SIDE_FLAG flag){
				++quadCount;
				m_pBlockList[index].sideFlag |= flag;
			};

			for(u32 i = 0; i < m_data.width; ++i)
			{
				for(u32 k = 0; k < m_data.length; ++k)
				{
					for(u32 j = 0; j < m_data.height; ++j)
					{
						m_pBlockList[index].sideFlag = 0;
						if(m_pBlockList[index].bEmpty)
						{
							++index;
							continue;
						}

						// Left.
						if(i == 0)
						{
							if(!m_pChunkAdj[SIDE_LEFT] || m_pChunkAdj[SIDE_LEFT]->m_pBlockList[m_pChunkAdj[SIDE_LEFT]->internalGetIndexRight(j, k)].bEmpty)
							{
								AddQuad(SIDE_FLAG_LEFT);
							}
						}
						else if(m_pBlockList[internalGetIndex(i - 1, j, k)].bEmpty)
						{
							AddQuad(SIDE_FLAG_LEFT);
						}

						// Right.
						if(i == m_data.width - 1)
						{
							if(!m_pChunkAdj[SIDE_RIGHT] || m_pChunkAdj[SIDE_RIGHT]->m_pBlockList[m_pChunkAdj[SIDE_RIGHT]->internalGetIndexLeft(j, k)].bEmpty)
							{
								AddQuad(SIDE_FLAG_RIGHT);
							}
						}
						else if(m_pBlockList[internalGetIndex(i + 1, j, k)].bEmpty)
						{
							AddQuad(SIDE_FLAG_RIGHT);
						}

						// Bottom.
						if(j == 0)
						{
							if(!m_pChunkAdj[SIDE_BOTTOM] || m_pChunkAdj[SIDE_BOTTOM]->m_pBlockList[m_pChunkAdj[SIDE_BOTTOM]->internalGetIndexTop(i, k)].bEmpty)
							{
								AddQuad(SIDE_FLAG_BOTTOM);
							}
						}
						else if(m_pBlockList[internalGetIndex(i, j - 1, k)].bEmpty)
						{
							AddQuad(SIDE_FLAG_BOTTOM);
						}

						// Top.
						if(j == m_data.height - 1)
						{
							if(!m_pChunkAdj[SIDE_TOP] || m_pChunkAdj[SIDE_TOP]->m_pBlockList[m_pChunkAdj[SIDE_TOP]->internalGetIndexBottom(i, k)].bEmpty)
							{
								AddQuad(SIDE_FLAG_TOP);
							}
						}
						else if(m_pBlockList[internalGetIndex(i, j + 1, k)].bEmpty)
						{
							AddQuad(SIDE_FLAG_TOP);
						}

						// Back.
						if(k == 0)
						{
							if(!m_pChunkAdj[SIDE_BACK] || m_pChunkAdj[SIDE_BACK]->m_pBlockList[m_pChunkAdj[SIDE_BACK]->internalGetIndexFront(i, j)].bEmpty)
							{
								AddQuad(SIDE_FLAG_BACK);
							}
						}
						else if(m_pBlockList[internalGetIndex(i, j, k - 1)].bEmpty)
						{
							AddQuad(SIDE_FLAG_BACK);
						}

						// Front.
						if(k == m_data.length - 1)
						{
							if(!m_pChunkAdj[SIDE_FRONT] || m_pChunkAdj[SIDE_FRONT]->m_pBlockList[m_pChunkAdj[SIDE_FRONT]->internalGetIndexBack(i, j)].bEmpty)
							{
								AddQuad(SIDE_FLAG_FRONT);
							}
						}
						else if(m_pBlockList[internalGetIndex(i, j, k + 1)].bEmpty)
						{
							AddQuad(SIDE_FLAG_FRONT);
						}

						++index;
					}
				}
			}

			if(static_cast<u64>(quadCount) * 6 >= std::numeric_limits<u32>().max())
			{
				// Index overflow from counted quads. Force optimization in an attempt to bring indices back within numerical bounds.
				bOptimize = true;
			}
			else
			{
				data.vertexCount = quadCount << 2;
				data.indexCount = quadCount * 6;
			}
		}

		if(bOptimize)
		{
			GenerateOptimalMeshData(meshIndex, data);
		}
		else
		{
			m_meshData[meshIndex].SetData(data);
			m_meshData[meshIndex].Initialize();

			{ // Generate vertex and index data.
				std::shared_lock<std::shared_mutex> lk[7];

				for(int i = 0; i < 7 && mutexList[i] != 0; ++i)
				{
					lk[i] = std::shared_lock<std::shared_mutex>(*reinterpret_cast<std::shared_mutex*>(mutexList[i]));
				}

				u32 vIndex = 0;
				u32 iIndex = 0;

				auto GenerateQuad = [&](BlockId id, const Math::Vector3& offset, const Math::Vector3& center, const Math::Vector3& right, const Math::Vector3& up, const Math::Vector3& normal){
					*(Index*)m_meshData[meshIndex].GetIndexAt(iIndex++) = vIndex;
					*(Index*)m_meshData[meshIndex].GetIndexAt(iIndex++) = vIndex + 1;
					*(Index*)m_meshData[meshIndex].GetIndexAt(iIndex++) = vIndex + 2;
					*(Index*)m_meshData[meshIndex].GetIndexAt(iIndex++) = vIndex;
					*(Index*)m_meshData[meshIndex].GetIndexAt(iIndex++) = vIndex + 2;
					*(Index*)m_meshData[meshIndex].GetIndexAt(iIndex++) = vIndex + 3;

					static const float u = 1.0f / 8.0f;
					static const float v = 1.0f / 32.0f;
					const BlockId i = id % 8;
					const BlockId j = id / 8;

					const Math::Vector2 uv = Math::Vector2(u * i, v * j);

					const Math::Vector3 v0 = center + (-right - up + normal) * 0.5f;
					const Math::Vector3 v1 = center + ( right - up + normal) * 0.5f;
					const Math::Vector3 v2 = center + ( right + up + normal) * 0.5f;
					const Math::Vector3 v3 = center + (-right + up + normal) * 0.5f;

					const Math::Vector4 uv0(Math::Vector3::Dot(right, v0), Math::Vector3::Dot(up, v0), uv.x, uv.y);
					const Math::Vector4 uv1(Math::Vector3::Dot(right, v1), Math::Vector3::Dot(up, v1), uv.x, uv.y);
					const Math::Vector4 uv2(Math::Vector3::Dot(right, v2), Math::Vector3::Dot(up, v2), uv.x, uv.y);
					const Math::Vector4 uv3(Math::Vector3::Dot(right, v3), Math::Vector3::Dot(up, v3), uv.x, uv.y);

					*(Vertex*)m_meshData[meshIndex].GetVertexAt(vIndex++) = { { offset + v0 * m_data.blockSize }, normal, right, uv0 };
					*(Vertex*)m_meshData[meshIndex].GetVertexAt(vIndex++) = { { offset + v1 * m_data.blockSize }, normal, right, uv1 };
					*(Vertex*)m_meshData[meshIndex].GetVertexAt(vIndex++) = { { offset + v2 * m_data.blockSize }, normal, right, uv2 };
					*(Vertex*)m_meshData[meshIndex].GetVertexAt(vIndex++) = { { offset + v3 * m_data.blockSize }, normal, right, uv3 };
				};

				const Math::Vector3 offset(
					static_cast<float>(m_data.offset.x) * m_data.blockSize,
					static_cast<float>(m_data.offset.y) * m_data.blockSize,
					static_cast<float>(m_data.offset.z) * m_data.blockSize
				);

				u32 index = 0;
				for(u32 i = 0; i < m_data.width; ++i)
				{
					for(u32 k = 0; k < m_data.length; ++k)
					{
						for(u32 j = 0; j < m_data.height; ++j)
						{
							const Block block = m_pBlockList[index++];
							if(block.bEmpty) continue;

							Math::Vector3 center(i + 0.5f, j + 0.5f, k + 0.5f);

							
							// Left.
							if(i == 0)
							{
								if(!m_pChunkAdj[SIDE_LEFT] || m_pChunkAdj[SIDE_LEFT]->m_pBlockList[m_pChunkAdj[SIDE_LEFT]->internalGetIndexRight(j, k)].bEmpty)
								{
									GenerateQuad(block.id, offset, center, Math::VEC3_BACKWARD, Math::VEC3_UP, Math::VEC3_LEFT);
								}
							}
							else if(m_pBlockList[internalGetIndex(i - 1, j, k)].bEmpty)
							{
								GenerateQuad(block.id, offset, center, Math::VEC3_BACKWARD, Math::VEC3_UP, Math::VEC3_LEFT);
							}

							// Right.
							if(i == m_data.width - 1)
							{
								if(!m_pChunkAdj[SIDE_RIGHT] || m_pChunkAdj[SIDE_RIGHT]->m_pBlockList[m_pChunkAdj[SIDE_RIGHT]->internalGetIndexLeft(j, k)].bEmpty)
								{
									GenerateQuad(block.id, offset, center, Math::VEC3_FORWARD, Math::VEC3_UP, Math::VEC3_RIGHT);
								}
							}
							else if(m_pBlockList[internalGetIndex(i + 1, j, k)].bEmpty)
							{
								GenerateQuad(block.id, offset, center, Math::VEC3_FORWARD, Math::VEC3_UP, Math::VEC3_RIGHT);
							}

							// Bottom.
							if(j == 0)
							{
								if(!m_pChunkAdj[SIDE_BOTTOM] || m_pChunkAdj[SIDE_BOTTOM]->m_pBlockList[m_pChunkAdj[SIDE_BOTTOM]->internalGetIndexTop(i, k)].bEmpty)
								{
									GenerateQuad(block.id, offset, center, Math::VEC3_RIGHT, Math::VEC3_BACKWARD, Math::VEC3_DOWN);
								}
							}
							else if(m_pBlockList[internalGetIndex(i, j - 1, k)].bEmpty)
							{
								GenerateQuad(block.id, offset, center, Math::VEC3_RIGHT, Math::VEC3_BACKWARD, Math::VEC3_DOWN);
							}

							// Top.
							if(j == m_data.height - 1)
							{
								if(!m_pChunkAdj[SIDE_TOP] || m_pChunkAdj[SIDE_TOP]->m_pBlockList[m_pChunkAdj[SIDE_TOP]->internalGetIndexBottom(i, k)].bEmpty)
								{
									GenerateQuad(block.id, offset, center, Math::VEC3_RIGHT, Math::VEC3_FORWARD, Math::VEC3_UP);
								}
							}
							else if(m_pBlockList[internalGetIndex(i, j + 1, k)].bEmpty)
							{
								GenerateQuad(block.id, offset, center, Math::VEC3_RIGHT, Math::VEC3_FORWARD, Math::VEC3_UP);
							}

							// Back.
							if(k == 0)
							{
								if(!m_pChunkAdj[SIDE_BACK] || m_pChunkAdj[SIDE_BACK]->m_pBlockList[m_pChunkAdj[SIDE_BACK]->internalGetIndexFront(i, j)].bEmpty)
								{
									GenerateQuad(block.id, offset, center, Math::VEC3_RIGHT, Math::VEC3_UP, Math::VEC3_BACKWARD);
								}
							}
							else if(m_pBlockList[internalGetIndex(i, j, k - 1)].bEmpty)
							{
								GenerateQuad(block.id, offset, center, Math::VEC3_RIGHT, Math::VEC3_UP, Math::VEC3_BACKWARD);
							}

							// Front.
							if(k == m_data.length - 1)
							{
								if(!m_pChunkAdj[SIDE_FRONT] || m_pChunkAdj[SIDE_FRONT]->m_pBlockList[m_pChunkAdj[SIDE_FRONT]->internalGetIndexBack(i, j)].bEmpty)
								{
									GenerateQuad(block.id, offset, center, Math::VEC3_LEFT, Math::VEC3_UP, Math::VEC3_FORWARD);
								}
							}
							else if(m_pBlockList[internalGetIndex(i, j, k + 1)].bEmpty)
							{
								GenerateQuad(block.id, offset, center, Math::VEC3_LEFT, Math::VEC3_UP, Math::VEC3_FORWARD);
							}
						}
					}
				}
			}
		}

		{ // Create the mesh renderer.
			m_pMeshRendererList[meshIndex] = CFactory::Instance().CreateMeshRenderer(m_pObject); // TODO: Create a pool of these renderers managed by CChunkManager.

			Graphics::CMeshRenderer_::Data data { };
			data.bSkipRegistration = true;
			data.pMaterial = m_pMaterial;
			data.pMeshData = &m_meshData[meshIndex];
			m_pMeshRendererList[meshIndex]->SetData(data);

			m_pMeshRendererList[meshIndex]->Initialize();
		}

		/*{ // Create the mesh wire renderer.
			m_pMeshRendererListWire[meshIndex] = CFactory::Instance().CreateMeshRenderer(m_pObject);

			Graphics::CMeshRenderer_::Data data { };
			data.bSkipRegistration = true;
			data.pMaterial = m_pMaterialWire;
			data.pMeshData = &m_meshData[meshIndex];
			m_pMeshRendererListWire[meshIndex]->SetData(data);

			m_pMeshRendererListWire[meshIndex]->Initialize();
		}*/
		
		m_meshData[meshIndex].Release();
	}

	void CChunk::ProcessIsland(std::unordered_set<u32>& set, std::unordered_set<u32>::iterator it, u32 iStep, u32 kStep, u32 maxIndex, u32 iAxis, u32 kAxis,
		const QuadSides& flags, const QuadEdges& edges, std::unordered_map<u32, u64>& disjointedSet)
	{
		// Sub-Lambda for adding edge to disjointed set.
		auto AddEdge = [&](u32 a, u32 b){
			auto elem = disjointedSet.find(a);
			if(elem != disjointedSet.end())
			{
				elem->second <<= 32;
				elem->second |= b;
			}
			else
			{
				disjointedSet.insert({ a, b });
			}
		};

		u32 chunkSize[3] = { m_data.width, m_data.height, m_data.length };

		std::queue<u32> q;
		q.push(*it);

		while(!q.empty())
		{
			auto iter = set.find(q.front());
			q.pop();

			if(iter == set.end()) continue;

			const u32 index = *iter;
			// Erase quad from set so it isn't revisited.
			set.erase(iter);
			
			u32 indices[3];
			internalGetCoordsFromIndex(index, indices[0], indices[1], indices[2]);

			// Add edges of quad to disjointed set.
			if((m_pBlockList[index].sideFlag & flags.v[1]) || indices[iAxis] == 0 || m_pBlockList[index - iStep].id != m_pBlockList[index].id || !(m_pBlockList[index - iStep].sideFlag & flags.v[0])) 
			{
				AddEdge(internalGetVertexIndex(indices[0] + (u32)edges.e[0].e0.x, indices[1] + (u32)edges.e[0].e0.y, indices[2] + (u32)edges.e[0].e0.z),
					internalGetVertexIndex(indices[0] + (u32)edges.e[0].e1.x, indices[1] + (u32)edges.e[0].e1.y, indices[2] + (u32)edges.e[0].e1.z));
			}

			if((m_pBlockList[index].sideFlag & flags.v[2]) || indices[iAxis] == chunkSize[iAxis] - 1 || m_pBlockList[index + iStep].id != m_pBlockList[index].id || !(m_pBlockList[index + iStep].sideFlag & flags.v[0]))
			{
				AddEdge(internalGetVertexIndex(indices[0] + (u32)edges.e[1].e0.x, indices[1] + (u32)edges.e[1].e0.y, indices[2] + (u32)edges.e[1].e0.z),
					internalGetVertexIndex(indices[0] + (u32)edges.e[1].e1.x, indices[1] + (u32)edges.e[1].e1.y, indices[2] + (u32)edges.e[1].e1.z));
			}

			if((m_pBlockList[index].sideFlag & flags.v[3]) || indices[kAxis] == 0 || m_pBlockList[index - kStep].id != m_pBlockList[index].id || !(m_pBlockList[index - kStep].sideFlag & flags.v[0]))
			{
				AddEdge(internalGetVertexIndex(indices[0] + (u32)edges.e[2].e0.x, indices[1] + (u32)edges.e[2].e0.y, indices[2] + (u32)edges.e[2].e0.z),
					internalGetVertexIndex(indices[0] + (u32)edges.e[2].e1.x, indices[1] + (u32)edges.e[2].e1.y, indices[2] + (u32)edges.e[2].e1.z));
			}

			if((m_pBlockList[index].sideFlag & flags.v[4]) || indices[kAxis] == chunkSize[kAxis] - 1 || m_pBlockList[index + kStep].id != m_pBlockList[index].id || !(m_pBlockList[index + kStep].sideFlag & flags.v[0]))
			{
				AddEdge(internalGetVertexIndex(indices[0] + (u32)edges.e[3].e0.x, indices[1] + (u32)edges.e[3].e0.y, indices[2] + (u32)edges.e[3].e0.z),
					internalGetVertexIndex(indices[0] + (u32)edges.e[3].e1.x, indices[1] + (u32)edges.e[3].e1.y, indices[2] + (u32)edges.e[3].e1.z));
			}

			if(indices[iAxis] > 0)
			{ // Left.
				q.push(index - iStep);
			}
			if(indices[iAxis] < chunkSize[iAxis] - 1)
			{ // Right.
				q.push(index + iStep);
			}

			if(indices[kAxis] > 0)
			{ // Back.
				q.push(index - kStep);
			}
			if(indices[kAxis] < chunkSize[kAxis] - 1)
			{ // Front.
				q.push(index + kStep);
			}
		}
	}

	void CChunk::GenerateOptimalMeshData(u8 meshIndex, Graphics::CMeshData_::Data& data)
	{
		const Math::Vector3 offset(
			static_cast<float>(m_data.offset.x) * m_data.blockSize,
			static_cast<float>(m_data.offset.y) * m_data.blockSize,
			static_cast<float>(m_data.offset.z) * m_data.blockSize
		);

		std::vector<Vertex> vertexList;
		std::vector<Index> indexList;

		std::unordered_map<BlockId, std::unordered_set<u32>> blockMap[2];
		std::unordered_map<u32, u64> disjointedSet;

		// Lambda for triangulating a planar adjacency list.
		auto Triangulate = [this, &vertexList, &indexList, &offset](const std::vector<Math::Vector3>& pointList, const Math::Vector3& normal, const Math::Vector3& tangent, const Math::Vector3& bitangent, BlockId id){
			std::unordered_map<u32, u32> vertexMap;

			int n = static_cast<int>(pointList.size());
			short prev[4096];
			short next[4096];
			for(int i = 0; i < n; ++i)
			{
				prev[i] = i - 1;
				next[i] = i + 1;
			}

			prev[0] = n - 1;
			next[n - 1] = 0;

			int i = 0;
			while(n >= 3)
			{
				int isEar = 1;
				
				if(n > 3)
				{
					const Math::Vector3 v0 = pointList[prev[i]];
					const Math::Vector3 v1 = pointList[i];
					const Math::Vector3 v2 = pointList[next[i]];

					const Math::Vector3 e0 = v2 - v1;
					const Math::Vector3 e1 = v1 - v0;
					const Math::Vector3 perp = Math::Vector3::Cross(e0, e1);

					// Is CCW?
					if(Math::Vector3::Dot(perp, normal) > 1e-5f)
					{
						int k = next[next[i]];
						do
						{
							auto TestPointK = [&](){

								if((pointList[k] - v0).LengthSq() < 1e-5f) return false;
								if((pointList[k] - v1).LengthSq() < 1e-5f) return false;
								if((pointList[k] - v2).LengthSq() < 1e-5f) return false;
								
								if(Math::Vector3::Dot(pointList[k] - v1, Math::Vector3::Cross(e0, normal)) < 0.0f) return false;
								if(Math::Vector3::Dot(pointList[k] - v0, Math::Vector3::Cross(e1, normal)) < 0.0f) return false;
								if(Math::Vector3::Dot(pointList[k] - v2, Math::Vector3::Cross(v0 - v2, normal)) < 0.0f) return false;

								return true;
							};

							if(TestPointK())
							{
								isEar = 0;
								break;
							}

							k = next[k];
						} while(k != prev[i]);
					}
					else
					{
						isEar = 0;
					}
				}

				if(isEar)
				{
					// Output triangle.
					static const float u = 1.0f / 8.0f;
					static const float v = 1.0f / 32.0f;
					const Math::Vector2 uv = Math::Vector2(u * (id % 8), v * (id / 8));

					const Math::Vector3 vList[3] = {
						pointList[prev[i]],
						pointList[i],
						pointList[next[i]]
					};

					const u32 iList[3] = {
						internalGetVertexIndex(static_cast<u32>(vList[0].x), static_cast<u32>(vList[0].y), static_cast<u32>(vList[0].z)),
						internalGetVertexIndex(static_cast<u32>(vList[1].x), static_cast<u32>(vList[1].y), static_cast<u32>(vList[1].z)),
						internalGetVertexIndex(static_cast<u32>(vList[2].x), static_cast<u32>(vList[2].y), static_cast<u32>(vList[2].z))
					};

					std::unordered_map<u32, u32>::iterator it[3] = {
						vertexMap.find(iList[0]),
						vertexMap.find(iList[1]),
						vertexMap.find(iList[2]),
					};

					for(size_t j = 0; j < 3; ++j)
					{
						if(it[j] == vertexMap.end())
						{
							it[j] = vertexMap.insert({ iList[j], static_cast<u32>(vertexList.size()) }).first;
							Math::Vector4 texCoord(Math::Vector3::Dot(tangent, vList[j]), Math::Vector3::Dot(bitangent, vList[j]), uv.x, uv.y);
							vertexList.push_back({ offset + vList[j] * m_data.blockSize, normal, tangent, texCoord });
						}

						indexList.push_back(it[j]->second);
					}

					next[prev[i]] = next[i];
					prev[next[i]] = prev[i];
					--n;

					i = prev[i];
				}
				else
				{
					i = next[i];
				}
			}
		};

		auto ProcessAdjList = [&](const Math::Vector3& normal, const Math::Vector3& tangent, const Math::Vector3& bitangent, BlockId id){
			
			float closestDot = FLT_MAX;
			size_t closestList = 0;
			size_t closestVertex = 0;

			// Create point lists out of the disjointed set created for the current island.
			std::vector<std::vector<Math::Vector3>> pointLists;
			while(!disjointedSet.empty())
			{
				std::vector<Math::Vector3> pointList;
				u32 index = disjointedSet.begin()->first;
				u32 i0, j0, k0;
				while(!disjointedSet.empty())
				{
					internalGetCoordsFromVertexIndex(index, i0, j0, k0);
					const Math::Vector3 vertex = { static_cast<float>(i0), static_cast<float>(j0), static_cast<float>(k0) };

					auto vertexIter = disjointedSet.find(index);
					if(vertexIter == disjointedSet.end()) break;

					if(vertexIter->second > std::numeric_limits<u32>::max())
					{
						index = static_cast<u32>(vertexIter->second & 0xFFFFFFFF);

						if(pointList.empty())
						{
							vertexIter->second >>= 32;
						}
						else
						{
							u32 i1, j1, k1;
							internalGetCoordsFromVertexIndex(index, i1, j1, k1);
							const Math::Vector3 nextVertex = { static_cast<float>(i1), static_cast<float>(j1), static_cast<float>(k1) };
							if(Math::Vector3::Dot(Math::Vector3::Cross(nextVertex - vertex, vertex - pointList.back()), normal) > 0.0f)
							{ // CCW
								vertexIter->second >>= 32;
							}
							else
							{ // CW, so swap the order.
								index = static_cast<u32>(vertexIter->second >> 32);
								vertexIter->second &= 0xFFFFFFFF;
							}
						}
					}
					else
					{
						index = static_cast<u32>(vertexIter->second);
						disjointedSet.erase(vertexIter);
					}

					pointList.push_back(vertex);
				}

				auto TestCollinear = [&](const Math::Vector3& left, const Math::Vector3& middle, const Math::Vector3& right){
					auto diff0 = right - middle;
					auto diff1 = middle - left;
					return fabsf(Math::Vector3::Dot(diff0, diff1) - diff0.Length() * diff1.Length()) < 1e-5f;
				};

				size_t ptOffset = 0;
				while(pointList.size() > ptOffset + 1)
				{
					auto diff0 = pointList[ptOffset + 1] - pointList[ptOffset];
					auto diff1 = pointList[ptOffset] - pointList.back();
					if(TestCollinear(pointList.back(), pointList[ptOffset], pointList[ptOffset + 1]))
					{
						++ptOffset;
					}
					else
					{
						break;
					}
				}

				auto AddPoint = [&](const Math::Vector3 point){
					const float dot = Math::Vector3::Dot(tangent, point);
					if(dot < closestDot)
					{
						closestList = pointLists.size() - 1;
						closestVertex = pointLists.back().size();
						closestDot = dot;
					}

					pointLists.back().push_back(point);
				};

				if(ptOffset + 2 < pointList.size())
				{
					pointLists.push_back({ });
					AddPoint(pointList[ptOffset]);

					for(size_t pt = ptOffset + 1; pt < pointList.size(); ++pt)
					{
						if(pt + 1 < pointList.size())
						{
							if(!TestCollinear(pointLists.back().back(), pointList[pt], pointList[pt + 1]))
							{
								AddPoint(pointList[pt]);
							}
						}
						else
						{
							if(!TestCollinear(pointLists.back().back(), pointList[pt], pointLists.back().front()))
							{
								AddPoint(pointList[pt]);
							}
						}
					}
				}
			}

			const size_t mergedListIndex = closestList;
			std::unordered_set<size_t> mergedLists;
			mergedLists.insert(closestList);
			while(mergedLists.size() < pointLists.size())
			{
				// Find point in remaining point lists that is closest to direction ONE.
				closestDot = FLT_MAX;
				for(size_t l = 0; l < pointLists.size(); ++l)
				{
					if(mergedLists.find(l) != mergedLists.end()) continue;

					for(size_t v = 0; v < pointLists[l].size(); ++v)
					{
						const float dot = Math::Vector3::Dot(tangent, pointLists[l][v]);
						if(dot < closestDot)
						{
							closestList = l;
							closestVertex = v;
							closestDot = dot;
						}
					}
				}

				float closestDist = FLT_MAX;
				size_t closestMergedVertex = 0;

				// Find point in merge point list that is closest to that point.
				for(size_t v = 0; v < pointLists[mergedListIndex].size(); ++v)
				{
					const auto diff = pointLists[mergedListIndex][v] - pointLists[closestList][closestVertex];
					float dot = Math::Vector3::Dot(-tangent, diff);
					if(dot < 1e-5f) continue;

					const float dist = Math::Vector3::LengthSq(diff);

					if(dist <= closestDist)
					{
						size_t prevV = v == 0 ? pointLists[mergedListIndex].size() - 1 : v - 1;
						
						if(closestDist != FLT_MAX && pointLists[mergedListIndex][closestMergedVertex] == pointLists[mergedListIndex][v])
						{
							// Make sure that when connecting point lists that we're creating CCW triangles.
							if(Math::Vector3::Dot(normal, Math::Vector3::Cross(pointLists[mergedListIndex][v] - pointLists[mergedListIndex][prevV], diff)) < 0.0f)
							{
								continue;
							}
						}
						
						{
							size_t nextV = v == pointLists[mergedListIndex].size() - 1 ? 0 : v + 1;

							const Math::Vector3 e0 = pointLists[mergedListIndex][v] - pointLists[mergedListIndex][prevV];
							const Math::Vector3 e1 = pointLists[mergedListIndex][v] - pointLists[mergedListIndex][nextV];
							const Math::Vector3 n0 = Math::Vector3::Cross(e0, normal);
							const Math::Vector3 n1 = Math::Vector3::Cross(normal, e1);

							if(Math::Vector3::Dot(normal, Math::Vector3::Cross(e0, e1)) > 0.0f)
							{ // CCW
								if(Math::Vector3::Dot(n0, diff) > 0.0f) continue;
								if(Math::Vector3::Dot(n1, diff) > 0.0f) continue;
							}
							else if(Math::Vector3::Dot(e0, e1) > -1e-5f)
							{ // CW, beyond ~270 degrees. This extension is important for the case of holes in the geometry.
								if(Math::Vector3::Dot(n0, diff) < 0.0f && Math::Vector3::Dot(n1, diff) < 0.0f) continue;
							}
						}

						closestMergedVertex = v;
						closestDist = dist;
					}
				}

				const size_t lastSize = pointLists[mergedListIndex].size();
				pointLists[mergedListIndex].resize(lastSize + pointLists[closestList].size() + 2);

				// Shift ending elements to the new end of the merged list.
				for(size_t v = lastSize - 1; v > closestMergedVertex; --v)
				{
					pointLists[mergedListIndex][v + pointLists[closestList].size() + 2] = pointLists[mergedListIndex][v];
				}

				// Merge points.
				for(size_t v = 1; v <= pointLists[closestList].size() + 1; ++v)
				{
					pointLists[mergedListIndex][closestMergedVertex + v] = pointLists[closestList][closestVertex];
					if(++closestVertex >= pointLists[closestList].size())
					{
						closestVertex = 0;
					}
				}

				pointLists[mergedListIndex][closestMergedVertex + pointLists[closestList].size() + 2] = pointLists[mergedListIndex][closestMergedVertex];

				mergedLists.insert(closestList);
			}

			// Build island mesh data out of completed adjacency list.
			Triangulate(pointLists[mergedListIndex], normal, tangent, bitangent, id);
		};

		// X-Axis.
		for(u32 i = 0, index = 0; i < m_data.width; ++i)
		{
			blockMap[0].clear();
			blockMap[1].clear();
			//u32 index = i * m_data.length * m_data.height;

			for(u32 k = 0; k < m_data.length; ++k)
			{
				for(u32 j = 0; j < m_data.height; ++j)
				{
					if(!m_pBlockList[index].bEmpty)
					{
						if(m_pBlockList[index].sideFlag & SIDE_FLAG_LEFT)
						{
							blockMap[0][m_pBlockList[index].id].insert(index);
						}
						if(m_pBlockList[index].sideFlag & SIDE_FLAG_RIGHT)
						{
							blockMap[1][m_pBlockList[index].id].insert(index);
						}
					}

					++index;
				}
			}

			// Left.
			for(auto elem : blockMap[0])
			{
				disjointedSet.clear();
				while(!elem.second.empty())
				{
					ProcessIsland(elem.second, elem.second.begin(), m_data.height, 1, m_chunkSize, 2, 1,
						{ SIDE_FLAG_LEFT, SIDE_FLAG_BACK, SIDE_FLAG_FRONT, SIDE_FLAG_BOTTOM, SIDE_FLAG_TOP },
						{ 
							Math::VectorInt3(0, 0, 0), Math::VectorInt3(0, 1, 0),
							Math::VectorInt3(0, 1, 1), Math::VectorInt3(0, 0, 1),
							Math::VectorInt3(0, 0, 1), Math::VectorInt3(0, 0, 0),
							Math::VectorInt3(0, 1, 0), Math::VectorInt3(0, 1, 1)
						}, disjointedSet);

					ProcessAdjList(Math::VEC3_LEFT, Math::VEC3_BACKWARD, Math::VEC3_UP, elem.first);
				}
			}

			// Right.
			for(auto elem : blockMap[1])
			{
				disjointedSet.clear();
				while(!elem.second.empty())
				{
					ProcessIsland(elem.second, elem.second.begin(), m_data.height, 1, m_chunkSize, 2, 1,
						{ SIDE_FLAG_RIGHT, SIDE_FLAG_BACK, SIDE_FLAG_FRONT, SIDE_FLAG_BOTTOM, SIDE_FLAG_TOP },
						{ 
							Math::VectorInt3(1, 1, 0), Math::VectorInt3(1, 0, 0),
							Math::VectorInt3(1, 0, 1), Math::VectorInt3(1, 1, 1),
							Math::VectorInt3(1, 0, 0), Math::VectorInt3(1, 0, 1),
							Math::VectorInt3(1, 1, 1), Math::VectorInt3(1, 1, 0)
						}, disjointedSet);

					ProcessAdjList(Math::VEC3_RIGHT, Math::VEC3_FORWARD, Math::VEC3_UP, elem.first);
				}
			}
		}

		// Y-Axis.
		for(u32 j = 0; j < m_data.height; ++j)
		{
			blockMap[0].clear();
			blockMap[1].clear();
			u32 index = j;

			for(u32 i = 0; i < m_data.width; ++i)
			{
				for(u32 k = 0; k < m_data.length; ++k)
				{
					if(!m_pBlockList[index].bEmpty)
					{
						if(m_pBlockList[index].sideFlag & SIDE_FLAG_BOTTOM)
						{
							blockMap[0][m_pBlockList[index].id].insert(index);
						}
						if(m_pBlockList[index].sideFlag & SIDE_FLAG_TOP)
						{
							blockMap[1][m_pBlockList[index].id].insert(index);
						}
					}

					index += m_data.height;
				}
			}

			// Bottom.
			for(auto elem : blockMap[0])
			{
				disjointedSet.clear();
				while(!elem.second.empty())
				{
					ProcessIsland(elem.second, elem.second.begin(), m_data.length * m_data.height, m_data.height, m_chunkSize, 0, 2,
						{ SIDE_FLAG_BOTTOM, SIDE_FLAG_LEFT, SIDE_FLAG_RIGHT, SIDE_FLAG_BACK, SIDE_FLAG_FRONT },
						{ 
							Math::VectorInt3(0, 0, 0), Math::VectorInt3(0, 0, 1),
							Math::VectorInt3(1, 0, 1), Math::VectorInt3(1, 0, 0),
							Math::VectorInt3(1, 0, 0), Math::VectorInt3(0, 0, 0),
							Math::VectorInt3(0, 0, 1), Math::VectorInt3(1, 0, 1)
						}, disjointedSet);

					ProcessAdjList(Math::VEC3_DOWN, Math::VEC3_RIGHT, Math::VEC3_BACKWARD, elem.first);
				}
			}

			// Top.
			for(auto elem : blockMap[1])
			{
				disjointedSet.clear();
				while(!elem.second.empty())
				{
					ProcessIsland(elem.second, elem.second.begin(), m_data.length * m_data.height, m_data.height, m_chunkSize, 0, 2,
						{ SIDE_FLAG_TOP, SIDE_FLAG_LEFT, SIDE_FLAG_RIGHT, SIDE_FLAG_BACK, SIDE_FLAG_FRONT },
						{ 
							Math::VectorInt3(0, 1, 1), Math::VectorInt3(0, 1, 0),
							Math::VectorInt3(1, 1, 0), Math::VectorInt3(1, 1, 1),
							Math::VectorInt3(0, 1, 0), Math::VectorInt3(1, 1, 0),
							Math::VectorInt3(1, 1, 1), Math::VectorInt3(0, 1, 1)
						}, disjointedSet);

					ProcessAdjList(Math::VEC3_UP, Math::VEC3_RIGHT, Math::VEC3_FORWARD, elem.first);
				}
			}
		}

		// Z-Axis.
		for(u32 k = 0; k < m_data.length; ++k)
		{
			blockMap[0].clear();
			blockMap[1].clear();

			for(u32 i = 0; i < m_data.width; ++i)
			{
				u32 index = i * m_data.length * m_data.height + k * m_data.height;

				for(u32 j = 0; j < m_data.height; ++j)
				{
					if(!m_pBlockList[index].bEmpty)
					{
						if(m_pBlockList[index].sideFlag & SIDE_FLAG_BACK)
						{
							blockMap[0][m_pBlockList[index].id].insert(index);
						}
						if(m_pBlockList[index].sideFlag & SIDE_FLAG_FRONT)
						{
							blockMap[1][m_pBlockList[index].id].insert(index);
						}
					}

					++index;
				}
			}

			// Back.
			for(auto elem : blockMap[0])
			{
				disjointedSet.clear();
				while(!elem.second.empty())
				{
					ProcessIsland(elem.second, elem.second.begin(), m_data.length * m_data.height, 1, m_chunkSize, 0, 1,
						{ SIDE_FLAG_BACK, SIDE_FLAG_LEFT, SIDE_FLAG_RIGHT, SIDE_FLAG_BOTTOM, SIDE_FLAG_TOP },
						{ 
							Math::VectorInt3(0, 1, 0), Math::VectorInt3(0, 0, 0),
							Math::VectorInt3(1, 0, 0), Math::VectorInt3(1, 1, 0),
							Math::VectorInt3(0, 0, 0), Math::VectorInt3(1, 0, 0),
							Math::VectorInt3(1, 1, 0), Math::VectorInt3(0, 1, 0)
						}, disjointedSet);

					ProcessAdjList(Math::VEC3_BACKWARD, Math::VEC3_RIGHT, Math::VEC3_UP, elem.first);
				}
			}

			// Front.
			for(auto elem : blockMap[1])
			{
				disjointedSet.clear();
				while(!elem.second.empty())
				{
					ProcessIsland(elem.second, elem.second.begin(), m_data.length * m_data.height, 1, m_chunkSize, 0, 1,
						{ SIDE_FLAG_FRONT, SIDE_FLAG_LEFT, SIDE_FLAG_RIGHT, SIDE_FLAG_BOTTOM, SIDE_FLAG_TOP },
						{ 
							Math::VectorInt3(0, 0, 1), Math::VectorInt3(0, 1, 1),
							Math::VectorInt3(1, 1, 1), Math::VectorInt3(1, 0, 1),
							Math::VectorInt3(1, 0, 1), Math::VectorInt3(0, 0, 1),
							Math::VectorInt3(0, 1, 1), Math::VectorInt3(1, 1, 1)
						}, disjointedSet);

					ProcessAdjList(Math::VEC3_FORWARD, Math::VEC3_LEFT, Math::VEC3_UP, elem.first);
				}
			}
		}

		data.vertexCount = static_cast<u32>(vertexList.size());
		data.indexCount = static_cast<u32>(indexList.size());

		m_meshData[meshIndex].SetData(data);
		m_meshData[meshIndex].Initialize();

		m_meshData[meshIndex].ProcessVertexList([&vertexList](u32 index, u8* pData){
			*(Vertex*)pData = vertexList[index];
		});

		m_meshData[meshIndex].ProcessIndexList([&indexList](u32 index, u8* pData){
			*(Index*)pData = indexList[index];
		});
	}

	void CChunk::RebuildMesh()
	{
		if(m_bDirty)
		{
			m_bAwaitingRebuild = true;
			return;
		}
		
		m_bDirty = true;
		u8 meshIndex = m_meshIndex = (m_meshIndex + 1) & 0x1;

		m_meshFuture[meshIndex] = Util::CJobSystem::Instance().JobGraphics([meshIndex, this](){
			BuildMesh(meshIndex, true);
		}, true);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	u16 CChunk::UpdateBlock(u32 index, u16 id)
	{
		// Find last block id at data.index, and update it to the new value in the queue map.
		u16 lastId;
		auto elem = m_blockUpdateMap.find(index);
		if(elem != m_blockUpdateMap.end())
		{
			lastId = elem->second;
			elem->second = id;
		}
		else
		{
			Block lastBlock;
			{ // Get last block from block list.
				std::shared_lock<std::shared_mutex> lk(m_mutex);
				lastBlock = m_pBlockList[index];
			}

			lastId = lastBlock.bEmpty ? 256 : lastBlock.id;
			m_blockUpdateMap.insert({ index, id });
		}

		return lastId;
	}
	
	void CChunk::Set(const Block* pBlocks)
	{
		{
			std::lock_guard<std::shared_mutex> lk(m_mutex);
			memcpy(m_pBlockList, pBlocks, sizeof(Block) * m_chunkSize);
		}

		RebuildMesh();
	}
	
	void CChunk::Set(std::function<void(Block*, size_t)> func)
	{
		{
			std::lock_guard<std::shared_mutex> lk(m_mutex);
			func(m_pBlockList, m_chunkSize);
		}

		RebuildMesh();
	}

	void CChunk::Read(std::function<void(const Block*, size_t)> func)
	{
		std::lock_guard<std::shared_mutex> lk(m_mutex);
		func(m_pBlockList, m_chunkSize);
	}
	
	void CChunk::Reset()
	{
		{
			std::lock_guard<std::shared_mutex> lk(m_mutex);
		
			u32 index = 0;
			for(u32 i = 0; i < m_data.width; ++i)
			{
				for(u32 k = 0; k < m_data.length; ++k)
				{
					for(u32 j = 0; j < m_data.height; ++j)
					{
						m_pBlockList[index++] = { 0,
							(m_data.offset.y + (int)j) >= -60,
							0 };
					}
				}
			}
		}

		RebuildMesh();
	}

	void CChunk::Clear()
	{
		{
			std::lock_guard<std::shared_mutex> lk(m_mutex);
			u32 index = 0;
			for(u32 i = 0; i < m_data.width; ++i)
			{
				for(u32 k = 0; k < m_data.length; ++k)
				{
					for(u32 j = 0; j < m_data.height; ++j)
					{
						m_pBlockList[index++] = { 0, true };
					}
				}
			}
		}

		RebuildMesh();
	}
};
