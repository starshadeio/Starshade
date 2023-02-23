//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CEditor.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CEditor.h"
#include "CAppData.h"
#include "../Resources/CAssetManager.h"
#include <Application/CSceneManager.h>
#include <Application/CCommandManager.h>
#include <Physics/CPhysics.h>

namespace App
{
	CEditor::CEditor()
	{
	}

	CEditor::~CEditor()
	{

	}
	
	void CEditor::Initialize()
	{
		{
			m_editorPanel.Initialize();
		}

		{ // Setup app state.
			m_state.Initialize();
			m_state.Reset();
		}

		m_projectManager.Initialize();
		m_nodeEditor.Initialize();

		{ // Setup commands.
			CCommandManager::Instance().Initialize();
			
			CCommandManager::Instance().RegisterAction(CAppState::CMD_KEY_PLAY_SPAWN, std::bind(&CEditor::OnCmdPlay, this, true));
			CCommandManager::Instance().RegisterAction(CAppState::CMD_KEY_PLAY_AT, std::bind(&CEditor::OnCmdPlay, this, false));
			CCommandManager::Instance().RegisterAction(CAppState::CMD_KEY_EDIT, std::bind(&CEditor::OnCmdEdit, this));

			CCommandManager::Instance().RegisterAction(CProjectManager::CMD_KEY_NEW_PROJECT, std::bind(&CEditor::OnCmdNew, this));
			CCommandManager::Instance().RegisterAction(CProjectManager::CMD_KEY_OPEN_PROJECT, std::bind(&CEditor::OnCmdLoad, this));
			CCommandManager::Instance().RegisterAction(CProjectManager::CMD_KEY_SAVE_PROJECT, std::bind(&CEditor::OnCmdSave, this));
			CCommandManager::Instance().RegisterAction(CProjectManager::CMD_KEY_SAVE_PROJECT_AS, std::bind(&CEditor::OnCmdSave, this));
			CCommandManager::Instance().RegisterAction(CProjectManager::CMD_KEY_BUILD_PROJECT, std::bind(&CEditor::OnCmdBuild, this));
		}

		{ // Create a physic updater. This is called from the physics thread.
			Physics::CPhysics::Instance().CreatePhysicsUpdate(&m_physicsUpdate, nullptr);
			Physics::CPhysicsUpdate::Data data { };
			data.updateFunc = std::bind(&CEditor::PhysicsUpdate, this);
			m_physicsUpdate.pRef->SetData(data);
		}
	}

	void CEditor::PostInitialize()
	{
		m_nodeEditor.PostInitialize();
		CCommandManager::Instance().ProcessActionRegistrationQueue();

		OnCmdEdit();
	}

	void CEditor::Update()
	{
		m_nodeEditor.Update();
	}

	void CEditor::LateUpdate()
	{
		m_editorPanel.LateUpdate();
		m_nodeEditor.LateUpdate();
	}

	void CEditor::PhysicsUpdate()
	{
		m_nodeEditor.PhysicsUpdate();
	}

	bool CEditor::OnQuit(int exitCode)
	{
		return m_projectManager.ShouldQuit();
	}

	void CEditor::Release()
	{
		Physics::CPhysics::Instance().DestroyPhysicsUpdate(&m_physicsUpdate);

		m_nodeEditor.Release();
		m_editorPanel.Release();
	}
	
	void CEditor::OnResize(const Math::Rect& rect)
	{
		m_nodeEditor.OnResize(rect);
		m_editorPanel.OnResize(rect);
	}

	//-----------------------------------------------------------------------------------------------
	// Command methods.
	//-----------------------------------------------------------------------------------------------

	void CEditor::OnCmdPlay(bool bSpawn)
	{
		m_editorPanel.OnPlay();
		m_nodeEditor.OnPlay(bSpawn);
	}

	void CEditor::OnCmdEdit()
	{
		m_editorPanel.OnEdit();
		m_nodeEditor.OnEdit();
	}

	void CEditor::OnCmdNew()
	{
		m_nodeEditor.New();
	}
	
	void CEditor::OnCmdSave()
	{
		m_nodeEditor.Save();
	}

	void CEditor::OnCmdLoad()
	{
		m_nodeEditor.Load();
	}

	void CEditor::OnCmdBuild()
	{
		m_nodeEditor.Build();
		Resources::CAssets::Instance().Save(m_projectManager.GetBuildResourcePath());
	}
};
