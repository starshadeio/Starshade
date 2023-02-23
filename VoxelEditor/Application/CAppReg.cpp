//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CAppReg.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAppReg.h"

#include <Logic/CNodeTransform.h>
#include <Actors/CSpawner.h>
#include <Graphics/CMeshFilter.h>
#include <Graphics/CMeshRenderer.h>

namespace App
{
	CAppReg::CAppReg()
	{
		Logic::CNodeTransform::Get().Register();
		Actor::CSpawner::Get().Register();
		Graphics::CMeshFilter::Get().Register();
		Graphics::CMeshRenderer::Get().Register();
	}

	CAppReg::~CAppReg()
	{
	}
};