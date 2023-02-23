//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPostLighting.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPostLighting.h"
#include "CPostSky.h"
#include "CPostEdge.h"
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
	CPostLighting::CPostLighting()
	{
		m_renderTextureCount = 1;
	}

	CPostLighting::~CPostLighting()
	{
	}

	void CPostLighting::Initialize()
	{
		m_pGraphicsAPI = CFactory::Instance().GetGraphicsAPI();
		m_pPostMat = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, "MATERIAL_POST_LIGHTING"));

		m_pRenderTextureDataList = new CRenderTexture::Data[m_renderTextureCount];
		m_pRenderTextureList = new CRenderTexture[m_renderTextureCount];

		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			memset(&m_pRenderTextureDataList[i], 0, sizeof(m_pRenderTextureDataList[i]));

			m_pRenderTextureDataList[i].colorBufferCount = 1;
			m_pRenderTextureDataList[i].width = static_cast<u32>(m_pGraphicsAPI->GetWidth());
			m_pRenderTextureDataList[i].height = static_cast<u32>(m_pGraphicsAPI->GetHeight());
			m_pRenderTextureDataList[i].downScale = 0;
			m_pRenderTextureDataList[i].colorFormat[0] = GFX_FORMAT_R32G32B32A32_FLOAT;
			m_pRenderTextureDataList[i].clearColor[0] = Math::Color(0.0f, 0.0f);
			m_pRenderTextureDataList[i].clearDepthStencil.depth = 1.0f;
			m_pRenderTextureDataList[i].clearDepthStencil.stencil = 0;
			
			m_pRenderTextureDataList[i].colorTexKey[0] = m_pPostProcessor->GetName() + L":POST_LIGHTING_" + std::to_wstring(i);

			m_pRenderTextureList[i].SetData(m_pRenderTextureDataList[i]);
			m_pRenderTextureList[i].Initialize();
		}
	}

	void CPostLighting::PostInitialize()
	{
		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			m_pRenderTextureList[i].PostInitialize();
		}
	}

	const class CRenderTexture** CPostLighting::ApplyEffect(const CRenderTexture** ppTargetTextureList, u32 targetTextureCount,
		const CRenderTexture** ppPostTextureList, u32 postTextureCount, u32& outRenderTextureCount)
	{
		static const u32 dataBufferHash = Math::FNV1a_32("DataBuffer");
		static const u32 projInvHash = Math::FNV1a_32("ProjInv");
		static const u32 viewInvHash = Math::FNV1a_32("ViewInv");

		static const u32 colorHash = Math::FNV1a_32("colorTexture");
		static const u32 normalHash = Math::FNV1a_32("normalTexture");
		static const u32 uiHash = Math::FNV1a_32("uiTexture");
		static const u32 depthHash = Math::FNV1a_32("depthTexture");
		static const u32 skyHash = Math::FNV1a_32("skyTexture");
		static const u32 edgeHash = Math::FNV1a_32("edgeTexture");
		static const u32 ssaoHash = Math::FNV1a_32("ssaoTexture");

		m_pPostMat->SetFloat(dataBufferHash, projInvHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrixInv().f32, 16);
		m_pPostMat->SetFloat(dataBufferHash, viewInvHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrixInv().f32, 16);

		m_pPostMat->SetTexture(colorHash, ppTargetTextureList[0]->GetRTVTexture(0));
		m_pPostMat->SetTexture(normalHash, ppTargetTextureList[0]->GetRTVTexture(1));
		m_pPostMat->SetTexture(uiHash, ppTargetTextureList[0]->GetRTVTexture(3));
		m_pPostMat->SetTexture(depthHash, ppTargetTextureList[0]->GetDSVTexture());
		m_pPostMat->SetTexture(skyHash, m_data.pPostSky->GetRenderTextureList()[0]->GetRTVTexture(0));
		m_pPostMat->SetTexture(edgeHash, m_data.pPostEdge->GetRenderTextureList()[0]->GetRTVTexture(0));
		m_pPostMat->SetTexture(ssaoHash, m_data.pPostSSAO->GetRenderTextureList()[0]->GetRTVTexture(0));

		m_pPostMat->GetShader()->GetRootSignature()->Bind();
		m_pPostMat->Bind();

		m_pRenderTextureList[0].BeginRender(CLEAR_ALL, 0, nullptr);
		m_pGraphicsAPI->RenderEmptyBuffer(PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 3, 1);
		m_pRenderTextureList[0].EndRender();

		outRenderTextureCount = m_renderTextureCount;
		return const_cast<const CRenderTexture**>(&m_pRenderTextureList);
	}

	void CPostLighting::OnResize(const Math::Rect& rect)
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

	void CPostLighting::Release()
	{
		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			m_pRenderTextureList[i].Release();
		}

		SAFE_DELETE_ARRAY(m_pRenderTextureList);
		SAFE_DELETE_ARRAY(m_pRenderTextureDataList);
	}
};
