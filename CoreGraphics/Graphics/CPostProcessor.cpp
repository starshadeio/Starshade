//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPostProcessor.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPostProcessor.h"
#include "CPostEffect.h"
#include "CGraphicsAPI.h"
#include "CMaterial.h"
#include "CShader.h"
#include "CRootSignature.h"
#include "../Application/CPanel.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include <Math/CMathFNV.h>

namespace Graphics
{
	CPostProcessor::CPostProcessor(const wchar_t* pName) :
		m_name(pName),
		m_pPostMat(nullptr),
		m_pGraphicsAPI(nullptr) {
	}

	CPostProcessor::~CPostProcessor() { }

	void CPostProcessor::Initialize()
	{
		m_pGraphicsAPI = CFactory::Instance().GetGraphicsAPI();
		m_pPostMat = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, "MATERIAL_POST"));

		{ // Setup post render texture.
			m_renderTexture.SetData(m_renderTextureData);
			m_renderTexture.Initialize();
		}
	}

	void CPostProcessor::PostInitialize()
	{
		m_renderTexture.PostInitialize();
	}

	void CPostProcessor::BeginRenderTo(CLEAR_MODE clearMode, u32 numRects, const Math::RectLTRB* pRects)
	{
		m_renderTexture.BeginRender(clearMode, numRects, pRects);
	}

	void CPostProcessor::EndRenderTo()
	{
		m_renderTexture.EndRender();
		PostProcess();
	}

	void CPostProcessor::PostProcess()
	{
		const CRenderTexture* ppInputRenderTextureList[] = { &m_renderTexture };
		u32 renderTextureCount = m_renderTextureData.colorBufferCount;
		const CRenderTexture** ppRenderTextureList = { ppInputRenderTextureList };

		for(size_t i = 0; i < m_postEffectList.size(); ++i)
		{
			ppRenderTextureList = m_postEffectList[i]->ApplyEffect(ppInputRenderTextureList, 1, ppRenderTextureList, renderTextureCount, renderTextureCount);
		}

		assert(renderTextureCount == 1);
		static const u32 textureHash = Math::FNV1a_32("inputTexture_1");
		m_pPostMat->SetTexture(textureHash, ppRenderTextureList[0]->GetRTVTexture(0));
	}
	
	void CPostProcessor::RenderQuad()
	{
		m_pPostMat->GetShader()->GetRootSignature()->Bind();
		m_pPostMat->Bind();
		m_pGraphicsAPI->RenderEmptyBuffer(PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 3, 1);
	}

	void CPostProcessor::OnResize(const Math::Rect& rect)
	{
		const u32 width = static_cast<u32>(rect.w);
		const u32 height = static_cast<u32>(rect.h);

		if(width == m_renderTextureData.width && height == m_renderTextureData.height)
		{
			return;
		}

		m_renderTextureData.width = width;
		m_renderTextureData.height = height;
		m_renderTexture.SetData(m_renderTextureData);

		m_renderTexture.Resize();
		m_renderTexture.PostInitialize();
	}

	void CPostProcessor::Release()
	{
		m_renderTexture.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CPostProcessor::RegisterPostEffect(CPostEffect* pPostEffect)
	{
		pPostEffect->SetPostProcessor(this);
		m_postEffectList.push_back(pPostEffect);
	}
};
