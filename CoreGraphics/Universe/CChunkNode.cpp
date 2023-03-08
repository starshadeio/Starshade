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
#include "CChunkGen.h"
#include "CChunk.h"
#include "CChunkManager.h"
#include "../Application/CSceneManager.h"
#include <Application/CCommandManager.h>
#include <Math/CMathVectorInt3.h>

namespace Universe
{
	CChunkNode::CChunkNode(const wchar_t* pName, u32 viewHash) :
		CVObject(pName, viewHash),
		m_bWorkspaceActive(true),
		m_minExtentsPhysics(-16),
		m_maxExtentsPhysics(16),
		m_minExtentsLocal(-32),
		m_maxExtentsLocal(32),
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

	void CChunkNode::LateUpdate()
	{
		Math::Vector3 cameraOffset = *(Math::Vector3*)App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition().ToFloat();

		cameraOffset.x = roundf(cameraOffset.x / m_data.blockSize) * m_data.blockSize;
		cameraOffset.y = roundf(cameraOffset.y / m_data.blockSize) * m_data.blockSize;
		cameraOffset.z = roundf(cameraOffset.z / m_data.blockSize) * m_data.blockSize;

		if(cameraOffset != m_lastCameraOffset)
		{
			m_lastCameraOffset = cameraOffset;

			std::lock_guard<std::shared_mutex> lk(m_extentMutex);
			m_minExtentsPhysics = cameraOffset + -m_data.physicsExtents;
			m_maxExtentsPhysics = cameraOffset +  m_data.physicsExtents;
			m_minExtentsLocal = cameraOffset + -m_data.localExtents;
			m_maxExtentsLocal = cameraOffset +  m_data.localExtents;
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
		/*for(int i = 0; i < 4; ++i)
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
		}*/

		auto chunkMap = App::CSceneManager::Instance().UniverseManager().ChunkManager().GetChunkMap(this);
		u32 sz = static_cast<u32>(chunkMap.size());
		file.write(reinterpret_cast<char*>(&sz), sizeof(sz));

		for(auto chunk : chunkMap)
		{
			Math::VectorInt3 coord = chunk.second->GetChunkCoord();
			file.write(reinterpret_cast<char*>(&coord), sizeof(coord));
			chunk.second->Read([&file](const Block* pBlockList, size_t blockCount){
				file.write(reinterpret_cast<const char*>(pBlockList), sizeof(Block) * blockCount);
			});
		}
	}

	void CChunkNode::Load(const std::wstring& path, u32 version)
	{
		std::ifstream file(path + L"\\chunk.dat", std::ios::binary);
		assert(file.is_open());

		if(file.is_open())
		{
			Load(file, version);
			file.close();
		}
	}

	void CChunkNode::Load(std::ifstream& file, u32 version)
	{
		if(version == 0)
		{
			for(int i = 0; i < 4; ++i)
			{
				for(int j = 0; j < 4; ++j)
				{
					for(int k = 0; k < 4; ++k)
					{
						auto coord = Math::VectorInt3(i - 2, j - 2, k - 2);
						auto pChunk = App::CSceneManager::Instance().UniverseManager().ChunkManager().GetChunk(this, coord);
						if(pChunk == nullptr) pChunk = CreateChunk(coord);
						pChunk->Set([&file](Block* pBlockList, size_t blockCount){
							file.read(reinterpret_cast<char*>(pBlockList), sizeof(Block) * blockCount);
					
							for(size_t i = 0; i < blockCount; ++i)
							{
								pBlockList[i].bFilled = !pBlockList[i].bFilled;
							}

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
		else
		{
			Clear();

			u32 sz;
			file.read(reinterpret_cast<char*>(&sz), sizeof(sz));
			for(u32 i = 0; i < sz; ++i)
			{
				Math::VectorInt3 coord;
				file.read(reinterpret_cast<char*>(&coord), sizeof(coord));

				auto pChunk = App::CSceneManager::Instance().UniverseManager().ChunkManager().GetChunk(this, coord);
				if(pChunk == nullptr)
				{
					pChunk = CreateChunk(coord);
				}

				pChunk->Set([&file](Block* pBlockList, size_t blockCount){
					file.read(reinterpret_cast<char*>(pBlockList), sizeof(Block) * blockCount);
				});
			}
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Action methods.
	//-----------------------------------------------------------------------------------------------

	void CChunkNode::Reset()
	{
		App::CSceneManager::Instance().UniverseManager().ChunkManager().ClearNode(this);
	}

	void CChunkNode::Clear()
	{
		Reset();
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

			Math::Vector3 mnExtents, mxExtents;
			GetExtentsWorking(mnExtents, mxExtents);

			std::pair<Math::VectorInt3, u32> indices;
			u32 coordIndex = 0;
			for(int i = 0; i < brushIndices; ++i)
			{
				for(int j = 0; j < brushIndices; ++j)
				{
					for(int k = 0; k < brushIndices; ++k, ++coordIndex)
					{
						const Math::Vector3 offset(
							static_cast<float>(i - brushHalf) + coord.x,
							static_cast<float>(j - brushHalf) + coord.y,
							static_cast<float>(k - brushHalf) + coord.z
						);

						const Math::Vector3 realOffet = offset * m_data.blockSize;
						if(realOffet.x < mnExtents.x || realOffet.x >= mxExtents.x) continue;
						if(realOffet.y < mnExtents.y || realOffet.y >= mxExtents.y) continue;
						if(realOffet.z < mnExtents.z || realOffet.z >= mxExtents.z) continue;

						Block lastBlock = GetBlock(offset, &indices);
									
						if(indices.second == std::numeric_limits<u32>::max())
						{
							continue;
						}

						bEmpty &= !lastBlock.bFilled;
						bFull &= lastBlock.bFilled;

						if(lastBlock.bFilled)
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
						data.coordList[coordIndex].chunkCoord = indices.first;
						data.coordList[coordIndex].blockIndex = indices.second;

						switch(info.alt.U8.lo[1])
						{
							case 0: // Erase
								data.coordList[coordIndex].block.bFilled = false;
								break;
							case 1: // Fill
								data.coordList[coordIndex].block.bFilled = true;
								break;
							case 2: // Paint
								data.coordList[coordIndex].block.bFilled = lastBlock.bFilled;
								break;
							default:
								break;
						}
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
	// Access methods.
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

		res.first = chunkCoord;
		res.second = blockCoord.x * m_data.chunkLength * m_data.chunkHeight + blockCoord.z * m_data.chunkHeight + (blockCoord.y);

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
		//CChunk* pChunk = App::CSceneManager::Instance().UniverseManager().ChunkManager().GetChunk(this, indices.first);
		u32 i, j, k;
		
		i = indices.second;

		j = i % m_data.chunkHeight;
		i = i / m_data.chunkHeight;

		k = i % m_data.chunkLength;
		i = i / m_data.chunkLength;

		return (Math::Vector3(
			static_cast<float>(indices.first.x) * static_cast<int>(m_data.chunkWidth) + i,
			static_cast<float>(indices.first.y) * static_cast<int>(m_data.chunkHeight) + j,
			static_cast<float>(indices.first.z) * static_cast<int>(m_data.chunkLength) + k
		) + 0.5f/* - m_halfChunkSize + 0.5f*/) * m_data.blockSize;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Processing methods.
	//-----------------------------------------------------------------------------------------------

	Block CChunkNode::ReadBlock(const std::pair<Math::VectorInt3, u32>& indices) const
	{
		CChunk* pChunk = App::CSceneManager::Instance().UniverseManager().ChunkManager().GetChunk(this, indices.first);
		if(pChunk == nullptr)
		{
			return Block(0);
		}

		return pChunk->GetBlock(indices.second);
	}

	void CChunkNode::WriteBlock(Block block, const std::pair<Math::VectorInt3, u32>& indices)
	{
		CChunk* pChunk = App::CSceneManager::Instance().UniverseManager().ChunkManager().GetChunk(this, indices.first);
		if(pChunk == nullptr)
		{
			pChunk = CreateChunk(indices.first);//App::CSceneManager::Instance().UniverseManager().ChunkManager().CreateChunk(this, indices.first, false);
		}

		pChunk->SetBlock(indices.second, block.bFilled ? block.id : 256);
	}

	void CChunkNode::ReadBlocksInExtents(const Math::Vector3& mn, const Math::Vector3& mx, std::function<void(u32, Block, const std::pair<Math::VectorInt3, u32>&)> func) const
	{
		ProcessBlocksInExtentsRO(mn, mx, [&](u32 index, const std::pair<Math::VectorInt3, u32>& indices, class CChunk* pChunk){
			func(index, pChunk->GetBlock(indices.second), indices);
		});
	}

	void CChunkNode::WriteBlocksInExtents(const Math::Vector3& mn, const Math::Vector3& mx, std::function<bool(u32, const std::pair<Math::VectorInt3, u32>&, Block&)> func)
	{
		ProcessBlocksInExtents(mn, mx, [&](u32 index, const std::pair<Math::VectorInt3, u32>& indices, class CChunk* pChunk){
			Block block;
			if(func(index, indices, block))
			{
				pChunk->SetBlock(indices.second, block.bFilled ? block.id : 256);
			}
		});
	}

	void CChunkNode::ReadWriteBlocksInExtents(const Math::Vector3& mn, const Math::Vector3& mx, std::function<bool(u32, const std::pair<Math::VectorInt3, u32>&, Block&)> funcWrite,
		std::function<void(u32, Block, const std::pair<Math::VectorInt3, u32>&)> funcRead)
	{
		ProcessBlocksInExtents(mn, mx, [&](u32 index, const std::pair<Math::VectorInt3, u32>& indices, class CChunk* pChunk){
			Block block;
			if(funcWrite(index, indices, block))
			{
				u16 blockId = pChunk->UpdateBlock(indices.second, block.bFilled ? block.id : 256);
				funcRead(index, Block(blockId == 256 ? 0 : blockId, blockId < 256), indices);
			}
			else
			{
				block = pChunk->GetBlock(indices.second);
				funcRead(index, block, indices);
			}
		});
	}

	void CChunkNode::ReadPaintBlocksInExtents(const Math::Vector3& mn, const Math::Vector3& mx, std::function<bool(u32, const std::pair<Math::VectorInt3, u32>&, Block&)> funcWrite,
		std::function<void(u32, Block, const std::pair<Math::VectorInt3, u32>&)> funcRead)
	{
		ProcessBlocksInExtents(mn, mx, [&](u32 index, const std::pair<Math::VectorInt3, u32>& indices, class CChunk* pChunk){
			Block block;
			if(funcWrite(index, indices, block))
			{
				u16 blockId = pChunk->PaintBlock(indices.second, block.bFilled ? block.id : 256);
				funcRead(index, Block(blockId == 256 ? 0 : blockId, blockId < 256), indices);
			}
			else
			{
				block = pChunk->GetBlock(indices.second);
				funcRead(index, block, indices);
			}
		});
	}
	
	void CChunkNode::ProcessBlocksInExtents(const Math::Vector3& mn, const Math::Vector3& mx, std::function<void(u32, const std::pair<Math::VectorInt3, u32>&, class CChunk*)> func)
	{
		ProcessBlocksInExtentsRO(mn, mx, func, [this](const Math::VectorInt3& coord){ return CreateChunk(coord); });
	}

	void CChunkNode::ProcessBlocksInExtentsRO(const Math::Vector3& mn, const Math::Vector3& mx, std::function<void(u32, const std::pair<Math::VectorInt3, u32>&, class CChunk*)> func,
		std::function<class CChunk*(const Math::VectorInt3&)> onChunkNotFound) const
	{
		Math::VectorInt3 mnInt(
			static_cast<int>(roundf(mn.x / m_data.blockSize)),
			static_cast<int>(roundf(mn.y / m_data.blockSize)),
			static_cast<int>(roundf(mn.z / m_data.blockSize))
		);

		Math::VectorInt3 mxInt(
			static_cast<int>(roundf(mx.x / m_data.blockSize)),
			static_cast<int>(roundf(mx.y / m_data.blockSize)),
			static_cast<int>(roundf(mx.z / m_data.blockSize))
		);

		std::pair<Math::VectorInt3, u32> indices;
		Math::Vector3 coord;
		Math::VectorInt3 chunkCoordLast;
		Math::VectorInt3 blockCoord;
		CChunk* pChunk = nullptr;
		Block block;
		
		u32 index = 0;
		for(int x = mnInt.x; x < mxInt.x; ++x)
		{
			coord.x = static_cast<float>(x);
			indices.first.x = static_cast<int>(floorf(coord.x / m_data.chunkWidth));
			blockCoord.x = static_cast<int>(floorf(coord.x - indices.first.x * static_cast<int>(m_data.chunkWidth))) * m_data.chunkLength * m_data.chunkHeight;
			
			for(int z = mnInt.z; z < mxInt.z; ++z)
			{
				coord.z = static_cast<float>(z);
				indices.first.z = static_cast<int>(floorf(coord.z / m_data.chunkLength));
				blockCoord.z = static_cast<int>(floorf(coord.z - indices.first.z * static_cast<int>(m_data.chunkLength))) * m_data.chunkHeight;

				for(int y = mnInt.y; y < mxInt.y; ++y)
				{
					coord.y = static_cast<float>(y);
					indices.first.y = static_cast<int>(floorf(coord.y / m_data.chunkHeight));
					blockCoord.y = static_cast<int>(floorf(coord.y - indices.first.y * static_cast<int>(m_data.chunkHeight)));

					if(!pChunk || chunkCoordLast != indices.first)
					{
						pChunk = App::CSceneManager::Instance().UniverseManager().ChunkManager().GetChunk(this, indices.first);

						if(pChunk == nullptr && onChunkNotFound)
						{
							pChunk = onChunkNotFound(indices.first);
							//pChunk = CreateChunk(indices.first);//App::CSceneManager::Instance().UniverseManager().ChunkManager().CreateChunk(this, indices.first, false);
						}

						chunkCoordLast = indices.first;
					}

					indices.second = blockCoord.x + blockCoord.y + blockCoord.z;
					func(index++, indices, pChunk);
				}
			}
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	CChunk* CChunkNode::CreateChunk(const Math::VectorInt3& coords)
	{
		for(size_t i = 0; i < 3; ++i)
		{
			m_minExtents[i] = std::min(m_minExtents[i], static_cast<float>(coords[i]) * m_data.chunkWidth * m_data.blockSize);
			m_maxExtents[i] = std::max(m_maxExtents[i], static_cast<float>(coords[i] + 1) * m_data.chunkWidth * m_data.blockSize);
		}

		CChunk* pChunk = App::CSceneManager::Instance().UniverseManager().ChunkManager().CreateChunk(this, coords, false);
		return pChunk;
	}
	
	void CChunkNode::FrustumCulling() const
	{
		FrustumCulling(m_minExtents, m_maxExtents);
	}

	// Frustum culling with an implicit octree.
	void CChunkNode::FrustumCulling(const Math::Vector3& mn, const Math::Vector3& mx) const
	{
		if(mn.x == mx.x || mn.y == mx.y || mn.z == mx.z) return;

		const Math::Vector3* mnmx[2] = { &mn, &mx };

		bool bIntersecting = true;

		for(size_t i = 0; i < 6; ++i)
		{
			Math::Vector3 normal = *(Math::Vector3*)App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetFrustumPlane(i).ToFloat();
			Math::Vector3 nearPoint(mnmx[normal.x > 0.0f]->x, mnmx[normal.y > 0.0f]->y, mnmx[normal.z > 0.0f]->z);

			if(_mm_cvtss_f32(App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetFrustumPlane(i).DotCoord(nearPoint)) < 0.0f)
			{
				bIntersecting = false;
				break;
			}
		}

		const u32 chunkSize[3] = { m_data.chunkWidth, m_data.chunkHeight, m_data.chunkLength };

		const Math::VectorInt3 mnInt(
			static_cast<int>(roundf(mn.x / m_data.blockSize / m_data.chunkWidth)),
			static_cast<int>(roundf(mn.y / m_data.blockSize / m_data.chunkHeight)),
			static_cast<int>(roundf(mn.z / m_data.blockSize / m_data.chunkLength))
		);

		const Math::VectorInt3 mxInt(
			static_cast<int>(roundf(mx.x / m_data.blockSize / m_data.chunkWidth)),
			static_cast<int>(roundf(mx.y / m_data.blockSize / m_data.chunkHeight)),
			static_cast<int>(roundf(mx.z / m_data.blockSize / m_data.chunkLength))
		);

		const Math::VectorInt3 diff = mxInt - mnInt;
		const bool bSingleCell = diff.x == 1 && diff.y == 1 && diff.z == 1;

		if(bIntersecting)
		{
			auto FullyInside = [&](){
				if(!App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->CheckPoint(Math::Vector3(mn.x, mn.y, mn.z))) return false;
				if(!App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->CheckPoint(Math::Vector3(mx.x, mn.y, mn.z))) return false;
				if(!App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->CheckPoint(Math::Vector3(mn.x, mn.y, mx.z))) return false;
				if(!App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->CheckPoint(Math::Vector3(mx.x, mn.y, mx.z))) return false;
				if(!App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->CheckPoint(Math::Vector3(mn.x, mx.y, mn.z))) return false;
				if(!App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->CheckPoint(Math::Vector3(mx.x, mx.y, mn.z))) return false;
				if(!App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->CheckPoint(Math::Vector3(mn.x, mx.y, mx.z))) return false;
				if(!App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->CheckPoint(Math::Vector3(mx.x, mx.y, mx.z))) return false;
				return true;
			};
			
			if(bSingleCell || FullyInside())
			{ // Found cell at minimal size or fully inside frustum.
				SetChunksInExtentsToRender(mnInt, mxInt);
			}
			else
			{ // Continue with recursion.
				Math::Vector3 half[2];
				for(size_t i = 0; i < 3; ++i)
				{
					if(diff[i] & 0x1)
					{ // Odd
						half[0][i] = m_data.blockSize * ((diff[i] >> 1) * chunkSize[i]);
						half[1][i] = m_data.blockSize * (((diff[i] >> 1) + 1) * chunkSize[i]);
					}
					else
					{ // Even
						half[0][i] = half[1][i] = m_data.blockSize * ((diff[i] >> 1) * chunkSize[i]);
					}
				}
			
				u32 mnMask = 0xFF;

				if(half[0].x == 0) mnMask &= ~(2 | 8 | 32 | 128);
				if(half[0].y == 0) mnMask &= ~(16 | 32 | 64 | 128);
				if(half[0].z == 0) mnMask &= ~(4 | 8 | 64 | 128);
				
				for(u32 i = 0; i < 2; ++i)
				{
					for(u32 j = 0; j < 3; ++j)
					{
						half[i][j] = std::max(m_data.blockSize * chunkSize[j], half[i][j]);
					}
				}

				const Math::Vector3 mnList[] = {
					{ mn.x, mn.y, mn.z },
					{ mn.x + half[1].x, mn.y, mn.z },
					{ mn.x, mn.y, mn.z + half[1].z },
					{ mn.x + half[1].x, mn.y, mn.z + half[1].z },

					{ mn.x, mn.y + half[1].y, mn.z },
					{ mn.x + half[1].x, mn.y + half[1].y, mn.z },
					{ mn.x, mn.y + half[1].y, mn.z + half[1].z },
					{ mn.x + half[1].x, mn.y + half[1].y, mn.z + half[1].z },
				};

				for(u32 i = 0; i < 8; ++i)
				{
					if(mnMask & (0x1 << i))
					{
						FrustumCulling(mnList[i], mnList[i] + Math::Vector3(half[!(i & 1)].x, half[!((i >> 2) & 1)].y, half[!((i >> 1) & 1)].z));
					}
				}
			}
		}
	}

	void CChunkNode::SetChunksInExtentsToRender(const Math::VectorInt3& mn, const Math::VectorInt3& mx) const
	{
		for(int i = mn.x; i < mx.x; ++i)
		{
			for(int j = mn.y; j < mx.y; ++j)
			{
				for(int k = mn.z; k < mx.z; ++k)
				{
					CChunk* pChunk = App::CSceneManager::Instance().UniverseManager().ChunkManager().GetChunk(this, Math::VectorInt3(i, j, k));
					if(pChunk)
					{
						App::CSceneManager::Instance().UniverseManager().ChunkManager().QueueChunkRender(pChunk);
					}
				}
			}
		}
	}
};
