//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunkManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCHUNKMANAGER_H
#define CCHUNKMANAGER_H

#include "CChunk.h"
#include <Math/CMathVectorInt3.h>
#include <Math/CMathFNV.h>
#include <Objects/CVObject.h>
#include <Logic/CTransform.h>
#include <unordered_map>
#include <queue>
#include <cassert>
#include <vector>

namespace Graphics
{
	class CMaterial;
	class CTexture;
};

namespace Universe
{
	class CChunkManager : CVObject
	{
	public:
		static const u64 CMD_KEY_BLOCK_EDIT = Math::FNV1a_64("block_edit");

	public:
		struct Data
		{
			u32 viewHash = 0;
		};

	public:
		struct ChunkBlockUpdateData
		{
			struct Coord
			{
				Block block;
				Math::VectorInt3 chunkCoord;
				u32 blockIndex;
			};

			class CChunkNode* pChunkNode;

			size_t coordCount = 1;
			Coord coordList[64];

			inline size_t GetSize() const
			{
				assert(coordCount <= (sizeof(coordList)) / sizeof(Coord));
				return sizeof(ChunkBlockUpdateData) - sizeof(Coord) * (64 - coordCount);
			}
		};

	private:
		struct ChunkKeyHasher
		{
			size_t operator()(const Math::VectorInt3& k) const
			{
				size_t hash = 0xcbf29ce484222325;

				for(size_t i = 0; i < 3; ++i)
				{
					hash = hash ^ k[i];
					hash *= 0x100000001B3;
				}
				
				return hash;
			}
		};

	public:
		CChunkManager();
		~CChunkManager();
		CChunkManager(const CChunkManager&) = delete;
		CChunkManager(CChunkManager&&) = delete;
		CChunkManager& operator = (const CChunkManager&) = delete;
		CChunkManager& operator = (CChunkManager&&) = delete;
		
		void Initialize();
		void Update();
		void LateUpdate();
		void Render(u32 hash);
		void Release();

		bool ClearNode(const class CChunkNode* pChunkNode);

		u8 LODDown(const class CChunkNode* pChunkNode);
		u8 LODUp(const class CChunkNode* pChunkNode);

		// Accessors.
		inline CChunk* GetChunk(const class CChunkNode* pChunkNode, const Math::VectorInt3& chunkCoord)
		{
			auto node = m_chunkMap.find(pChunkNode);
			if(node == m_chunkMap.end()) return nullptr;
			auto chunk = node->second.find(chunkCoord);
			if(chunk == node->second.end()) return nullptr;
			return chunk->second;
		}

		inline std::unordered_map<Math::VectorInt3, CChunk*, ChunkKeyHasher>& GetChunkMap(const class CChunkNode* pNode) { return m_chunkMap.find(pNode)->second; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetTexture(Graphics::CTexture* pTexture) { m_pTexture = pTexture; }
		inline void QueueChunkUpdate(class CChunk* pChunk) { m_updateQueue.push(pChunk); }
		inline void QueueChunkRender(class CChunk* pChunk) { m_renderQueue.push(pChunk); }
		
	private:
		CChunk* RegisterChunk(const class CChunkNode* pChunkNode, const Math::VectorInt3& chunkCoord, const CChunk::Data& data, bool bInitIfNotFound = true);
		bool DeregisterChunk(const class CChunkNode* pChunkNode, const Math::VectorInt3& chunkCoord);

		bool BlockEdit(const void* param, bool bInverse);

	public:
		CChunk* CreateChunk(const class CChunkNode* pChunkNode, const Math::VectorInt3& chunkCoord, bool bBuild = true);

	private:
		Data m_data;

		std::unordered_map<const class CChunkNode*, std::unordered_map<Math::VectorInt3, CChunk*, ChunkKeyHasher>> m_chunkMap;
		std::queue<class CChunk*> m_updateQueue;
		std::queue<class CChunk*> m_renderQueue;

		Graphics::CMaterial* m_pMaterial;
		Graphics::CMaterial* m_pMaterialWire;
		Graphics::CTexture* m_pTexture;
	};
};

#endif
