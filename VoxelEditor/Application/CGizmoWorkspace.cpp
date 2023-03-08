//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoWorkspace.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmoWorkspace.h"
#include "CGizmoPivot.h"
#include "CEditor.h"
#include "../Universe/CChunkWorkspace.h"
#include <Application/CCommandManager.h>
#include <Graphics/CGraphicsAPI.h>
#include <Factory/CFactory.h>
#include <Application/CSceneManager.h>
#include <unordered_map>

namespace App
{
	CGizmoWorkspace::CGizmoWorkspace()
	{
	}

	CGizmoWorkspace::~CGizmoWorkspace()
	{
	}
	
	void CGizmoWorkspace::Initialize()
	{
		{ // Setup pivot.
			CGizmoPivot_::Callbacks callback { };

			callback.getPosition = std::bind(&CGizmoWorkspace::GetPosition, this);
			callback.getRotation = std::bind(&CGizmoWorkspace::GetRotation, this);
			callback.getScale = std::bind(&CGizmoWorkspace::GetScale, this);

			callback.getRight = std::bind(&CGizmoWorkspace::GetRight, this);
			callback.getUp = std::bind(&CGizmoWorkspace::GetUp, this);
			callback.getForward = std::bind(&CGizmoWorkspace::GetForward, this);
			
			callback.setPosition = std::bind(&CGizmoWorkspace::SetPosition, this, std::placeholders::_1);
			callback.setRotation = std::bind(&CGizmoWorkspace::SetRotation, this, std::placeholders::_1);
			callback.setScale = std::bind(&CGizmoWorkspace::SetScale, this, std::placeholders::_1);

			m_gizmoPivot.SetCallbacks(callback);
		}

		{ // Setup chunk grid gizmo.
			CGizmoChunkGrid::Data data { };
			data.pGizmoPivot = &m_gizmoPivot;
			m_gizmoChunkGrid.SetData(data);
			m_gizmoChunkGrid.Initialize();
		}

		{ // Setup commands.
			App::CCommand::Properties props = { };
			props.bSaveEnabled = true;
			props.bUndoEnabled = true;

			CCommandManager::Instance().RegisterCommand(CMD_KEY_TRANSLATE, std::bind(&CGizmoWorkspace::Translate, this, std::placeholders::_1, std::placeholders::_2), props);
			CCommandManager::Instance().RegisterCommand(CMD_KEY_EXTENTS, std::bind(&CGizmoWorkspace::Extents, this, std::placeholders::_1, std::placeholders::_2), props);
		}
	}

	void CGizmoWorkspace::Update()
	{
		m_gizmoPivot.Update();

		switch(CEditor::Instance().Nodes().Gizmo().GetActiveType())
		{
			case CGizmo::GizmoType::Workspace:
				m_gizmoChunkGrid.Interact();
				break;
			default:
				break;
		}
	}

	void CGizmoWorkspace::LateUpdate()
	{
		m_gizmoPivot.LateUpdate();

		switch(CEditor::Instance().Nodes().Gizmo().GetActiveType())
		{
			case CGizmo::GizmoType::Workspace:
				m_gizmoChunkGrid.Draw();
				break;
			default:
				break;
		}
	}

	void CGizmoWorkspace::Release()
	{
	}

	//-----------------------------------------------------------------------------------------------
	// Command methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CGizmoWorkspace::Translate(const void* param, bool bInverse)
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
	
	bool CGizmoWorkspace::Extents(const void* param, bool bInverse)
	{
		std::pair<Math::Vector3, Math::Vector3> extents = *(std::pair<Math::Vector3, Math::Vector3>*)param;
		if(extents.first == m_gizmoChunkGrid.GetLastMinExtents() && extents.second == m_gizmoChunkGrid.GetLastMaxExtents()) return false;
		
		m_pChunkGrid->SetExtents(extents.first, extents.second);

		if(!bInverse)
		{
			std::pair<Math::Vector3, Math::Vector3> lastExtents = { m_gizmoChunkGrid.GetLastMinExtents(), m_gizmoChunkGrid.GetLastMaxExtents() };
			App::CCommandManager::Instance().RecordUndo(&lastExtents, sizeof(lastExtents), alignof(Math::Vector3));
		}

		m_gizmoChunkGrid.GatherExtents();

		return true;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Command methods.
	//-----------------------------------------------------------------------------------------------
	
	const Math::SIMDVector& CGizmoWorkspace::GetPosition()
	{
		return m_pChunkGrid->GetOffset();
	}

	const Math::SIMDQuaternion& CGizmoWorkspace::GetRotation()
	{
		return m_pChunkGrid->GetTransform()->GetRotation();
	}

	const Math::SIMDVector& CGizmoWorkspace::GetScale()
	{
		return m_pChunkGrid->GetTransform()->GetLocalScale();
	}

	Math::SIMDVector CGizmoWorkspace::GetRight()
	{
		return m_pChunkGrid->GetTransform()->GetRight();
	}

	Math::SIMDVector CGizmoWorkspace::GetUp()
	{
		return m_pChunkGrid->GetTransform()->GetUp();
	}

	Math::SIMDVector CGizmoWorkspace::GetForward()
	{
		return m_pChunkGrid->GetTransform()->GetForward();
	}

	void CGizmoWorkspace::SetPosition(const Math::SIMDVector& position)
	{
		m_pChunkGrid->SetOffset(*(Math::Vector3*)position.ToFloat());
	}

	void CGizmoWorkspace::SetRotation(const Math::SIMDQuaternion& rotation)
	{
		m_pChunkGrid->GetTransform()->SetRotation(rotation);
	}

	void CGizmoWorkspace::SetScale(const Math::SIMDVector& scale)
	{
		m_pChunkGrid->GetTransform()->SetScale(scale);
	}
};
