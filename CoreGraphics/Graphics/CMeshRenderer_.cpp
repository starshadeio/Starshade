//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshRenderer_.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMeshRenderer_.h"
#include "CMaterial.h"
#include "../Application/CSceneManager.h"

namespace Graphics
{
	CMeshRenderer_::CMeshRenderer_(const CVObject* pObject) :
		CRenderer_(pObject) {
	}

	CMeshRenderer_::~CMeshRenderer_() { }

	void CMeshRenderer_::Initialize()
	{
		if(!m_data.bSkipRegistration)
		{
			CRenderer_::Initialize();
		}
	}

	void CMeshRenderer_::Render()
	{
		if(m_data.onPreRender) { m_data.onPreRender(); }
		m_data.pMaterial->Bind();
	}

	void CMeshRenderer_::Release()
	{
		if(!m_data.bSkipRegistration)
		{
			CRenderer_::Release();
		}
	}
};
