//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CChunkEditor.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CChunkEditor.h"
#include "../Application/CEditor.h"
#include "../Application/CAppData.h"
#include <Factory/CFactory.h>
#include <Application/CPlatform.h>
#include <Application/CPanel.h>
#include <Application/CSceneManager.h>
#include <Application/CCommandManager.h>
#include <Application/CLocalization.h>
#include <Math/CMathFNV.h>

namespace Universe
{
	CChunkEditor::CChunkEditor() :
		m_pNode(nullptr)
	{
	}

	CChunkEditor::~CChunkEditor()
	{
	}

	void CChunkEditor::Initialize()
	{
		{ // Setup commands.
			App::CCommand::Properties props = { };
			props.bSaveEnabled = true;
			props.bUndoEnabled = false;

			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_CHUNK_RESET, std::bind(&CChunkEditor::ChunkReset, this, std::placeholders::_1));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_CHUNK_CLEAR, std::bind(&CChunkEditor::ChunkClear, this, std::placeholders::_1), props);
		}
	}

	void CChunkEditor::LateUpdate()
	{
	}

	void CChunkEditor::Release()
	{
	}
	
	//-----------------------------------------------------------------------------------------------
	// Chunk methods.
	//-----------------------------------------------------------------------------------------------
	
	static const u64 LOC_KEY_MSG_TITLE_CONFIRM = Math::FNV1a_64("MENUS:MSG_TITLE_CONFIRM");
	static const u64 LOC_KEY_MSG_CLEAR_CHUNK = Math::FNV1a_64("MENUS:MSG_CLEAR_CHUNK");

	bool CChunkEditor::ChunkReset(const void* param)
	{
		if(m_pNode == nullptr) return false;
		m_pNode->GetChunkNode()->Reset();

		return true;
	}

	bool CChunkEditor::ChunkClear(const void* param)
	{
		if(m_pNode == nullptr) return false;

		auto result = CFactory::Instance().GetPlatform()->PostMessageBox(App::CLocalization::Instance().Get(LOC_KEY_MSG_CLEAR_CHUNK).Get().c_str(), 
			App::CLocalization::Instance().Get(LOC_KEY_MSG_TITLE_CONFIRM).Get().c_str(), App::MessageBoxType::YesNoCancel);

		if(result == App::MessageBoxState::Yes)
		{
			m_pNode->GetChunkNode()->Clear();
			App::CCommandManager::Instance().ResetRecorder();

			return true;
		}

		return false;
	}
};
