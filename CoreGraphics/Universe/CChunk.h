//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunk.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCHUNK_H
#define CCHUNK_H

#include "CChunkData.h"
#include "../Physics/CVolumeChunk.h"
#include "../Graphics/CMeshData_.h"
#include "../Graphics/CMeshContainer_.h"
#include <Math/CMathVector2.h>
#include <Math/CMathVectorInt3.h>
#include <Globals/CGlobals.h>
#include <Utilities/CFuture.h>
#include <Objects/CVComponent.h>
#include <shared_mutex>
#include <unordered_set>
#include <unordered_map>

#if _DEBUG
#include <cassert>
#endif

namespace Graphics
{
	class CMeshRenderer_;
	class CMaterial;
};

namespace Universe
{
	class CChunk : CVComponent
	{
	private:
		struct Vertex
		{
			Math::Vector3 position;
			Math::Vector3 normal;
			Math::Vector3 tangent;
			Math::Vector4 texCoord;
		};

		typedef u32 Index;

	public:
		struct Data
		{
			Math::VectorInt3 offset;
			u32 width;
			u32 height;
			u32 length;
			float blockSize;
			u64 matHash;
			u64 matWireHash;
		};

	public:
		CChunk(const CVObject* pObject);
		~CChunk();
		CChunk(const CChunk&) = delete;
		CChunk(CChunk&&) = delete;
		CChunk& operator = (const CChunk&) = delete;
		CChunk& operator = (CChunk&&) = delete;

		void Initialize();
		void Release();
		
		u16 UpdateBlock(u32 index, u16 block);
		void Set(const Block* pBlocks);
		void Set(std::function<void(Block*, size_t)> func);
		void Read(std::function<void(const Block*, size_t)> func);
		void Reset();
		void Clear();
		
		// Accessors.
		inline Math::Vector3 GetOffset() const
		{
			//std::shared_lock<std::shared_mutex> lk(m_mutex);
			return Math::Vector3(
				static_cast<float>(m_data.offset.x) * m_data.blockSize,
				static_cast<float>(m_data.offset.y) * m_data.blockSize,
				static_cast<float>(m_data.offset.z) * m_data.blockSize
			);
		}
		
		inline u32 GetIndex(u32 i, u32 j, u32 k) const
		{
			//std::shared_lock<std::shared_mutex> lk(m_mutex);
			return internalGetIndex(i, j, k);
		}

		inline void GetCoordsFromIndex(u32 index, u32& i, u32& j, u32& k) const
		{
			//std::shared_lock<std::shared_mutex> lk(m_mutex);
			internalGetCoordsFromIndex(index, i, j, k);
		}

		inline Block GetBlock(u32 index) const
		{
			std::shared_lock<std::shared_mutex> lk(m_mutex);
			return m_pBlockList[index];
		}
		
		inline Block GetBlock(u32 i, u32 j, u32 k, u32* pIndex = nullptr) const
		{
			std::shared_lock<std::shared_mutex> lk(m_mutex);
			const u32 index = internalGetIndex(i, j, k);
			if(pIndex) *pIndex = index;
			return m_pBlockList[index];
		}

		// Modifiers.
		inline void SetData(const Data& data)
		{
#if _DEBUG
			assert(!m_bInitialized);
#endif
			std::lock_guard<std::shared_mutex> lk(m_mutex);
			m_data = data;
		}

		inline void SetAdjacentChunk(SIDE side, CChunk* pChunk)
		{
			std::lock_guard<std::shared_mutex> lk(m_mutex);
			m_pChunkAdj[side] = pChunk;
		}

	private:
		void BuildMesh(u8 meshIndex, bool bOptimize);
		void RebuildMesh();

		struct QuadSides
		{
			union
			{
				struct
				{
					SIDE_FLAG u;
					SIDE_FLAG l;
					SIDE_FLAG r;
					SIDE_FLAG b;
					SIDE_FLAG f;
				};

				SIDE_FLAG v[5];
			};
		};

		struct QuadEdges
		{
			struct Edge
			{
				Math::VectorInt3 e0;
				Math::VectorInt3 e1;
			};

			union
			{
				struct
				{
					Edge l;
					Edge r;
					Edge b;
					Edge f;
				};

				Edge e[4];
			};
		};

		void ProcessIsland(std::unordered_set<u32>& set, std::unordered_set<u32>::iterator it, u32 iStep, u32 kStep, u32 maxIndex, u32 iAxis, u32 kAxis, const QuadSides& flags, const QuadEdges& edges, std::unordered_map<u32, u64>& disjointedSet);
		void GenerateOptimalMeshData(u8 meshIndex, Graphics::CMeshData_::Data& data);

		void PreRender();
		
		// Internal accessors.
		inline u32 internalGetIndex(u32 i, u32 j, u32 k) const
		{
			return i * m_data.length * m_data.height + k * m_data.height + (j);
		}
		
		inline u32 internalGetVertexIndex(u32 i, u32 j, u32 k) const
		{
			return i * (m_data.length + 1) * (m_data.height + 1) + k * (m_data.height + 1) + (j);
		}

		inline u32 internalGetIndexLeft(u32 j, u32 k) const { return internalGetIndex(0, j, k); }
		inline u32 internalGetIndexRight(u32 j, u32 k) const { return internalGetIndex(m_data.width - 1, j, k); }

		inline u32 internalGetIndexBottom(u32 i, u32 k) const { return internalGetIndex(i, 0, k); }
		inline u32 internalGetIndexTop(u32 i, u32 k) const { return internalGetIndex(i, m_data.height - 1, k); }
		
		inline u32 internalGetIndexBack(u32 i, u32 j) const { return internalGetIndex(i, j, 0); }
		inline u32 internalGetIndexFront(u32 i, u32 j) const { return internalGetIndex(i, j, m_data.length - 1); }

		inline void internalGetCoordsFromIndex(u32 index, u32& i, u32& j, u32& k) const
		{
			i = index;

			j = i % m_data.height;
			i = i / m_data.height;

			k = i % m_data.length;
			i = i / m_data.length;
		}
		
		inline void internalGetCoordsFromVertexIndex(u32 index, u32& i, u32& j, u32& k) const
		{
			i = index;

			j = i % (m_data.height + 1);
			i = i / (m_data.height + 1);

			k = i % (m_data.length + 1);
			i = i / (m_data.length + 1);
		}

	private:
		mutable std::shared_mutex m_mutex;
		
#if _DEBUG
		bool m_bInitialized;
#endif

		bool m_bDirty;
		bool m_bAwaitingRebuild;
		u8 m_meshIndex;
		u32 m_chunkSize;

		Data m_data;
		
		std::unordered_map<u32, u16> m_blockUpdateMap;

		Graphics::CMeshData_ m_meshData[2];
		Graphics::CMeshContainer_ m_meshContainer;
		Graphics::CMeshContainer_ m_meshContainerWire;
		Graphics::CMeshRenderer_* m_pMeshRendererList[2];
		//Graphics::CMeshRenderer_* m_pMeshRendererListWire[2];
		Util::CFuture<void> m_meshFuture[2];
		Graphics::CMaterial* m_pMaterial;
		Graphics::CMaterial* m_pMaterialWire;

		CChunk* m_pChunkAdj[6];
		
		Block* m_pBlockList;
	};
};

#endif
