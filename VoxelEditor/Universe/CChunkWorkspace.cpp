//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CChunkWorkspace.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CChunkWorkspace.h"
#include "../Application/CGizmo.h"
#include <Application/CSceneManager.h>
#include <Application/CCommandManager.h>

namespace Universe
{
	CChunkWorkspace::CChunkWorkspace() : 
		m_bActive(true)
	{
	}

	CChunkWorkspace::~CChunkWorkspace()
	{
	}

	void CChunkWorkspace::Initialize()
	{
		{ // Setup grid.
			m_grid.Initialize();
		}

		{ // Setup commands.
			App::CCommand::Properties props = { };
			props.bSaveEnabled = true;
			props.bUndoEnabled = true;
			
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_WORKSPACE_TOGGLE, std::bind(&CChunkWorkspace::WorkspaceToggle, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_WORKSPACE_OFF, std::bind(&CChunkWorkspace::WorkspaceOff, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_WORKSPACE_ON, std::bind(&CChunkWorkspace::WorkspaceOn, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_WORKSPACE_RESET, std::bind(&CChunkWorkspace::WorkspaceReset, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_WORKSPACE_SET, std::bind(&CChunkWorkspace::WorkspaceSet, this, std::placeholders::_1, std::placeholders::_2), props);

			App::CCommandManager::Instance().RegisterAction(App::CGizmo::CMD_KEY_GIZMO_WORKSPACE, std::bind(&CChunkWorkspace::OnCmdGizmoWorkspace, this));
			App::CCommandManager::Instance().RegisterAction(App::CGizmo::CMD_KEY_GIZMO_WORKSPACE_OFF, std::bind(&CChunkWorkspace::OnCmdGizmoWorkspaceOff, this));
		}
	}

	void CChunkWorkspace::PostInitialize()
	{
		m_grid.PostInitialize();
	}

	void CChunkWorkspace::Update()
	{
	}

	void CChunkWorkspace::LateUpdate()
	{
	}

	void CChunkWorkspace::Release()
	{
		m_grid.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// File methods.
	//-----------------------------------------------------------------------------------------------

	void CChunkWorkspace::Save(std::ofstream& file) const
	{
		Math::Vector3 scale = m_grid.GetScale();
		Math::Vector3 offset = *(Math::Vector3*)m_grid.GetOffset().ToFloat();

		file.write(reinterpret_cast<char*>(&scale), sizeof(scale));
		file.write(reinterpret_cast<char*>(&offset), sizeof(offset));
	}

	void CChunkWorkspace::Load(std::ifstream& file)
	{
		Math::Vector3 scale;
		Math::Vector3 offset;
		file.read(reinterpret_cast<char*>(&scale), sizeof(scale));
		file.read(reinterpret_cast<char*>(&offset), sizeof(offset));
		scale *= 0.5f;
		m_grid.SetExtents(offset - scale, offset + scale);
	}

	//-----------------------------------------------------------------------------------------------
	// State methods.
	//-----------------------------------------------------------------------------------------------

	void CChunkWorkspace::OnPlay()
	{
		m_grid.SetActive(false);
	}

	void CChunkWorkspace::OnEdit()
	{
		m_grid.SetActive(m_bActive);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Commands methods.
	//-----------------------------------------------------------------------------------------------

	bool CChunkWorkspace::WorkspaceToggle()
	{
		if(m_bActive) return App::CCommandManager::Instance().Execute(CMD_KEY_WORKSPACE_OFF);
		else return App::CCommandManager::Instance().Execute(CMD_KEY_WORKSPACE_ON);
	}

	bool CChunkWorkspace::WorkspaceOff()
	{
		if(!m_bActive) return false;

		m_bActive = false;
		m_grid.SetActive(m_bActive);

		return true;
	}

	bool CChunkWorkspace::WorkspaceOn()
	{
		if(m_bActive) return false;

		m_bActive = true;
		m_grid.SetActive(m_bActive);

		return true;
	}

	bool CChunkWorkspace::WorkspaceReset()
	{
		if(!m_bActive) return false;

		Math::Vector3 offset = *(Math::Vector3*)(App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition() +
			App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetForward() * 12.0f).ToFloat();

		return App::CCommandManager::Instance().Execute(CMD_KEY_WORKSPACE_SET, &offset, sizeof(offset), alignof(Math::Vector3));
	}

	bool CChunkWorkspace::WorkspaceSet(const void* param, bool bInverse)
	{
		if(!m_bActive) return false;

		struct GridData
		{
			Math::Vector3 offset;
			Math::Vector3 scale;
		};

		if(!bInverse)
		{
			Math::Vector3 offset = *(Math::Vector3*)param;
			offset.x = roundf(offset.x * 4.0f) * 0.25f;
			offset.y = roundf(offset.y * 4.0f) * 0.25f;
			offset.z = roundf(offset.z * 4.0f) * 0.25f;

			if(m_grid.GetOffset() == offset && m_grid.GetScale() == m_grid.GetStartingExtents()) return false;

			GridData data { };
			data.offset = *(Math::Vector3*)m_grid.GetOffset().ToFloat();
			data.scale = m_grid.GetScale();
			App::CCommandManager::Instance().RecordUndo(&data, sizeof(data), alignof(GridData));

			m_grid.SetExtents(offset - m_grid.GetStartingHalfExtents(), offset + m_grid.GetStartingHalfExtents());
		}
		else
		{
			GridData data = *reinterpret_cast<const GridData*>(param);
			data.scale *= 0.5f;
			m_grid.SetExtents(data.offset - data.scale, data.offset + data.scale);
		}

		return false;
	}

	void CChunkWorkspace::OnCmdGizmoWorkspace()
	{
		m_grid.SetEdit(true);
	}

	void CChunkWorkspace::OnCmdGizmoWorkspaceOff()
	{
		m_grid.SetEdit(false);
	}
};
