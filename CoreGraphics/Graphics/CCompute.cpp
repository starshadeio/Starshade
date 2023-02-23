//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CCompute.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CCompute.h"
#include "CMaterial.h"
#include "../Application/CSceneManager.h"

namespace Graphics
{
	CCompute::CCompute(const CVObject* pObject) :
		CVComponent(pObject) {
	}

	CCompute::~CCompute() { }

	void CCompute::Initialize()
	{
		App::CSceneManager::Instance().ComputeSystem().Register(this);
	}

	void CCompute::Dispatch()
	{
		m_data.pMaterial->Bind();
	}

	void CCompute::Release()
	{
		App::CSceneManager::Instance().ComputeSystem().Deregister(this);
	}
};
