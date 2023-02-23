//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CAppState.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAppState.h"
#include "CAppData.h"
#include <Application/CCommandManager.h>
#include <Application/CInput.h>
#include "CEditor.h"

namespace App
{
	CAppState::CAppState()
	{
	}

	CAppState::~CAppState()
	{
	}
	
	void CAppState::Initialize()
	{
		App::CCommandManager::Instance().RegisterCommand(CMD_KEY_TOGGLE_PLAY_SPAWN, std::bind(&CAppState::TogglePlaySpawn, this));
		App::CCommandManager::Instance().RegisterCommand(CMD_KEY_TOGGLE_PLAY_AT, std::bind(&CAppState::TogglePlayAt, this));
		App::CCommandManager::Instance().RegisterCommand(CMD_KEY_PLAY_SPAWN, std::bind(&CAppState::PlaySpawn, this));
		App::CCommandManager::Instance().RegisterCommand(CMD_KEY_PLAY_AT, std::bind(&CAppState::PlayAt, this));
		App::CCommandManager::Instance().RegisterCommand(CMD_KEY_EDIT, std::bind(&CAppState::Edit, this));
	}
	
	void CAppState::Reset()
	{
		m_bPlaying = false;
	}
	
	bool CAppState::TogglePlaySpawn()
	{
		if(!m_bPlaying)
		{
			App::CCommandManager::Instance().Execute(CMD_KEY_PLAY_SPAWN);
		}
		else
		{
			App::CCommandManager::Instance().Execute(CMD_KEY_EDIT);
		}

		return true;
	}
	
	bool CAppState::TogglePlayAt()
	{
		if(!m_bPlaying)
		{
			App::CCommandManager::Instance().Execute(CMD_KEY_PLAY_AT);
		}
		else
		{
			App::CCommandManager::Instance().Execute(CMD_KEY_EDIT);
		}

		return true;
	}
	
	bool CAppState::PlaySpawn()
	{
		if(m_bPlaying) return false;
		m_bPlaying = true;

		CInput::Instance().SwitchLayout(INPUT_HASH_LAYOUT_PLAYER);
		return true;
	}
	
	bool CAppState::PlayAt()
	{
		if(m_bPlaying) return false;
		m_bPlaying = true;

		CInput::Instance().SwitchLayout(INPUT_HASH_LAYOUT_PLAYER);
		return true;
	}
	
	bool CAppState::Edit()
	{
		if(!m_bPlaying) return false;
		m_bPlaying = false;

		CInput::Instance().SwitchLayout(INPUT_HASH_LAYOUT_EDITOR);
		return true;
	}
};
