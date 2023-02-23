//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Graphics/CPostEdge.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPostEdge.h"
#include "CPostProcessor.h"
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
	CPostEdge::CPostEdge()
	{
		m_renderTextureCount = 1;
	}

	CPostEdge::~CPostEdge() { }

	void CPostEdge::Initialize()
	{
		m_pGraphicsAPI = CFactory::Instance().GetGraphicsAPI();
		m_pPostMat = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, "MATERIAL_POST_EDGE"));

		m_pRenderTextureDataList = new CRenderTexture::Data[m_renderTextureCount];
		m_pRenderTextureList = new CRenderTexture[m_renderTextureCount];

		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			memset(&m_pRenderTextureDataList[i], 0, sizeof(m_pRenderTextureDataList[i]));

			m_pRenderTextureDataList[i].colorBufferCount = 1;
			m_pRenderTextureDataList[i].width = static_cast<u32>(m_pGraphicsAPI->GetWidth());
			m_pRenderTextureDataList[i].height = static_cast<u32>(m_pGraphicsAPI->GetHeight());
			m_pRenderTextureDataList[i].downScale = 0;
			m_pRenderTextureDataList[i].colorFormat[0] = GFX_FORMAT_R8_UNORM;
			m_pRenderTextureDataList[i].clearColor[0] = Math::Color(0.0f, 0.0f);
			m_pRenderTextureDataList[i].clearDepthStencil.depth = 1.0f;
			m_pRenderTextureDataList[i].clearDepthStencil.stencil = 0;

			m_pRenderTextureDataList[i].colorTexKey[0] = m_pPostProcessor->GetName() + L":POST_EDGE_" + std::to_wstring(i);

			m_pRenderTextureList[i].SetData(m_pRenderTextureDataList[i]);
			m_pRenderTextureList[i].Initialize();
		}
	}

	void CPostEdge::PostInitialize()
	{
		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			m_pRenderTextureList[i].PostInitialize();
		}
	}

	const CRenderTexture** CPostEdge::ApplyEffect(const CRenderTexture** ppTargetTextureList, u32 targetTextureCount,
		const CRenderTexture** ppPostTextureList, u32 postTextureCount, u32& outRenderTextureCount)
	{
		static const u32 dataBufferHash = Math::FNV1a_32("DataBuffer");
		static const u32 projInvHash = Math::FNV1a_32("ProjInv");

		static const u32 depthHash = Math::FNV1a_32("depthTexture");
		
		m_pPostMat->SetFloat(dataBufferHash, projInvHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrixInv().rows[3].m128_f32, 4);
		m_pPostMat->SetTexture(depthHash, ppTargetTextureList[0]->GetDSVTexture());

		m_pPostMat->GetShader()->GetRootSignature()->Bind();
		m_pPostMat->Bind();

		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			m_pRenderTextureList[i].BeginRender(CLEAR_ALL, 0, nullptr);
			m_pGraphicsAPI->RenderEmptyBuffer(PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 3, 1);
			m_pRenderTextureList[i].EndRender();
		}

		// This effect will be overlayed atop the final image, so don't pass it forward.
		outRenderTextureCount = postTextureCount;
		return ppPostTextureList;
	}

	void CPostEdge::OnResize(const Math::Rect& rect)
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

	void CPostEdge::Release()
	{
		for(u32 i = 0; i < m_renderTextureCount; ++i)
		{
			m_pRenderTextureList[i].Release();
		}

		SAFE_DELETE_ARRAY(m_pRenderTextureList);
		SAFE_DELETE_ARRAY(m_pRenderTextureDataList);
	}
};
