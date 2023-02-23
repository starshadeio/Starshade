//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Graphics/CPostSSAO.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPostSSAO.h"
#include "CGraphicsAPI.h"
#include "CRenderTexture.h"
#include "CShader.h"
#include "CMaterial.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include "../Application/CSceneManager.h"
#include <Utilities/CMemoryFree.h>

namespace Graphics
{
	CPostSSAO::CPostSSAO() :
		m_pHBlurMat(nullptr),
		m_pVBlurMat(nullptr)
	{
		m_renderTextureCount = 2;
	}

	CPostSSAO::~CPostSSAO() { }

	void CPostSSAO::Initialize()
	{
		m_pGraphicsAPI = CFactory::Instance().GetGraphicsAPI();
		m_pPostMat = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, "MATERIAL_POST_SSAO"));
		m_pHBlurMat = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, "MATERIAL_POST_HBLUR"));
		m_pVBlurMat = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, "MATERIAL_POST_VBLUR"));

		m_pRenderTextureDataList = new CRenderTexture::Data[m_renderTextureCount];
		m_pRenderTextureList = new CRenderTexture[m_renderTextureCount];

		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			memset(&m_pRenderTextureDataList[i], 0, sizeof(m_pRenderTextureDataList[i]));

			m_pRenderTextureDataList[i].colorBufferCount = 1;
			m_pRenderTextureDataList[i].width = static_cast<u32>(m_pGraphicsAPI->GetWidth());
			m_pRenderTextureDataList[i].height = static_cast<u32>(m_pGraphicsAPI->GetHeight());
			m_pRenderTextureDataList[i].downScale = 0;
			m_pRenderTextureDataList[i].colorFormat[0] = GFX_FORMAT_R32_FLOAT;
			m_pRenderTextureDataList[i].clearColor[0] = Math::Color(0.0f, 0.0f);
			m_pRenderTextureDataList[i].clearDepthStencil.depth = 1.0f;
			m_pRenderTextureDataList[i].clearDepthStencil.stencil = 0;
			
			m_pRenderTextureDataList[i].colorTexKey[0] = m_pPostProcessor->GetName() + L":POST_SSAO_" + std::to_wstring(i);

			m_pRenderTextureList[i].SetData(m_pRenderTextureDataList[i]);
			m_pRenderTextureList[i].Initialize();
		}
	}

	void CPostSSAO::PostInitialize()
	{
		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			m_pRenderTextureList[i].PostInitialize();
		}
	}

	const CRenderTexture** CPostSSAO::ApplyEffect(const CRenderTexture** ppTargetTextureList, u32 targetTextureCount,
		const CRenderTexture** ppPostTextureList, u32 postTextureCount, u32& outRenderTextureCount)
	{
		{ // SSAO.
			static const u32 dataBufferHash = Math::FNV1a_32("DataBuffer");
			static const u32 projInvHash = Math::FNV1a_32("ProjInv");
			static const u32 viewInvHash = Math::FNV1a_32("View");
			static const u32 screenSizeHash = Math::FNV1a_32("ScreenSize");

			static const u32 depthHash = Math::FNV1a_32("depthTexture");
			static const u32 normalHash = Math::FNV1a_32("normalTexture");
		
			m_pPostMat->SetFloat(dataBufferHash, projInvHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrixInv().f32, 16);
			m_pPostMat->SetFloat(dataBufferHash, viewInvHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix().f32, 16);
			Math::Vector4 screenSize(m_pGraphicsAPI->GetWidth(), m_pGraphicsAPI->GetHeight(), 1.0f / m_pGraphicsAPI->GetWidth(), 1.0f / m_pGraphicsAPI->GetHeight());
			m_pPostMat->SetFloat(dataBufferHash, screenSizeHash, &screenSize[0], 4);

			m_pPostMat->SetTexture(depthHash, ppTargetTextureList[0]->GetDSVTexture());
			m_pPostMat->SetTexture(normalHash, ppTargetTextureList[0]->GetRTVTexture(1));

			m_pPostMat->GetShader()->GetRootSignature()->Bind();
			m_pPostMat->Bind();

			m_pRenderTextureList[0].BeginRender(CLEAR_ALL, 0, nullptr);
			m_pGraphicsAPI->RenderEmptyBuffer(PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 3, 1);
			m_pRenderTextureList[0].EndRender();
		}

		{ // Blur.
			static const u32 inputHash = Math::FNV1a_32("inputTexture");

			m_pHBlurMat->SetTexture(inputHash, m_pRenderTextureList[0].GetRTVTexture(0));
			m_pHBlurMat->GetShader()->GetRootSignature()->Bind();
			m_pHBlurMat->Bind();
			
			m_pRenderTextureList[1].BeginRender(CLEAR_ALL, 0, nullptr);
			m_pGraphicsAPI->RenderEmptyBuffer(PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 3, 1);
			m_pRenderTextureList[1].EndRender();
			
			m_pVBlurMat->SetTexture(inputHash, m_pRenderTextureList[1].GetRTVTexture(0));
			m_pVBlurMat->GetShader()->GetRootSignature()->Bind();
			m_pVBlurMat->Bind();
			
			m_pRenderTextureList[0].BeginRender(CLEAR_ALL, 0, nullptr);
			m_pGraphicsAPI->RenderEmptyBuffer(PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 3, 1);
			m_pRenderTextureList[0].EndRender();
		}

		// This effect will be overlayed atop the final image, so don't pass it forward.
		outRenderTextureCount = postTextureCount;
		return ppPostTextureList;
	}

	void CPostSSAO::OnResize(const Math::Rect& rect)
	{
		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			m_pRenderTextureDataList[i].width = static_cast<u32>(rect.w);
			m_pRenderTextureDataList[i].height = static_cast<u32>(rect.h);
			m_pRenderTextureList[i].SetData(m_pRenderTextureDataList[i]);

			m_pRenderTextureList[i].Resize();
			m_pRenderTextureList[i].PostInitialize();
		}
	}

	void CPostSSAO::Release()
	{
		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			m_pRenderTextureList[i].Release();
		}

		SAFE_DELETE_ARRAY(m_pRenderTextureList);
		SAFE_DELETE_ARRAY(m_pRenderTextureDataList);
	}
};
