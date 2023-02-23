//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: UI/CEditorPanel.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CEditorPanel.h"
#include "../Application/CAppData.h"
#include "../Application/CAppState.h"
#include <Application/CSceneManager.h>
#include <Application/CCommandManager.h>
#include <Application/CView.h>
#include <Graphics/CPostProcessor.h>
#include <Graphics/CPostEffect.h>
#include <Resources/CResourceManager.h>

namespace UI
{
	CEditorPanel::CEditorPanel() :
		CVObject(L"Editor Panel"),
		m_canvasSceneView(this),
		m_canvasEditor(this),
		m_paneSceneView(this, &m_canvasSceneView),
		m_paneEditor(this, &m_canvasEditor)
	{
	}

	CEditorPanel::~CEditorPanel()
	{
	}
	
	void CEditorPanel::Initialize()
	{
		{ // Setup canvas.
			UI::CUICanvas::Data data { };
			
			data.depth = -1;
			data.frameData.position = 0.0f;
			data.frameData.size = 100.0f;
			data.frameData.anchorMax = 0.0f;
			data.frameData.anchorMin = 0.0f;
			data.frameData.pivot = 0.0f;
			data.frameData.rotation = 0.0f;
			data.frameData.scale = 1.0f;
			data.frameData.pParent = nullptr;
			m_canvasSceneView.SetData(data);
			m_canvasSceneView.SetActive(true);
			
			data.depth = 0;
			m_canvasEditor.SetData(data);
			m_canvasEditor.SetActive(true);
		}

		{ // Setup panel.
			CPane::Data data { };
			data.filename = *reinterpret_cast<std::wstring*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_SCRIPT, Math::FNV1a_64(L"SCRIPT_PANE_SCENE_VIEW")));
			m_paneSceneView.SetData(data);
			m_paneSceneView.Initialize();

			data.filename = *reinterpret_cast<std::wstring*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_SCRIPT, Math::FNV1a_64(L"SCRIPT_PANE_EDITOR")));
			m_paneEditor.SetData(data);
			m_paneEditor.Initialize();
		}
		
		m_canvasSceneView.Initialize();
		m_canvasEditor.Initialize();

		return;
	}
	
	void CEditorPanel::LateUpdate()
	{
		m_paneSceneView.DebugUpdate();
		m_paneEditor.DebugUpdate();
	}
	
	void CEditorPanel::Release()
	{
		m_paneEditor.Release();
		m_paneSceneView.Release();
		m_canvasEditor.Release();
		m_canvasSceneView.Release();
	}
	
	void CEditorPanel::OnResize(const Math::Rect& rect)
	{
		m_canvasSceneView.OnResize(rect);
		m_canvasEditor.OnResize(rect);
	}

	//-----------------------------------------------------------------------------------------------
	// Action methods.
	//-----------------------------------------------------------------------------------------------

	void CEditorPanel::OnPlay()
	{
		m_canvasEditor.SetActive(false);
	}

	void CEditorPanel::OnEdit()
	{
		m_canvasEditor.SetActive(true);
	}
};
