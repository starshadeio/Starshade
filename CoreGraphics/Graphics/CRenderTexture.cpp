//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRenderTexture.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CRenderTexture.h"
#include "CTexture.h"
#include "CGraphicsAPI.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include <Utilities/CMemoryFree.h>
#include <Math/CMathFNV.h>

namespace Graphics
{
	CRenderTexture::CRenderTexture() :
		m_rtvTextureHash { 0, 0, 0, 0, 0, 0, 0, 0 },
		m_dsvTextureHash(0),
		m_pRTVTexture { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
		m_pDSVTexture(nullptr),
		m_GraphicsAPI(nullptr) {
	}

	CRenderTexture::~CRenderTexture() { }

	void CRenderTexture::Initialize()
	{
		m_GraphicsAPI = CFactory::Instance().GetGraphicsAPI();

		if(m_data.colorBufferCount)
		{
			for(u8 i = 0; i < m_data.colorBufferCount; ++i)
			{
				m_pRTVTexture[i] = CFactory::Instance().CreateTexture();
				m_rtvTextureHash[i] = Math::FNV1a_64(m_data.colorTexKey[i].c_str());
				Resources::CManager::Instance().AddResourceEx(Resources::RESOURCE_TYPE_TEXTURE, m_rtvTextureHash[i], m_pRTVTexture[i]);
			}
		}

		if(m_data.bUseDepthStencil)
		{
			m_pDSVTexture = CFactory::Instance().CreateTexture();
			m_dsvTextureHash = Math::FNV1a_64(m_data.depthTexKey.c_str());
			Resources::CManager::Instance().AddResourceEx(Resources::RESOURCE_TYPE_TEXTURE, m_dsvTextureHash, m_pDSVTexture);
		}

		Resize();
	}

	void CRenderTexture::PostInitialize()
	{
		if(m_data.colorBufferCount)
		{
			for(u8 i = 0; i < m_data.colorBufferCount; ++i)
			{
				m_pRTVTexture[i]->PostInitialize();
			}
		}

		if(m_data.bUseDepthStencil)
		{
			m_pDSVTexture->PostInitialize();
		}
	}

	// Method for initializing/resizing render texture(s).
	void CRenderTexture::Resize()
	{
		CTexture::Data data { };
		data.width = m_data.width >> m_data.downScale;
		data.height = m_data.height >> m_data.downScale;
		data.depth = 1;
		data.pRenderTexture = this;

		for(u8 i = 0; i < m_data.colorBufferCount; ++i)
		{
			m_pRTVTexture[i]->Release();

			data.index = i;
			data.textureFormat = m_data.colorFormat[i];
			data.textureType = CTexture::TTP_RTV;
			m_pRTVTexture[i]->SetData(data);

			m_pRTVTexture[i]->Initialize();
		}

		if(m_data.bUseDepthStencil)
		{
			m_pDSVTexture->Release();

			data.index = 0;
			data.textureFormat = m_data.depthFormat;
			data.textureType = CTexture::TTP_DSV;
			m_pDSVTexture->SetData(data);

			m_pDSVTexture->Initialize();
		}

		// Setup view and scissor rects.
		m_viewport.topLeftX = m_viewport.topLeftY = 0.0f;
		m_viewport.minDepth = 0.0f;
		m_viewport.maxDepth = 1.0f;
		m_viewport.width = static_cast<float>(data.width);
		m_viewport.height = static_cast<float>(data.height);

		m_scissorRect.left = 0;
		m_scissorRect.top = 0;
		m_scissorRect.right = data.width;
		m_scissorRect.bottom = data.height;

		m_aspectRatio = m_viewport.width / m_viewport.height;
	}

	// Method for beginning render to texture.
	void CRenderTexture::BeginRender(CLEAR_MODE clearMode, u32 numRects, const Math::RectLTRB* pRects)
	{
		CGraphicsAPI::RenderTargetData rtData { };

		rtData.targetCount = m_data.colorBufferCount;
		for(u8 i = 0; i < m_data.colorBufferCount; ++i)
		{
			m_pRTVTexture[i]->TransitionFromShader();

			rtData.pRtvHandle[i] = m_pRTVTexture[i]->GetCPUHandle(DHT_RTV);
		}

		if(m_data.bUseDepthStencil)
		{
			m_pDSVTexture->TransitionFromShader();

			rtData.pDsvHandle = m_pDSVTexture->GetCPUHandle(DHT_DSV);
		}

		rtData.viewport = m_viewport;
		rtData.scissorRect = m_scissorRect;

		m_GraphicsAPI->SetRenderTargets(rtData);
		m_GraphicsAPI->ForceClear(clearMode, m_data.clearColor, m_data.clearDepthStencil, numRects, pRects);
	}

	// Method for ending render to target.
	void CRenderTexture::EndRender()
	{
		m_GraphicsAPI->ResetRenderTargets();

		for(u8 i = 0; i < m_data.colorBufferCount; ++i)
		{
			m_pRTVTexture[i]->TransitionToShader();
		}

		if(m_data.bUseDepthStencil)
		{
			m_pDSVTexture->TransitionToShader();
		}
	}

	void CRenderTexture::Bind(u32& index, bool bCompute)
	{
		for(u8 i = 0; i < m_data.colorBufferCount; ++i)
		{
			m_pRTVTexture[i]->Bind(index, bCompute);
		}

		if(m_data.bUseDepthStencil)
		{
			m_pDSVTexture->Bind(index, bCompute);
		}
	}

	void CRenderTexture::Release()
	{
		for(u8 i = 0; i < m_data.colorBufferCount; ++i)
		{
			SAFE_RELEASE_DELETE(m_pRTVTexture[i]);
		}

		SAFE_RELEASE_DELETE(m_pDSVTexture);
	}
};
