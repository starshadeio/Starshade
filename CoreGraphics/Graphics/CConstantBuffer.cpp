//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CConstantBuffer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CConstantBuffer.h"

namespace Graphics
{
	CConstantBuffer::CConstantBuffer() :
		m_bDirty(false),
		m_pCbvDataBegin(nullptr) {
	}

	CConstantBuffer::~CConstantBuffer() {
	}

	void CConstantBuffer::Bind(u32& index, bool bCompute)
	{
		if(!m_data.bRootConstant && !m_data.bFixed)
		{
			if(m_bDirty)
			{
				UpdateData();
				m_bDirty = false;
			}
		}
	}
};
