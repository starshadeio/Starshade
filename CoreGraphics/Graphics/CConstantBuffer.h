//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CConstantBuffer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCONSTANTBUFFER_H
#define CCONSTANTBUFFER_H

#include "CGraphicsData.h"
#include <Globals/CGlobals.h>

namespace Graphics
{
	class CConstantBuffer
	{
	public:
		struct Data
		{
			bool bFixed;
			bool bRootConstant;

			u32 bufferSize;
			u8* pBufferData;
		};

	protected:
		CConstantBuffer();
		CConstantBuffer(const CConstantBuffer&) = delete;
		CConstantBuffer(CConstantBuffer&&) = delete;
		CConstantBuffer& operator = (const CConstantBuffer&) = delete;
		CConstantBuffer& operator = (CConstantBuffer&&) = delete;

	public:
		virtual ~CConstantBuffer();

		virtual void Initialize() = 0;
		virtual void PostInitialize() = 0;
		virtual void Bind(u32& index, bool bCompute);
		virtual void Release() = 0;

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetAsDirty() { m_bDirty = true; }
		
	protected:
		void UpdateData()
		{
			memcpy(m_pCbvDataBegin, m_data.pBufferData, m_data.bufferSize);
		}

	protected:
		bool m_bDirty;
		Data m_data;

		u8* m_pCbvDataBegin;
	};
};

#endif
