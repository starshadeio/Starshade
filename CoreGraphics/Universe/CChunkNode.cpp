//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunkNode.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CChunkNode.h"
#include "CChunk.h"
#include "CChunkManager.h"
#include "../Application/CSceneManager.h"
#include <Application/CCommandManager.h>
#include <Math/CMathVectorInt3.h>

namespace Universe
{
	CChunkNode::CChunkNode(const wchar_t* pName, u32 viewHash) :
		CVObject(pName, viewHash),
		m_minExtents(-16),
		m_maxExtents(16),
		m_halfChunkSize(0.0f),
		m_transform(this),
		m_volume(this),
		m_callback(this)
	{
	}

	CChunkNode::~CChunkNode()
	{
	}
	
	void CChunkNode::Initialize()
	{
		m_halfChunkSize = Math::Vector3(
			static_cast<float>(m_data.chunkWidth) * 0.5f,
			static_cast<float>(m_data.chunkHeight) * 0.5f,
			static_cast<float>(m_data.chunkLength) * 0.5f
		);

		m_halfChunkSpan = Math::Vector3(
			static_cast<float>(m_data.chunksX) * m_halfChunkSize.x,
			static_cast<float>(m_data.chunksY) * m_halfChunkSize.y,
			static_cast<float>(m_data.chunksZ) * m_halfChunkSize.z
		);

		m_minExtents = m_halfChunkSpan * -m_data.blockSize;
		m_maxExtents = m_halfChunkSpan *  m_data.blockSize;

		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				for(int k = 0; k < 4; ++k)
				{
					CChunk::Data data { };
					data.offset = Math::VectorInt3(
						(i - 2) * m_data.chunkWidth,
						(j - 2) * m_data.chunkHeight,
						(k - 2) * m_data.chunkLength
					);

					data.width = m_data.chunkWidth;
					data.height = m_data.chunkHeight;
					data.length = m_data.chunkLength;
					data.blockSize = m_data.blockSize;

					data.matHash = Math::FNV1a_64("MATERIAL_VOXEL");
					data.matWireHash = Math::FNV1a_64("MATERIAL_VOXELWIRE");

					m_pChunkList[i][j][k] = App::CSceneManager::Instance().UniverseManager().ChunkManager().RegisterChunk(this, Math::VectorInt3(i - 2, j - 2, k - 2), data);
				}
			}
		}

		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				for(int k = 0; k < 4; ++k)
				{
					if(i > 0) m_pChunkList[i][j][k]->SetAdjacentChunk(SIDE_LEFT, m_pChunkList[i - 1][j][k]);
					if(i < 3) m_pChunkList[i][j][k]->SetAdjacentChunk(SIDE_RIGHT, m_pChunkList[i + 1][j][k]);
					if(j > 0) m_pChunkList[i][j][k]->SetAdjacentChunk(SIDE_BOTTOM, m_pChunkList[i][j - 1][k]);
					if(j < 3) m_pChunkList[i][j][k]->SetAdjacentChunk(SIDE_TOP, m_pChunkList[i][j + 1][k]);
					if(k > 0) m_pChunkList[i][j][k]->SetAdjacentChunk(SIDE_BACK, m_pChunkList[i][j][k - 1]);
					if(k < 3) m_pChunkList[i][j][k]->SetAdjacentChunk(SIDE_FRONT, m_pChunkList[i][j][k + 1]);
				}
			}
		}

		{ // Create collider.
			Physics::CVolumeChunk::Data data { };
			data.pChunkNode = this;
			data.bAllowRays = true;
			data.colliderType = Physics::ColliderType::Collider;
			m_volume.SetData(data);
			m_volume.Register(m_transform.GetWorldMatrix());
		}

		{ // Setup the logic callback(s).
			Logic::CCallback::Data data { };
			data.callbackMap.insert({ Logic::CALLBACK_INTERACT, std::bind(&CChunkNode::InteractCallback, this, std::placeholders::_1) });
			m_callback.SetData(data);
		}
	}

	void CChunkNode::Release()
	{
		m_volume.Deregister();
	}
	
	//-----------------------------------------------------------------------------------------------
	// Action methods.
	//-----------------------------------------------------------------------------------------------

	void CChunkNode::Save(const std::wstring& path) const
	{
		std::ofstream file(path + L"\\chunk.dat", std::ios::binary);
		assert(file.is_open());
		
		if(file.is_open())
		{
			Save(file);
			file.close();
		}
	}

	void CChunkNode::Save(std::ofstream& file) const
	{
		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				for(int k = 0; k < 4; ++k)
				{
					m_pChunkList[i][j][k]->Read([&file](const Block* pBlockList, size_t blockCount){
						file.write(reinterpret_cast<const char*>(pBlockList), sizeof(Block) * blockCount);
					});
				}
			}
		}
	}

	void CChunkNode::Load(const std::wstring& path)
	{
		std::ifstream file(path + L"\\chunk.dat", std::ios::binary);
		assert(file.is_open());

		if(file.is_open())
		{
			Load(file);
			file.close();
		}
	}

	void CChunkNode::Load(std::ifstream& file)
	{
		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				for(int k = 0; k < 4; ++k)
				{
					m_pChunkList[i][j][k]->Set([&file](Block* pBlockList, size_t blockCount){
						file.read(reinterpret_cast<char*>(pBlockList), sizeof(Block) * blockCount);
					
						/* Used for loading a project from prior versions of the editor.
						file.read(reinterpret_cast<char*>(pBlockList), 2 * blockCount);

						for(int i = static_cast<int>(blockCount * 2) - 2, j = static_cast<int>(blockCount) - 1; i >= 0; i -= 2, --j)
						{
							pBlockList[j] = *reinterpret_cast<Block*>(reinterpret_cast<u8*>(pBlockList) + i);
							pBlockList[j].padding = 0;
						}*/
					});
				}
			}
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Action methods.
	//-----------------------------------------------------------------------------------------------

	void CChunkNode::Reset()
	{
		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				for(int k = 0; k < 4; ++k)
				{
					m_pChunkList[i][j][k]->Reset();
				}
			}
		}
	}

	void CChunkNode::Clear()
	{
		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				for(int k = 0; k < 4; ++k)
				{
					m_pChunkList[i][j][k]->Clear();
				}
			}
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Callback methods.
	//-----------------------------------------------------------------------------------------------

	bool CChunkNode::InteractCallback(void* pVal)
	{
		Physics::RaycastInfo info = *(Physics::RaycastInfo*)pVal;
		if(info.alt.U8.lo[0] == 1)
		{
			union FloatU32
			{
				float f;
				u32 i;
				FloatU32(u32 i) : i(i) { } 
			};

			const Math::Vector3 coord(
				FloatU32(info.index.U32.lo[0]).f,
				FloatU32(info.index.U32.lo[1]).f,
				FloatU32(info.index.U32.hi[0]).f
			);
			
			int brushIndices = (1 << static_cast<int>(info.alt.U8.lo[2]));
			int brushHalf = brushIndices >> 1;

			CChunkManager::ChunkBlockUpdateData data { };
			data.pChunkNode = this;
			data.coordCount = brushIndices * brushIndices * brushIndices;

			bool bEmpty = true;
			bool bFull = true;
			short blockId = -1;

			std::pair<Math::VectorInt3, u32> indices;
			u32 coordIndex = 0;
			for(int i = 0; i < brushIndices; ++i)
			{
				for(int j = 0; j < brushIndices; ++j)
				{
					for(int k = 0; k < brushIndices; ++k, ++coordIndex)
					{
						const Math::Vector3 offset(
							static_cast<float>(i - brushHalf),
							static_cast<float>(j - brushHalf),
							static_cast<float>(k - brushHalf)
						);

						Block lastBlock = GetBlock(coord + offset, &indices);
									
						if(indices.second == std::numeric_limits<u32>::max())
						{
							continue;
						}

						bEmpty &= lastBlock.bEmpty;
						bFull &= !lastBlock.bEmpty;

						if(!lastBlock.bEmpty)
						{
							if(blockId < 0)
							{
								blockId = lastBlock.id;
							}
							else if(blockId != lastBlock.id)
							{
								blockId = 256;
							}
						}
						
						data.coordList[coordIndex].block.id = info.alt.U8.hi[0];
						data.coordList[coordIndex].block.bEmpty = info.alt.U8.lo[1] == 0;
						data.coordList[coordIndex].chunkCoord = indices.first;
						data.coordList[coordIndex].blockIndex = indices.second;
					}
				}
			}

			bool bValid = false;
			switch(info.alt.U8.lo[1])
			{
				case 0: // Erase
					if(!bEmpty)
					{
						bValid = true;
					} break;
				case 1: // Fill
					if(!bFull)
					{
						bValid = true;
					} break;
				case 2: // Paint
					if(blockId >= 0 && blockId != info.alt.U8.hi[0])
					{
						bValid = true;
					} break;
				default:
					break;
			}

			if(bValid)
			{
				App::CCommandManager::Instance().Execute(CChunkManager::CMD_KEY_BLOCK_EDIT, &data, data.GetSize(), alignof(CChunkManager::ChunkBlockUpdateData));
				return true;
			}
		}

		return false;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	std::pair<Math::VectorInt3, u32> CChunkNode::internalGetIndex(const Math::Vector3& coord, CChunk** ppChunk) const
	{
		std::pair<Math::VectorInt3, u32> res;

		const Math::Vector3 pt = coord;// + m_halfChunkSize;
		const Math::VectorInt3 chunkCoord(
			static_cast<int>(floorf(pt.x / m_data.chunkWidth)),
			static_cast<int>(floorf(pt.y / m_data.chunkHeight)),
			static_cast<int>(floorf(pt.z / m_data.chunkLength))
		);

		const Math::VectorInt3 blockCoord(
			static_cast<int>(floorf(pt.x - chunkCoord.x * static_cast<int>(m_data.chunkWidth))),
			static_cast<int>(floorf(pt.y - chunkCoord.y * static_cast<int>(m_data.chunkHeight))),
			static_cast<int>(floorf(pt.z - chunkCoord.z * static_cast<int>(m_data.chunkLength)))
		);

		CChunk* pChunk = App::CSceneManager::Instance().UniverseManager().ChunkManager().GetChunk(this, chunkCoord);
		if(ppChunk) *ppChunk = pChunk;
		if(pChunk == nullptr) { return { Math::VectorInt3(std::numeric_limits<int>::max()), std::numeric_limits<u32>::max() }; }

		res.first = chunkCoord;
		res.second = pChunk->GetIndex(static_cast<u32>(blockCoord.x), static_cast<u32>(blockCoord.y), static_cast<u32>(blockCoord.z));

		return res;
	}

	std::pair<Math::VectorInt3, u32> CChunkNode::GetIndex(const Math::Vector3& coord) const
	{
		return internalGetIndex(coord, nullptr);
	}

	Block CChunkNode::GetBlock(const Math::Vector3& coord, std::pair<Math::VectorInt3, u32>* pIndices) const
	{
		CChunk* pChunk = nullptr;
		std::pair<Math::VectorInt3, u32> indices = internalGetIndex(coord, &pChunk);
		if(pIndices) *pIndices = indices;
		return pChunk ? pChunk->GetBlock(indices.second) : Block();
	}

	Math::Vector3 CChunkNode::GetPosition(const std::pair<Math::VectorInt3, u32>& indices) const
	{
		CChunk* pChunk = App::CSceneManager::Instance().UniverseManager().ChunkManager().GetChunk(this, indices.first);
		u32 i, j, k;
		pChunk->GetCoordsFromIndex(indices.second, i, j, k);

		return (Math::Vector3(
			static_cast<float>(indices.first.x) * static_cast<int>(m_data.chunkWidth) + i,
			static_cast<float>(indices.first.y) * static_cast<int>(m_data.chunkHeight) + j,
			static_cast<float>(indices.first.z) * static_cast<int>(m_data.chunkLength) + k
		) + 0.5f/* - m_halfChunkSize + 0.5f*/) * m_data.blockSize;
	}
};
