//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CSceneNode.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSceneNode.h"
#include "CAppData.h"
#include <Application/CSceneManager.h>
#include <Application/CPlatform.h>
#include <Application/CPanel.h>
#include <Factory/CFactory.h>

namespace App
{
	CSceneNode::CSceneNode() :
		CScene(SCENE_NAME_NODE, SceneType::Default)
	{
		{ // Setup view overlay.
			CViewOverlay::Data data { };
			data.pViewScene = &m_viewScene;
			m_viewOverlay.SetData(data);
		}
	}

	CSceneNode::~CSceneNode() { }

	void CSceneNode::Initialize()
	{
	}

	void CSceneNode::PostInitialize() { }

	void CSceneNode::Load()
	{
		CSceneManager::Instance().LoadView(m_viewOverlay.GetHash());
		CSceneManager::Instance().LoadView(m_viewScene.GetHash());
	}

	void CSceneNode::Unload()
	{
		CSceneManager::Instance().UnloadView(m_viewScene.GetHash());
		CSceneManager::Instance().UnloadView(m_viewOverlay.GetHash());
	}
	
	//------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------

	void CSceneNode::Update() { }
	void CSceneNode::LateUpdate()
	{
	}

	void CSceneNode::Release() 
	{
	}

	void CSceneNode::OnResize(const Math::Rect& rect)
	{
		m_viewOverlay.OnResize(rect);
		m_viewScene.OnResize(rect);
	}
};
