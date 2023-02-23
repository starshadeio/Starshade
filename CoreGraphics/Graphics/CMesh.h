//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMesh.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMESH_H
#define CMESH_H

#include "CGraphicsData.h"
#include <string>

namespace Graphics
{
	class CMesh
	{
	public:
		struct Data
		{
			PRIMITIVE_TOPOLOGY topology;

			u32 vertexCount;
			u32 indexCount;

			u32 vertexStride;
			u32 indexStride;

			std::wstring filename;
			u8* pData;
		};

	public:
		CMesh();
		~CMesh();
		CMesh(const CMesh&) = delete;
		CMesh(CMesh&&) = delete;
		CMesh& operator = (const CMesh&) = delete;
		CMesh& operator = (CMesh&&) = delete;

		void Initialize();
		void SaveProductionFile();
		void Release();

		// Accessors.
		inline PRIMITIVE_TOPOLOGY GetTopology() const { return m_data.topology; }

		inline u32 GetVertexCount() const { return m_data.vertexCount; }
		inline u32 GetVertexStride() const { return m_data.vertexStride; }
		inline u32 GetVertexSize() const { return m_vertexSize; }

		inline u32 GetIndexCount() const { return m_data.indexCount; }
		inline u32 GetIndexStride() const { return m_data.indexStride; }
		inline u32 GetIndexSize() const { return m_indexSize; }

		inline const u8* GetVertexList() const { return m_pVertexList; }
		inline const u8* GetIndexList() const { return m_pIndexList; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

		inline u8* GetVertexAt(u32 vertex) { return m_pVertexList + vertex * m_data.vertexStride; }
		inline u8* GetNextVertex(size_t& offset) { u8* pVertex = m_pVertexList + offset; offset += m_data.vertexStride; return pVertex; }

		inline u8* GetIndexAt(u32 index) { return m_pIndexList + index * m_data.indexStride; }
		inline u8* GetNextIndex(size_t& offset) { u8* pIndex = m_pIndexList + offset; offset += m_data.indexStride; return pIndex; }

	private:
		Data m_data;

		u32 m_vertexSize;
		u32 m_indexSize;

		u8* m_pVertexList;
		u8* m_pIndexList;
		u8* m_pData;
	};
};

#endif
