//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoNode.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmoNode.h"
#include "CAppData.h"
#include "CEditor.h"
#include <Application/CCommandManager.h>
#include <Application/CSceneManager.h>
#include <Factory/CFactory.h>
#include <Graphics/CGraphicsAPI.h>

namespace App
{
	CGizmoNode::CGizmoNode()
	{
	}

	CGizmoNode::~CGizmoNode()
	{
	}

	void CGizmoNode::Initialize()
	{
		{ // Setup translation gizmo.
			CGizmoTranslate::Data data { };
			data.pGizmoPivot = &m_gizmoPivot;
			data.onReleaseHash = CMD_KEY_TRANSLATE;
			m_gizmoTranslate.SetData(data);
		}
		
		{ // Setup rotate gizmo.
			CGizmoRotate::Data data { };
			data.pGizmoPivot = &m_gizmoPivot;
			data.onReleaseHash = CMD_KEY_ROTATE;
			m_gizmoRotate.SetData(data);
		}
		
		{ // Setup scale gizmo.
			CGizmoScale::Data data { };
			data.pGizmoPivot = &m_gizmoPivot;
			data.onReleaseHash = CMD_KEY_SCALE;
			m_gizmoScale.SetData(data);
		}

		{ // Setup commands.
			App::CCommand::Properties props = { };
			props.bSaveEnabled = true;
			props.bUndoEnabled = true;

			CCommandManager::Instance().RegisterCommand(CMD_KEY_TRANSLATE, std::bind(&CGizmoNode::Translate, this, std::placeholders::_1, std::placeholders::_2), props);
			CCommandManager::Instance().RegisterCommand(CMD_KEY_ROTATE, std::bind(&CGizmoNode::Rotate, this, std::placeholders::_1, std::placeholders::_2), props);
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SCALE, std::bind(&CGizmoNode::Scale, this, std::placeholders::_1, std::placeholders::_2), props);
		}
	}

	void CGizmoNode::Update()
	{
		m_gizmoPivot.Update();

		switch(CEditor::Instance().Nodes().Gizmo().GetActiveType())
		{
			case CGizmo::GizmoType::Translate:
				m_gizmoTranslate.Interact();
				break;
			case CGizmo::GizmoType::Rotate:
				m_gizmoRotate.Interact();
				break;
			case CGizmo::GizmoType::Scale:
				m_gizmoScale.Interact();
				break;
			default:
				break;
		}
	}

	void CGizmoNode::LateUpdate()
	{
		m_gizmoPivot.LateUpdate();

		switch(CEditor::Instance().Nodes().Gizmo().GetActiveType())
		{
			case CGizmo::GizmoType::Translate:
				m_gizmoTranslate.Draw();
				break;
			case CGizmo::GizmoType::Rotate:
				m_gizmoRotate.Draw();
				break;
			case CGizmo::GizmoType::Scale:
				m_gizmoScale.Draw();
				break;
			default:
				break;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Command methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CGizmoNode::Translate(const void* param, bool bInverse)
	{
		Math::SIMDVector position = *(Math::SIMDVector*)param;
		if(position == m_gizmoPivot.GetLastPosition()) return false;

		if(!bInverse)
		{
			auto lastPosition = m_gizmoPivot.GetLastPosition();
			App::CCommandManager::Instance().RecordUndo(&lastPosition, sizeof(lastPosition), alignof(Math::SIMDVector));
		}

		m_gizmoPivot.SetPosition(position, true);
		return true;
	}

	bool CGizmoNode::Rotate(const void* param, bool bInverse)
	{
		Math::SIMDQuaternion rotation = *(Math::SIMDQuaternion*)param;
		if(rotation == m_gizmoPivot.GetLastRotation()) return false;

		if(!bInverse)
		{
			auto lastRotation = m_gizmoPivot.GetLastRotation();
			App::CCommandManager::Instance().RecordUndo(&lastRotation, sizeof(lastRotation), alignof(Math::SIMDQuaternion));
		}
		
		m_gizmoPivot.SetRotation(rotation, true);
		return true;
	}

	bool CGizmoNode::Scale(const void* param, bool bInverse)
	{
		Math::SIMDVector scale = *(Math::SIMDVector*)param;
		if(scale == m_gizmoPivot.GetLastScale()) return false;

		if(!bInverse)
		{
			auto lastScale = m_gizmoPivot.GetLastScale();
			App::CCommandManager::Instance().RecordUndo(&lastScale, sizeof(lastScale), alignof(Math::SIMDVector));
		}
		
		m_gizmoPivot.SetScale(scale, true);
		return true;
	}
};
