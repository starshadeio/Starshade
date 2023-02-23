//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CSceneView.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSceneView.h"
#include "CAppData.h"
#include <Application/CSceneManager.h>
#include <Application/CPlatform.h>
#include <Application/CPanel.h>
#include <Factory/CFactory.h>

namespace App
{
	CSceneView::CSceneView() :
		CScene(SCENE_NAME_VIEW, SceneType::Unloaded),
		m_cyberGrid(L"CyberGrid", VIEW_HASH_MAIN),
		m_cyberNode(L"CyberNode", VIEW_HASH_MAIN) {
	}

	CSceneView::~CSceneView() { }

	void CSceneView::Initialize()
	{
	}

	void CSceneView::PostInitialize() { }

	void CSceneView::Load()
	{
		{ // Debug grid.
			Universe::CCyberGrid::Data data { };
			data.width = 10;
			data.height = 10;
			data.length = 10;
			data.cellSize = 1.0f;
			data.right = Math::VEC3_RIGHT;
			data.up = Math::VEC3_UP;
			data.forward = Math::VEC3_FORWARD;
			m_cyberGrid.SetData(data);
			m_cyberGrid.Initialize();
		}

		{ // Debug node.
			Universe::CCyberNode::Data data { };
			data.size = 1.0f;
			m_cyberNode.SetData(data);
			m_cyberNode.Initialize();
		}
	}

	void CSceneView::Unload()
	{
		m_cyberGrid.Release();
		m_cyberNode.Release();
	}

	void CSceneView::Update() { }
	void CSceneView::LateUpdate() { }

	void CSceneView::Release()
	{
		m_cyberGrid.Release();
		m_cyberNode.Release();
	}

	void CSceneView::OnResize(const Math::Rect& rect) { }
};
