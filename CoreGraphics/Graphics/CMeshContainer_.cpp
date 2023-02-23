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
#include "CMeshRenderer_.h"
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
		App::CSceneManager::Instance().RenderingSystem().Register(this);
	}

	void CMeshContainer_::Render()
	{
		if(m_data.onPreRender) { m_data.onPreRender(); }
		if(m_data.pMeshRenderer)
		{
			m_data.pMeshRenderer->Render();
		}
	}

	void CMeshContainer_::Release()
	{
		App::CSceneManager::Instance().RenderingSystem().Deregister(this);
	}
};