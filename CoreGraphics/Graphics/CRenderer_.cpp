//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRenderer_.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CRenderer_.h"
#include "../Application/CSceneManager.h"

namespace Graphics
{
	CRenderer_::CRenderer_(const CVObject* pObject) :
		CVComponent(pObject),
		m_bActive(true) {
	}

	void CRenderer_::Initialize()
	{
		for(size_t i = 0; i < GetMaterialCount(); ++i)
		{
			App::CSceneManager::Instance().RenderingSystem().Register(this, i);
		}
	}

	void CRenderer_::Release()
	{
		App::CSceneManager::Instance().RenderingSystem().Deregister(this);
	}
};