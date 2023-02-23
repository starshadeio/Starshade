//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshRenderer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMeshRenderer.h"
#include "CShader.h"
#include "CRootSignature.h"
#include "CMaterial.h"
#include "../Resources/CResourceManager.h"
#include "../Factory/CFactory.h"
#include <Objects/CNodeObject.h>

#ifdef GRAPHICS_DX12
#include "CDX12MeshRenderer.h"
#endif
#ifdef GRAPHICS_VULKAN
#include "CVKMeshRenderer.h"
#endif

namespace Graphics
{
	//-----------------------------------------------------------------------------------------------
	// CMeshData::Data
	//-----------------------------------------------------------------------------------------------

	void CMeshRenderer::Data::Initialize()
	{
		m_pMaterial = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_matHash));
	}

	void CMeshRenderer::Data::Render()
	{
		if(m_onPreRender) m_onPreRender(m_pMaterial);
		m_pMaterial->GetShader()->GetRootSignature()->Bind();
		m_pMaterial->Bind();
	}

	//-----------------------------------------------------------------------------------------------
	// CMeshData methods.
	//-----------------------------------------------------------------------------------------------
	
	CMeshRenderer::CMeshRenderer(u32 hash) : 
		CRenderer(hash)
	{
	}

	CMeshRenderer::~CMeshRenderer()
	{
	}

	CMeshRenderer& CMeshRenderer::Get()
	{
#ifdef GRAPHICS_DX12
		static CDX12MeshRenderer comp;
#endif
#ifdef GRAPHICS_VULKAN
		static CVKMeshRenderer comp;
#endif
		return comp;
	}
};
