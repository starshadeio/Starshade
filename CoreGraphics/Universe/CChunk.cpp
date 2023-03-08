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
		m_bUpdateQueued(false),
		m_meshIndex(0),
		m_lodLevel(0),
		m_lodLevelMax(0),
		m_lodLevelOffset(0),
		m_chunkSize(0),
		m_blockListSize(0),
		m_meshData{ pObject, pObject },
		m_meshContainer(pObject),
		//m_meshContainerWire(pObject),
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
	
	void CChunk::Setup()
	{
		m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matHash));
		//m_pMaterialWire = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matWireHash));

		{ // Create mesh container.
			Graphics::CMeshContainer_::Data data { };
			data.bSkipRegistration = true;
			//data.onPreRender = std::bind(&CChunk::PreRender, this, std::placeholders::_1);
			data.pMeshRenderer = nullptr;
			m_meshContainer.SetData(data);
			m_meshContainer.AddMaterial(m_pMaterial);
			//m_meshContainer.AddMaterial(m_pMaterialWire);
			m_meshContainer.Initialize();
		}

		// Calculte the maximum lod level.
		auto LeastSignificantSetBit = [](u32 val){
			u32 bit = 1;

			while(val && (val & 0x1) == 0)
			{
				val >>= 1;
				++bit;
			}

			return bit;
		};

		m_lodLevelMax = 0;//std::min(std::min(LeastSignificantSetBit(m_data.width), LeastSignificantSetBit(m_data.height)), LeastSignificantSetBit(m_data.length));
	}
	
	void CChunk::Initialize()
	{
#if _DEBUG
		m_bInitialized = true;
#endif
		
		{ // Build initial chunk ids.
			AllocateBlockList();

			u32 index = 0;
			for(u32 i = 0; i < m_data.width; ++i)
			{
				for(u32 k = 0; k < m_data.length; ++k)
				{
					for(u32 j = 0; j < m_data.height; ++j)
					{
						m_pBlockList[index++] = { 0,
							(m_data.offset.y + (int)j) < -60,
							0, 0 };
					}
				}
			}
		}

		Setup();
		RebuildMesh();
	}
	
	void CChunk::LateUpdate()
	{
		m_bUpdateQueued = false;

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
					if(m_blockUpdateMap.empty())
					{
						RebuildMesh();
					}
				}
			}

			if(!m_blockUpdateMap.empty())
			{
				u8 adjUpdates = 0;

				{ // Update blocks and the indices provided.
					{
						std::lock_guard<std::shared_mutex> lk(m_mutex);
						
						if(m_pBlockList == nullptr)
						{
							AllocateBlockList();
						}

						for(const auto& elem : m_blockUpdateMap)
						{
							m_pBlockList[elem.first].bFilled = elem.second <= 0xFF;
							if(m_pBlockList[elem.first].bFilled)
							{
								m_pBlockList[elem.first].id = static_cast<u8>(elem.second);
							}
						}
					}

					for(const auto& elem : m_blockUpdateMap)
					{
						u32 i, j, k;
						internalGetCoordsFromIndex(elem.first, i, j, k);
						if(i == 0 && m_pChunkAdj[SIDE_LEFT] && m_pChunkAdj[SIDE_LEFT]->GetBlock(m_pChunkAdj[SIDE_LEFT]->internalGetIndexRight(j, k)).bFilled) adjUpdates |= SIDE_FLAG_LEFT;
						if(i == m_data.width - 1 && m_pChunkAdj[SIDE_RIGHT] && m_pChunkAdj[SIDE_RIGHT]->GetBlock(m_pChunkAdj[SIDE_RIGHT]->internalGetIndexLeft(j, k)).bFilled) adjUpdates |= SIDE_FLAG_RIGHT;
						if(j == 0 && m_pChunkAdj[SIDE_BOTTOM] && m_pChunkAdj[SIDE_BOTTOM]->GetBlock(m_pChunkAdj[SIDE_BOTTOM]->internalGetIndexTop(i, k)).bFilled) adjUpdates |= SIDE_FLAG_BOTTOM;
						if(j == m_data.height - 1 && m_pChunkAdj[SIDE_TOP] && m_pChunkAdj[SIDE_TOP]->GetBlock(m_pChunkAdj[SIDE_TOP]->internalGetIndexBottom(i, k)).bFilled) adjUpdates |= SIDE_FLAG_TOP;
						if(k == 0 && m_pChunkAdj[SIDE_BACK] && m_pChunkAdj[SIDE_BACK]->GetBlock(m_pChunkAdj[SIDE_BACK]->internalGetIndexFront(i, j)).bFilled) adjUpdates |= SIDE_FLAG_BACK;
						if(k == m_data.length - 1 && m_pChunkAdj[SIDE_FRONT] && m_pChunkAdj[SIDE_FRONT]->GetBlock(m_pChunkAdj[SIDE_FRONT]->internalGetIndexBack(i, j)).bFilled) adjUpdates |= SIDE_FLAG_FRONT;
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

		if(m_bDirty)
		{ // Keep chunk in update queue while it is dirty.
			App::CSceneManager::Instance().UniverseManager().ChunkManager().QueueChunkUpdate(this);
		}
	}

	
	void CChunk::ForceRender(size_t materialIndex)
	{
		m_meshContainer.RenderWithMaterial(materialIndex);
	}

	void CChunk::Release()
	{
		m_meshContainer.Release();
		//m_meshContainerWire.Release();
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

			if(m_pBlockList == nullptr)
			{ // Build initial chunk ids.
				AllocateBlockList();
				memset(m_pBlockList, 0, sizeof(Block) * m_chunkSize);
			}

			u32 index = m_lodLevelOffset;
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
						if(!m_pBlockList[index].bFilled)
						{
							++index;
							continue;
						}

						// Left.
						if(i == 0)
						{
							if(!m_pChunkAdj[SIDE_LEFT] || !m_pChunkAdj[SIDE_LEFT]->internalGetBlock(m_pChunkAdj[SIDE_LEFT]->internalGetIndexRight(j, k)).bFilled)
							{
								AddQuad(SIDE_FLAG_LEFT);
							}
						}
						else if(!m_pBlockList[m_lodLevelOffset + internalGetIndex(i - 1, j, k)].bFilled)
						{
							AddQuad(SIDE_FLAG_LEFT);
						}

						// Right.
						if(i == m_data.width - 1)
						{
							if(!m_pChunkAdj[SIDE_RIGHT] || !m_pChunkAdj[SIDE_RIGHT]->internalGetBlock(m_pChunkAdj[SIDE_RIGHT]->internalGetIndexLeft(j, k)).bFilled)
							{
								AddQuad(SIDE_FLAG_RIGHT);
							}
						}
						else if(!m_pBlockList[m_lodLevelOffset + internalGetIndex(i + 1, j, k)].bFilled)
						{
							AddQuad(SIDE_FLAG_RIGHT);
						}

						// Bottom.
						if(j == 0)
						{
							if(!m_pChunkAdj[SIDE_BOTTOM] || !m_pChunkAdj[SIDE_BOTTOM]->internalGetBlock(m_pChunkAdj[SIDE_BOTTOM]->internalGetIndexTop(i, k)).bFilled)
							{
								AddQuad(SIDE_FLAG_BOTTOM);
							}
						}
						else if(!m_pBlockList[m_lodLevelOffset + internalGetIndex(i, j - 1, k)].bFilled)
						{
							AddQuad(SIDE_FLAG_BOTTOM);
						}

						// Top.
						if(j == m_data.height - 1)
						{
							if(!m_pChunkAdj[SIDE_TOP] || !m_pChunkAdj[SIDE_TOP]->internalGetBlock(m_pChunkAdj[SIDE_TOP]->internalGetIndexBottom(i, k)).bFilled)
							{
								AddQuad(SIDE_FLAG_TOP);
							}
						}
						else if(!m_pBlockList[m_lodLevelOffset + internalGetIndex(i, j + 1, k)].bFilled)
						{
							AddQuad(SIDE_FLAG_TOP);
						}

						// Back.
						if(k == 0)
						{
							if(!m_pChunkAdj[SIDE_BACK] || !m_pChunkAdj[SIDE_BACK]->internalGetBlock(m_pChunkAdj[SIDE_BACK]->internalGetIndexFront(i, j)).bFilled)
							{
								AddQuad(SIDE_FLAG_BACK);
							}
						}
						else if(!m_pBlockList[m_lodLevelOffset + internalGetIndex(i, j, k - 1)].bFilled)
						{
							AddQuad(SIDE_FLAG_BACK);
						}

						// Front.
						if(k == m_data.length - 1)
						{
							if(!m_pChunkAdj[SIDE_FRONT] || !m_pChunkAdj[SIDE_FRONT]->internalGetBlock(m_pChunkAdj[SIDE_FRONT]->internalGetIndexBack(i, j)).bFilled)
							{
								AddQuad(SIDE_FLAG_FRONT);
							}
						}
						else if(!m_pBlockList[m_lodLevelOffset + internalGetIndex(i, j, k + 1)].bFilled)
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

				u32 index = m_lodLevelOffset;
				for(u32 i = 0; i < m_data.width; ++i)
				{
					for(u32 k = 0; k < m_data.length; ++k)
					{
						for(u32 j = 0; j < m_data.height; ++j)
						{
							const Block block = m_pBlockList[index++];
							if(!block.bFilled) continue;

							Math::Vector3 center(i + 0.5f, j + 0.5f, k + 0.5f);

							
							// Left.
							if(i == 0)
							{
								if(!m_pChunkAdj[SIDE_LEFT] || !m_pChunkAdj[SIDE_LEFT]->internalGetBlock(m_pChunkAdj[SIDE_LEFT]->internalGetIndexRight(j, k)).bFilled)
								{
									GenerateQuad(block.id, offset, center, Math::VEC3_BACKWARD, Math::VEC3_UP, Math::VEC3_LEFT);
								}
							}
							else if(!m_pBlockList[m_lodLevelOffset + internalGetIndex(i - 1, j, k)].bFilled)
							{
								GenerateQuad(block.id, offset, center, Math::VEC3_BACKWARD, Math::VEC3_UP, Math::VEC3_LEFT);
							}

							// Right.
							if(i == m_data.width - 1)
							{
								if(!m_pChunkAdj[SIDE_RIGHT] || !m_pChunkAdj[SIDE_RIGHT]->internalGetBlock(m_pChunkAdj[SIDE_RIGHT]->internalGetIndexLeft(j, k)).bFilled)
								{
									GenerateQuad(block.id, offset, center, Math::VEC3_FORWARD, Math::VEC3_UP, Math::VEC3_RIGHT);
								}
							}
							else if(!m_pBlockList[m_lodLevelOffset + internalGetIndex(i + 1, j, k)].bFilled)
							{
								GenerateQuad(block.id, offset, center, Math::VEC3_FORWARD, Math::VEC3_UP, Math::VEC3_RIGHT);
							}

							// Bottom.
							if(j == 0)
							{
								if(!m_pChunkAdj[SIDE_BOTTOM] || !m_pChunkAdj[SIDE_BOTTOM]->internalGetBlock(m_pChunkAdj[SIDE_BOTTOM]->internalGetIndexTop(i, k)).bFilled)
								{
									GenerateQuad(block.id, offset, center, Math::VEC3_RIGHT, Math::VEC3_BACKWARD, Math::VEC3_DOWN);
								}
							}
							else if(!m_pBlockList[m_lodLevelOffset + internalGetIndex(i, j - 1, k)].bFilled)
							{
								GenerateQuad(block.id, offset, center, Math::VEC3_RIGHT, Math::VEC3_BACKWARD, Math::VEC3_DOWN);
							}

							// Top.
							if(j == m_data.height - 1)
							{
								if(!m_pChunkAdj[SIDE_TOP] || !m_pChunkAdj[SIDE_TOP]->internalGetBlock(m_pChunkAdj[SIDE_TOP]->internalGetIndexBottom(i, k)).bFilled)
								{
									GenerateQuad(block.id, offset, center, Math::VEC3_RIGHT, Math::VEC3_FORWARD, Math::VEC3_UP);
								}
							}
							else if(!m_pBlockList[m_lodLevelOffset + internalGetIndex(i, j + 1, k)].bFilled)
							{
								GenerateQuad(block.id, offset, center, Math::VEC3_RIGHT, Math::VEC3_FORWARD, Math::VEC3_UP);
							}

							// Back.
							if(k == 0)
							{
								if(!m_pChunkAdj[SIDE_BACK] || !m_pChunkAdj[SIDE_BACK]->internalGetBlock(m_pChunkAdj[SIDE_BACK]->internalGetIndexFront(i, j)).bFilled)
								{
									GenerateQuad(block.id, offset, center, Math::VEC3_RIGHT, Math::VEC3_UP, Math::VEC3_BACKWARD);
								}
							}
							else if(!m_pBlockList[m_lodLevelOffset + internalGetIndex(i, j, k - 1)].bFilled)
							{
								GenerateQuad(block.id, offset, center, Math::VEC3_RIGHT, Math::VEC3_UP, Math::VEC3_BACKWARD);
							}

							// Front.
							if(k == m_data.length - 1)
							{
								if(!m_pChunkAdj[SIDE_FRONT] || !m_pChunkAdj[SIDE_FRONT]->internalGetBlock(m_pChunkAdj[SIDE_FRONT]->internalGetIndexBack(i, j)).bFilled)
								{
									GenerateQuad(block.id, offset, center, Math::VEC3_LEFT, Math::VEC3_UP, Math::VEC3_FORWARD);
								}
							}
							else if(!m_pBlockList[m_lodLevelOffset + internalGetIndex(i, j, k + 1)].bFilled)
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
			data.pMeshData = &m_meshData[meshIndex];
			m_pMeshRendererList[meshIndex]->SetData(data);
			m_pMeshRendererList[meshIndex]->AddMaterial(m_pMaterial);
			//m_pMeshRendererList[meshIndex]->AddMaterial(m_pMaterialWire);

			m_pMeshRendererList[meshIndex]->Initialize();
		}

		/*{ // Create the mesh wire renderer.
			m_pMeshRendererListWire[meshIndex] = CFactory::Instance().CreateMeshRenderer(m_pObject);

			Graphics::CMeshRenderer_::Data data { };
			data.bSkipRegistration = true;
			data.pMeshData = &m_meshData[meshIndex];
			m_pMeshRendererListWire[meshIndex]->SetData(data);
			m_pMeshRendererListWire[meshIndex]->AddMaterial(m_pMaterialWire);

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

			const u32 blockIndex = m_lodLevelOffset + index;

			// Add edges of quad to disjointed set.
			if((m_pBlockList[blockIndex].sideFlag & flags.v[1]) || indices[iAxis] == 0 || m_pBlockList[blockIndex - iStep].id != m_pBlockList[blockIndex].id || !(m_pBlockList[blockIndex - iStep].sideFlag & flags.v[0])) 
			{
				AddEdge(internalGetVertexIndex(indices[0] + (u32)edges.e[0].e0.x, indices[1] + (u32)edges.e[0].e0.y, indices[2] + (u32)edges.e[0].e0.z),
					internalGetVertexIndex(indices[0] + (u32)edges.e[0].e1.x, indices[1] + (u32)edges.e[0].e1.y, indices[2] + (u32)edges.e[0].e1.z));
			}

			if((m_pBlockList[blockIndex].sideFlag & flags.v[2]) || indices[iAxis] == chunkSize[iAxis] - 1 || m_pBlockList[blockIndex + iStep].id != m_pBlockList[blockIndex].id || !(m_pBlockList[blockIndex + iStep].sideFlag & flags.v[0]))
			{
				AddEdge(internalGetVertexIndex(indices[0] + (u32)edges.e[1].e0.x, indices[1] + (u32)edges.e[1].e0.y, indices[2] + (u32)edges.e[1].e0.z),
					internalGetVertexIndex(indices[0] + (u32)edges.e[1].e1.x, indices[1] + (u32)edges.e[1].e1.y, indices[2] + (u32)edges.e[1].e1.z));
			}

			if((m_pBlockList[blockIndex].sideFlag & flags.v[3]) || indices[kAxis] == 0 || m_pBlockList[blockIndex - kStep].id != m_pBlockList[blockIndex].id || !(m_pBlockList[blockIndex - kStep].sideFlag & flags.v[0]))
			{
				AddEdge(internalGetVertexIndex(indices[0] + (u32)edges.e[2].e0.x, indices[1] + (u32)edges.e[2].e0.y, indices[2] + (u32)edges.e[2].e0.z),
					internalGetVertexIndex(indices[0] + (u32)edges.e[2].e1.x, indices[1] + (u32)edges.e[2].e1.y, indices[2] + (u32)edges.e[2].e1.z));
			}

			if((m_pBlockList[blockIndex].sideFlag & flags.v[4]) || indices[kAxis] == chunkSize[kAxis] - 1 || m_pBlockList[blockIndex + kStep].id != m_pBlockList[blockIndex].id || !(m_pBlockList[blockIndex + kStep].sideFlag & flags.v[0]))
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

			int priorI = 0;
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

					priorI = i = prev[i];
				}
				else
				{
					i = next[i];
					assert(i != priorI);

#if PRODUCTION_BUILD
					if(i == priorI)
					{ // Break infinite loops in production build.
						//  This only happen if there is a bug with the optimizer, but it's better to have a buggy mesh than one's generation looping to infinity.
						break;
					}
#endif
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
				// Find point in remaining point lists that is closest to -tangent direction.
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

						if(fabsf(Math::Vector3::Dot(diff / sqrtf(dist), (pointLists[mergedListIndex][prevV] - pointLists[mergedListIndex][v]).Normalized())) > 0.99999f)
						{ // Parallel lines cannot be used as they create degenerate triangle.
							continue;
						}
						
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
							{ // CCW, make sure diff is pointing away from the two edges of the infinite triangle.
								if(Math::Vector3::Dot(n0, diff) > -1e-5f) continue;
								if(Math::Vector3::Dot(n1, diff) > -1e-5f) continue;
							}
							else if(Math::Vector3::Dot(e0, e1) > -1e-5f)
							{ // CW, beyond ~270 degrees. This extension is important for the case of holes in the geometry.
								if(Math::Vector3::Dot(n0, diff) > -1e-5f && Math::Vector3::Dot(n1, diff) > -1e-5f) continue;
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
					if(m_pBlockList[m_lodLevelOffset + index].bFilled)
					{
						if(m_pBlockList[m_lodLevelOffset + index].sideFlag & SIDE_FLAG_LEFT)
						{
							blockMap[0][m_pBlockList[m_lodLevelOffset + index].id].insert(index);
						}
						if(m_pBlockList[m_lodLevelOffset + index].sideFlag & SIDE_FLAG_RIGHT)
						{
							blockMap[1][m_pBlockList[m_lodLevelOffset + index].id].insert(index);
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
					if(m_pBlockList[m_lodLevelOffset + index].bFilled)
					{
						if(m_pBlockList[m_lodLevelOffset + index].sideFlag & SIDE_FLAG_BOTTOM)
						{
							blockMap[0][m_pBlockList[m_lodLevelOffset + index].id].insert(index);
						}
						if(m_pBlockList[m_lodLevelOffset + index].sideFlag & SIDE_FLAG_TOP)
						{
							blockMap[1][m_pBlockList[m_lodLevelOffset + index].id].insert(index);
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
					if(m_pBlockList[m_lodLevelOffset + index].bFilled)
					{
						if(m_pBlockList[m_lodLevelOffset + index].sideFlag & SIDE_FLAG_BACK)
						{
							blockMap[0][m_pBlockList[m_lodLevelOffset + index].id].insert(index);
						}
						if(m_pBlockList[m_lodLevelOffset + index].sideFlag & SIDE_FLAG_FRONT)
						{
							blockMap[1][m_pBlockList[m_lodLevelOffset + index].id].insert(index);
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
		if(m_blockUpdateMap.size())
		{
			return;
		}

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

		// Push chunk to update queue until finished generating.
		PushToUpdateQueue();
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
			lastBlock.i = 0;
			
			{
				std::shared_lock<std::shared_mutex> lk(m_mutex);
				if(m_pBlockList)
				{
					// Get last block from block list.
					lastBlock = m_pBlockList[index];
				}
			}

			lastId = lastBlock.bFilled ? lastBlock.id : 256;
			m_blockUpdateMap.insert({ index, id });
			PushToUpdateQueue();
		}

		return lastId;
	}

	u16 CChunk::PaintBlock(u32 index, BlockId id)
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
			lastBlock.i = 0;
			
			{
				std::shared_lock<std::shared_mutex> lk(m_mutex);
				if(m_pBlockList)
				{
					// Get last block from block list.
					lastBlock = m_pBlockList[index];
				}
			}

			lastId = lastBlock.bFilled ? lastBlock.id : 256;
			if(lastBlock.bFilled)
			{
				m_blockUpdateMap.insert({ index, id });
				PushToUpdateQueue();
			}
		}

		return lastId;
	}
	
	void CChunk::SetBlock(u32 index, u16 id)
	{
		m_blockUpdateMap[index] = id;
		PushToUpdateQueue();
	}
	
	void CChunk::Set(const Block* pBlocks)
	{
		{
			std::lock_guard<std::shared_mutex> lk(m_mutex);
			if(m_pBlockList == nullptr) AllocateBlockList();
			memcpy(m_pBlockList, pBlocks, sizeof(Block) * m_chunkSize);
		}

		RebuildMesh();
	}
	
	void CChunk::Set(std::function<void(Block*, size_t)> func)
	{
		{
			std::lock_guard<std::shared_mutex> lk(m_mutex);
			if(m_pBlockList == nullptr) AllocateBlockList();
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
							(m_data.offset.y + (int)j) < -60,
							0, 0 };
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
						m_pBlockList[index++] = { 0, false, 0, 0 };
					}
				}
			}
		}

		RebuildMesh();
	}

	void CChunk::PushToUpdateQueue()
	{
		if(m_bUpdateQueued) return;
		App::CSceneManager::Instance().UniverseManager().ChunkManager().QueueChunkUpdate(this);
		m_bUpdateQueued = true;
	}

	//-----------------------------------------------------------------------------------------------
	// LOD Methods.
	//-----------------------------------------------------------------------------------------------

	void CChunk::SetLODLevel(u8 lodLevel)
	{
		if(m_lodLevel == lodLevel || m_lodLevelMax < lodLevel) return;
		m_lodLevel = lodLevel;
		m_lodLevelOffset = m_lodLevel * m_chunkSize;

		if(m_lodLevel > 0)
		{
			const u32 lodLevelOffsetLast = (m_lodLevel - 1) * m_chunkSize;
			const u32 stepSize = 1 << m_lodLevel;

			const double targetCoverage = 0.3333;//Math::g_1Over3;
			const double targetPlanarCoverage = 0.0;//Math::g_1Over3;
			const double targetExtentCoveragePlanar = 0.125;//Math::g_1Over3;
			const double targetExtentCoverageLinear = 0.275;//Math::g_1Over3;
			const double blockStength = 1.0 / pow(stepSize, 3U);
			const double blockStength2D = 1.0 / pow(stepSize, 2U);
			const double blockStength1D = 1.0 / pow(stepSize, 1U);
		
			double coverage;
			std::unordered_map<BlockId, u32> blockMap;
			std::pair<BlockId, u32> maxBlock;

			struct VectorUInt2 { union { struct { u32 x, y; }; u32 v[2]; }; u32 operator [] (size_t index) const { return v[index]; } };
			struct VectorUInt3 { union { struct { u32 x, y, z; }; u32 v[3]; }; u32 operator [] (size_t index) const { return v[index]; } };
			VectorUInt3 mnExtents;
			VectorUInt3 mxExtents;

			std::lock_guard<std::shared_mutex> lk(m_mutex);

			memset(m_pBlockList + m_lodLevelOffset, 0, sizeof(Block) * m_chunkSize);

			for(u32 i = 0; i < m_data.width; i += stepSize)
			{
				for(u32 k = 0; k < m_data.length; k += stepSize)
				{
					for(u32 j = 0; j < m_data.height; j += stepSize)
					{
						coverage = 0.0;
						blockMap.clear();
						maxBlock = { 0, 0 };

						mnExtents = { UINT_MAX, UINT_MAX, UINT_MAX };
						mxExtents = { 0, 0, 0 };

						auto PlanarCoverage = [&](u32 u, u32 v, u32 w, u32 wOffset){
							u32 steps[3];
							const u32 x = wOffset;
							double planarCoverage = 0.0;

							for(u32 z = 0; z < stepSize; ++z)
							{
								for(u32 y = 0; y < stepSize; ++y)
								{
									steps[u] = y; steps[v] = z; steps[w] = x;

									Block block = m_pBlockList[lodLevelOffsetLast + internalGetIndex(i + steps[0], j + steps[1], k + steps[2])];
									if(block.bFilled)
									{
										planarCoverage += blockStength2D;
									}
								}
							}

							return planarCoverage >= targetExtentCoveragePlanar;
						};

						auto LinearCoverage = [&](u32 u, u32 v, u32 w, u32 vOffset, u32 wOffset){
							u32 steps[3];
							const u32 x = wOffset;
							const u32 z = vOffset;
							double linearCoverage = 0.0;

							for(u32 y = 0; y < stepSize; ++y)
							{
								steps[u] = y; steps[v] = z; steps[w] = x;

								Block block = m_pBlockList[lodLevelOffsetLast + internalGetIndex(i + steps[0], j + steps[1], k + steps[2])];
								if(block.bFilled)
								{
									linearCoverage += blockStength1D;
								}
							}

							return linearCoverage >= targetExtentCoverageLinear;
						};

						// Check for LOD block for coverage.
						for(u32 x = 0; x < stepSize; ++x)
						{
							for(u32 z = 0; z < stepSize; ++z)
							{
								for(u32 y = 0; y < stepSize; ++y)
								{
									Block block = m_pBlockList[lodLevelOffsetLast + internalGetIndex(i + x, j + y, k + z)];
									if(block.bFilled)
									{
										if(x < mnExtents.x && PlanarCoverage(1, 2, 0, x)) mnExtents.x = x;
										if(x >= mxExtents.x && PlanarCoverage(1, 2, 0, x)) mxExtents.x = x + 1;
										if(y < mnExtents.y && PlanarCoverage(2, 0, 1, y)) mnExtents.y = y;
										if(y >= mxExtents.y && PlanarCoverage(2, 0, 1, y)) mxExtents.y = y + 1;
										if(z < mnExtents.z && PlanarCoverage(1, 0, 2, z)) mnExtents.z = z;
										if(z >= mxExtents.z && PlanarCoverage(1, 0, 2, z)) mxExtents.z = z + 1;

										coverage += blockStength;

										while(block.sideFlag)
										{
											if((block.sideFlag & 0x1) && maxBlock.second < ++blockMap[block.id])
											{
												maxBlock.first = block.id;
											}

											block.sideFlag >>= 1;
										}
									}
								}
							}
						}

						if(coverage >= targetCoverage)
						{ 
							for(u32 x = mnExtents.x; x < mxExtents.x; ++x)
							{
								for(u32 z = mnExtents.z; z < mxExtents.z; ++z)
								{
									for(u32 y = mnExtents.y; y < mxExtents.y; ++y)
									{
										m_pBlockList[m_lodLevelOffset + internalGetIndex(i + x, j + y, k + z)] = Block(maxBlock.first, true);
									}
								}
							}
						}
						else
						{ // If LOD block < target coverage, check for LOD planes for coverage.
							VectorUInt2 mnPlanar;
							VectorUInt2 mxPlanar;

							auto PlanarLOD = [&](u32 u, u32 v, u32 w){
								u32 steps[3];

								for(u32 x = 0; x < stepSize; ++x)
								{
									coverage = 0.0;
									blockMap.clear();
									maxBlock = { 0, 0 };
									mnPlanar = { UINT_MAX, UINT_MAX };
									mxPlanar = { 0, 0 };

									for(u32 z = 0; z < stepSize; ++z)
									{
										for(u32 y = 0; y < stepSize; ++y)
										{
											steps[u] = y; steps[v] = z; steps[w] = x;

											Block block = m_pBlockList[lodLevelOffsetLast + internalGetIndex(i + steps[0], j + steps[1], k + steps[2])];
											if(block.bFilled)
											{
												if(y < mnPlanar.x && LinearCoverage(v, u, w, y, x)) mnPlanar.x = y;
												if(y >= mxPlanar.x && LinearCoverage(v, u, w, y, x)) mxPlanar.x = y + 1;
												if(z < mnPlanar.y && LinearCoverage(u, v, w, z, x)) mnPlanar.y = z;
												if(z >= mxPlanar.y && LinearCoverage(u, v, w, z, x)) mxPlanar.y = z + 1;

												coverage += blockStength2D;

												while(block.sideFlag)
												{
													if((block.sideFlag & 0x1) && maxBlock.second < ++blockMap[block.id])
													{
														maxBlock.first = block.id;
													}

													block.sideFlag >>= 1;
												}
											}
										}
									}

									if(coverage >= targetPlanarCoverage)
									{
										for(u32 z = mnPlanar.y; z < mxPlanar.y; ++z)
										{
											for(u32 y = mnPlanar.x; y < mxPlanar.x; ++y)
											{
												steps[u] = y; steps[v] = z; steps[w] = x;

												m_pBlockList[m_lodLevelOffset + internalGetIndex(i + steps[0], j + steps[1], k + steps[2])] = Block(maxBlock.first, true);
											}
										}
									}
								}
							};

							// X-Axis.
							PlanarLOD(1, 2, 0);
							
							// Y-Axis.
							PlanarLOD(2, 0, 1);

							// Z-Axis.
							PlanarLOD(1, 0, 2);
						}
					}
				}
			}
		}
	}
};
