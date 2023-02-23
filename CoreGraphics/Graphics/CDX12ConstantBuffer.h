//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12ConstantBuffer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDX12CONSTANTBUFFER_H
#define CDX12CONSTANTBUFFER_H

#include "CConstantBuffer.h"
#include <d3d12.h>

namespace Graphics
{
	class CDX12ConstantBuffer : public CConstantBuffer
	{
	public:
		CDX12ConstantBuffer();
		~CDX12ConstantBuffer();
		CDX12ConstantBuffer(const CDX12ConstantBuffer&) = delete;
		CDX12ConstantBuffer(CDX12ConstantBuffer&&) = delete;
		CDX12ConstantBuffer& operator = (const CDX12ConstantBuffer&) = delete;
		CDX12ConstantBuffer& operator = (CDX12ConstantBuffer&&) = delete;

		void Initialize() final;
		void PostInitialize() final;
		void Bind(u32& index, bool bCompute) final;
		void Release() final;

	private:
		u32 m_32BitValueCount;
		u32 m_heapIndex;

		ID3D12Resource* m_pConstantBuffer;
		ID3D12Resource* m_pConstantBufferUpload;

		class CDX12Graphics* m_pDX12Graphics;
	};
};

#endif
