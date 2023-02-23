//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CUAVBuffer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUAVBUFFER_H
#define CUAVBUFFER_H

#include <Globals/CGlobals.h>

namespace Graphics
{
	class CUAVBuffer
	{
	public:
		struct Data
		{
			u32 bufferStride;
			u32 bufferSize;
			u8* pBufferData;
		};

	protected:
		CUAVBuffer() { }
		CUAVBuffer(const CUAVBuffer&) = delete;
		CUAVBuffer(CUAVBuffer&&) = delete;
		CUAVBuffer& operator = (const CUAVBuffer&) = delete;
		CUAVBuffer& operator = (CUAVBuffer&&) = delete;

	public:
		virtual ~CUAVBuffer() { }

		virtual void Initialize() = 0;
		virtual void PostInitialize() = 0;
		virtual void Bind(u32& index, bool bCompute) = 0;
		virtual void Release() = 0;

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	protected:
		Data m_data;
	};
};

#endif
