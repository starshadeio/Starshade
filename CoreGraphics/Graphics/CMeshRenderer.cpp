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

	void CMeshRenderer::Data::Render()
	{
		if(!IsActive()) return;

		for(size_t i = 0; i < m_pMaterialList.size(); ++i)
		{
			RenderWithMaterial(i);
		}
	}
	
	void CMeshRenderer::Data::RenderWithMaterial(size_t materialIndex)
	{
		if(m_onPreRender) m_onPreRender(m_pMaterialList[materialIndex]);
		m_pMaterialList[materialIndex]->GetShader()->GetRootSignature()->Bind();
		m_pMaterialList[materialIndex]->Bind();
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
