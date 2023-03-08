//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshContainer_.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMeshContainer_.h"
#include "CMaterial.h"
#include "CShader.h"
#include "CRootSignature.h"
#include "../Application/CSceneManager.h"

namespace Graphics
{
	CMeshContainer_::CMeshContainer_(const CVObject* pObject) :
		CRenderer_(pObject) {
	}

	void CMeshContainer_::Initialize()
	{
		if(!m_data.bSkipRegistration)
		{
			for(size_t i = 0; i < GetMaterialCount(); ++i)
			{
				App::CSceneManager::Instance().RenderingSystem().Register(this, i);
			}
		}
	}

	void CMeshContainer_::Render()
	{
		for(size_t i = 0; i < m_pMaterialList.size(); ++i)
		{
			RenderWithMaterial(i);
		}
	}

	void CMeshContainer_::RenderWithMaterial(size_t materialIndex)
	{
		if(m_data.onPreRender) { m_data.onPreRender(m_pMaterialList[materialIndex]); }
		if(m_data.pMeshRenderer)
		{
			m_data.pMeshRenderer->RenderWithMaterial(materialIndex);
		}
	}

	void CMeshContainer_::Release()
	{
		if(!m_data.bSkipRegistration)
		{
			App::CSceneManager::Instance().RenderingSystem().Deregister(this);
		}
	}
};