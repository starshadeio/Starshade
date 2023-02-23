//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12Texture.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDX12TEXTURE_H
#define CDX12TEXTURE_H

#include "CTexture.h"
#include <d3d12.h>

#pragma comment(lib, "DirectXTK12.lib")

namespace Graphics
{
	class CDX12Texture : public CTexture
	{
	public:
		CDX12Texture();
		~CDX12Texture();
		CDX12Texture(const CDX12Texture&) = delete;
		CDX12Texture(CDX12Texture&&) = delete;
		CDX12Texture& operator = (const CDX12Texture&) = delete;
		CDX12Texture& operator = (CDX12Texture&&) = delete;

		void Initialize() final;
		void PostInitialize() final;
		void Bind(u32& index, bool bCompute) final;
		void Release() final;

		void* GetCPUHandle(DESC_HEAP_TYPE tp) final;

		void TransitionToShader() final;
		void TransitionFromShader() final;

		void SaveAsTexture(const wchar_t* filename) final;

	private:
		bool m_bHeapIndexed;
		u32 m_csuIndex;
		union
		{
			u32 m_rtvIndex;
			u32 m_dsvIndex;
		};

		GFX_FORMAT m_resourceFormat;
		D3D12_SRV_DIMENSION m_resourceDimension;
		D3D12_RESOURCE_STATES m_resourceStateFrom;
		D3D12_RESOURCE_STATES m_resourceStateTo;
		D3D12_RESOURCE_STATES m_resourceState;

		ID3D12Resource* m_pTexture;
		ID3D12Resource* m_pTextureUpload;

		class CDX12Graphics* m_pDX12Graphics;
	};
};

#endif
