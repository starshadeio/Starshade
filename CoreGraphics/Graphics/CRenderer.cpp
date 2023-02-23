//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRenderer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CRenderer.h"
#include "CRenderingSystem.h"
#include "../Application/CSceneManager.h"
#include <Objects/CNodeObject.h>

namespace Graphics
{
	//-----------------------------------------------------------------------------------------------
	// CMeshData methods.
	//-----------------------------------------------------------------------------------------------
	
	CRenderer::CRenderer(u32 hash) : 
		CNodeComponent(hash)
	{
		App::CSceneManager::Instance().RenderingSystem().Register(this);
	}

	CRenderer::~CRenderer()
	{
	}
};
