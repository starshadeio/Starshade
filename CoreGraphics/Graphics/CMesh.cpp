//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMesh.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMesh.h"

namespace Graphics
{
	CMesh::CMesh() :
		m_vertexSize(0),
		m_indexSize(0),
		m_pVertexList(nullptr),
		m_pIndexList(nullptr),
		m_pData(nullptr)
	{
	}

	CMesh::~CMesh()
	{
	}

	void CMesh::Initialize()
	{
		m_vertexSize = m_data.vertexCount * m_data.vertexStride;
		m_indexSize = m_data.indexCount * m_data.indexStride;
		
		const u32 totalSize = m_vertexSize + m_indexSize;
		m_pData = new u8[totalSize];
		m_pVertexList = m_pData;
		m_pIndexList = m_pVertexList + m_vertexSize;

		if(m_data.filename.size())
		{
		}
		else if(m_data.pData)
		{
			u8* pData = m_data.pData;
			memcpy(m_pVertexList, pData, m_vertexSize);
			pData += m_vertexSize;
			memcpy(m_pIndexList, pData, m_indexSize);
		}
	}
	
	void CMesh::SaveProductionFile()
	{
	}

	void CMesh::Release()
	{
		delete [] m_pData;
		m_pData = m_pVertexList = m_pIndexList = nullptr;
	}
};