//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshData_.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMeshData_.h"

namespace Graphics
{
	CMeshData_::CMeshData_(const class CVObject* pObject) :
		CVComponent(pObject),
		m_bInstanceDirty(false),
		m_vertexSize(0),
		m_instanceSize(0),
		m_instanceCount(0),
		m_indexSize(0),
		m_pVertexList(nullptr),
		m_pInstanceList(nullptr),
		m_pIndexList(nullptr),
		m_pBuffer(nullptr) {
	}

	CMeshData_::~CMeshData_() { }

	void CMeshData_::Initialize()
	{
		m_vertexSize = m_data.vertexCount * m_data.vertexStride;
		m_instanceSize = m_data.instanceMax * m_data.instanceStride;
		m_indexSize = m_data.indexCount * m_data.indexStride;
		const u32 totalSize = m_vertexSize + m_instanceSize + m_indexSize;

		m_pBuffer = new u8[totalSize];
		m_pVertexList = m_pBuffer;
		m_pInstanceList = m_pVertexList + m_vertexSize;
		m_pIndexList = m_pInstanceList + m_instanceSize;
	}

	void CMeshData_::Release()
	{
		delete[] m_pBuffer;
		m_pIndexList = m_pInstanceList = m_pVertexList = m_pBuffer = nullptr;
	}

	//-----------------------------------------------------------------------------------------------
	// Data processors.
	//-----------------------------------------------------------------------------------------------

	void CMeshData_::ProcessVertexList(std::function<void(u32, u8*)> processor)
	{
		u8* pVertex = m_pVertexList;
		for(u32 i = 0; i < m_data.vertexCount; ++i)
		{
			processor(i, pVertex);
			pVertex += m_data.vertexStride;
		}
	}

	void CMeshData_::ProcessIndexList(std::function<void(u32, u8*)> processor)
	{
		u8* pIndex = m_pIndexList;
		for(u32 i = 0; i < m_data.indexCount; ++i)
		{
			processor(i, pIndex);
			pIndex += m_data.indexStride;
		}
	}
};
