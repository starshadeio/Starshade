//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPost.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPost.h"
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
	CPost::CPost() :
		m_postProcessorCount(0),
		m_postProcessorList{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr },
		m_pPostMat(nullptr),
		m_pGraphicsAPI(nullptr)
	{
	}

	CPost::~CPost()
	{
	}

	void CPost::PostInitialize()
	{
		m_pGraphicsAPI = CFactory::Instance().GetGraphicsAPI();
		m_pPostMat = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, "MATERIAL_POST"));
	}

	void CPost::RenderQuad()
	{
		for(size_t i = 0; i < m_postProcessorCount; ++i)
		{

		}

		m_pPostMat->GetShader()->GetRootSignature()->Bind();
		m_pPostMat->Bind();
		m_pGraphicsAPI->RenderEmptyBuffer(PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 3, 1);
	}
};
