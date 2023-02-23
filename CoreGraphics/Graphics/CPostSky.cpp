//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPostSky.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPostSky.h"
#include "../Factory/CFactory.h"
#include "CGraphicsAPI.h"
#include "CRenderTexture.h"
#include "CShader.h"
#include "CMaterial.h"
#include "../Resources/CResourceManager.h"
#include "../Application/CSceneManager.h"
#include <Utilities/CMemoryFree.h>

namespace Graphics
{
	CPostSky::CPostSky() : m_skyInterp(0.0f)
	{
		m_renderTextureCount = 1;
	}

	CPostSky::~CPostSky() { }

	void CPostSky::Initialize()
	{
		m_pGraphicsAPI = CFactory::Instance().GetGraphicsAPI();
		m_pPostMat = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, "MATERIAL_POST_SKY"));

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
			
			m_pRenderTextureDataList[i].colorTexKey[0] = m_pPostProcessor->GetName() + L":POST_SKY_" + std::to_wstring(i);

			m_pRenderTextureList[i].SetData(m_pRenderTextureDataList[i]);
			m_pRenderTextureList[i].Initialize();
		}
	}

	void CPostSky::PostInitialize()
	{
		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			m_pRenderTextureList[i].PostInitialize();
		}
	}

	const CRenderTexture** CPostSky::ApplyEffect(const CRenderTexture** ppTargetTextureList, u32 targetTextureCount,
		const CRenderTexture** ppPostTextureList, u32 postTextureCount, u32& outRenderTextureCount)
	{
		static const u32 dataBufferHash = Math::FNV1a_32("DataBuffer");
		static const u32 viewHash = Math::FNV1a_32("View");
		static const u32 aspectHash = Math::FNV1a_32("Aspect");
		static const u32 fovHash = Math::FNV1a_32("Fov");
		static const u32 skyInterpHash = Math::FNV1a_32("SkyInterp");

		static const u32 depthHash = Math::FNV1a_32("depthTexture");

		const float aspect = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetAspectRatio();
		const float fov = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetFOVRadians();

		m_pPostMat->SetFloat(dataBufferHash, viewHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix().f32, 16);
		m_pPostMat->SetFloat(dataBufferHash, aspectHash, &aspect, 1);
		m_pPostMat->SetFloat(dataBufferHash, fovHash, &fov, 1);
		m_pPostMat->SetFloat(dataBufferHash, skyInterpHash, &m_skyInterp, 1);

		m_pPostMat->SetTexture(depthHash, ppTargetTextureList[0]->GetDSVTexture());

		m_pPostMat->GetShader()->GetRootSignature()->Bind();
		m_pPostMat->Bind();

		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			m_pRenderTextureList[i].BeginRender(CLEAR_ALL, 0, nullptr);
			m_pGraphicsAPI->RenderEmptyBuffer(PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 3, 1);
			m_pRenderTextureList[i].EndRender();
		}

		outRenderTextureCount = m_renderTextureCount;
		return const_cast<const CRenderTexture**>(&m_pRenderTextureList);
	}

	void CPostSky::OnResize(const Math::Rect& rect)
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

	void CPostSky::Release()
	{
		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			m_pRenderTextureList[i].Release();
		}

		SAFE_DELETE_ARRAY(m_pRenderTextureList);
		SAFE_DELETE_ARRAY(m_pRenderTextureDataList);
	}
};
