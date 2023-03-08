//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmo.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmo.h"
#include "CEditor.h"
#include "../Universe/CChunkWorkspace.h"
#include <Application/CCommandManager.h>

namespace App
{
	CGizmo::CGizmo() : 
		m_bActive(true)
	{
	}

	CGizmo::~CGizmo()
	{
	}

	void CGizmo::Initialize()
	{
		m_gizmoNode.Initialize();
		m_gizmoWorkspace.Initialize();
		m_gizmoInteract.Initialize();
		m_gizmoDraw.Initialize();
		
		{ // Setup commands.
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_SELECT, std::bind(&CGizmo::GizmoSelect, this));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_TRANSLATE, std::bind(&CGizmo::GizmoTranslate, this));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_ROTATE, std::bind(&CGizmo::GizmoRotate, this));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_SCALE, std::bind(&CGizmo::GizmoScale, this));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_WORKSPACE, std::bind(&CGizmo::GizmoWorkspace, this));
			
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_SELECT_OFF, [](const void* params, bool bInverse, size_t sz, u16 align){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_TRANSLATE_OFF, [](const void* params, bool bInverse, size_t sz, u16 align){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_ROTATE_OFF, [](const void* params, bool bInverse, size_t sz, u16 align){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_SCALE_OFF, [](const void* params, bool bInverse, size_t sz, u16 align){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_WORKSPACE_OFF, [](const void* params, bool bInverse, size_t sz, u16 align){ return true; });

			CCommandManager::Instance().RegisterAction(Universe::CChunkWorkspace::CMD_KEY_WORKSPACE_OFF, std::bind(&CGizmo::OnCmdWorkspaceOff, this));
		}
	}

	void CGizmo::Update()
	{
		if(!m_bActive) { return; }

		m_gizmoNode.Update();
		m_gizmoWorkspace.Update();

		// Interaction update should always come after all other gizmo updates.
		m_gizmoInteract.Update();
	}

	void CGizmo::LateUpdate()
	{
		if(!m_bActive) { return; }

		m_gizmoNode.LateUpdate();
		m_gizmoWorkspace.LateUpdate();
	}

	void CGizmo::Release()
	{
		m_gizmoDraw.Release();
		m_gizmoWorkspace.Release();
	}
	
	//-----------------------------------------------------------------------------------------------
	// Command methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CGizmo::GizmoSelect()
	{
		SwitchGizmo(GizmoType::Select);
		return true;
	}
	
	bool CGizmo::GizmoTranslate()
	{
		SwitchGizmo(GizmoType::Translate);
		return true;
	}

	bool CGizmo::GizmoRotate()
	{
		SwitchGizmo(GizmoType::Rotate);
		return true;
	}

	bool CGizmo::GizmoScale()
	{
		SwitchGizmo(GizmoType::Scale);
		return true;
	}

	bool CGizmo::GizmoWorkspace()
	{
		if(!CEditor::Instance().Nodes().GetChunkEditor().GetWorkspace().IsActive()) return false;
		SwitchGizmo(GizmoType::Workspace);
		return true;
	}

	void CGizmo::OnCmdWorkspaceOff()
	{
		if(m_gizmoType == GizmoType::Workspace)
		{
			App::CCommandManager::Instance().Execute(CMD_KEY_GIZMO_SELECT);
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CGizmo::SwitchGizmo(GizmoType next)
	{
		switch(m_gizmoType)
		{
			case GizmoType::Select:
				if(next != GizmoType::Select)
					App::CCommandManager::Instance().Execute(CMD_KEY_GIZMO_SELECT_OFF);
				break;
			case GizmoType::Translate:
				if(next != GizmoType::Translate)
					App::CCommandManager::Instance().Execute(CMD_KEY_GIZMO_TRANSLATE_OFF);
				break;
			case GizmoType::Rotate:
				if(next != GizmoType::Rotate)
					App::CCommandManager::Instance().Execute(CMD_KEY_GIZMO_ROTATE_OFF);
				break;
			case GizmoType::Scale:
				if(next != GizmoType::Scale)
					App::CCommandManager::Instance().Execute(CMD_KEY_GIZMO_SCALE_OFF);
				break;
			case GizmoType::Workspace:
				if(next != GizmoType::Workspace)
					App::CCommandManager::Instance().Execute(CMD_KEY_GIZMO_WORKSPACE_OFF);
				break;
			default:
				break;
		}

		m_gizmoType = next;
	}

};
