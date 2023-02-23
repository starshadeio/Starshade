//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12UAVBuffer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDX12UAVBUFFER_H
#define CDX12UAVBUFFER_H

#include "CUAVBuffer.h"
#include <d3d12.h>

namespace Graphics
{
	class CDX12UAVBuffer : public CUAVBuffer
	{
	public:
		CDX12UAVBuffer();
		~CDX12UAVBuffer();
		CDX12UAVBuffer(const CDX12UAVBuffer&) = delete;
		CDX12UAVBuffer(CDX12UAVBuffer&&) = delete;
		CDX12UAVBuffer& operator= (const CDX12UAVBuffer&) = delete;
		CDX12UAVBuffer& operator= (CDX12UAVBuffer&&) = delete;

		void Initialize() final;
		void PostInitialize() final;
		void Bind(u32& index, bool bCompute) final;
		void Release() final;

	private:
		u32 m_heapIndex;

		ID3D12Resource* m_pUAVBuffer;
		ID3D12Resource* m_pUAVBufferUpload;

		class CDX12Graphics* m_pDX12Graphics;
	};
};

#endif
