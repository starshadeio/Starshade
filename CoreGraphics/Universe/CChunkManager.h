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
#include <cassert>

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

			const class CChunkNode* pChunkNode;

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
		void Release();

		CChunk* RegisterChunk(const class CChunkNode* pChunkNode, const Math::VectorInt3& chunkCoord, const CChunk::Data& data);
		bool DeregisterChunk(const class CChunkNode* pChunkNode, const Math::VectorInt3& chunkCoord);

		// Accessors.
		inline CChunk* GetChunk(const class CChunkNode* pChunkNode, const Math::VectorInt3& chunkCoord)
		{
			auto node = m_chunkMap.find(pChunkNode);
			auto chunk = node->second.find(chunkCoord);
			if(chunk == node->second.end()) return nullptr;
			return chunk->second;
		}

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetTexture(Graphics::CTexture* pTexture) { m_pTexture = pTexture; }
		
	private:
		bool BlockEdit(const void* param, bool bInverse);

	private:
		Data m_data;

		std::unordered_map<const class CChunkNode*, std::unordered_map<Math::VectorInt3, CChunk*, ChunkKeyHasher>> m_chunkMap;

		Graphics::CMaterial* m_pMaterial;
		Graphics::CMaterial* m_pMaterialWire;
		Graphics::CTexture* m_pTexture;
	};
};

#endif
