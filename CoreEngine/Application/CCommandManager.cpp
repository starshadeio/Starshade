//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CCommandManager.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CCommandManager.h"
#include "CCommandBuffer.h"
#include <cassert>

namespace App
{
	CCommandManager::CCommandManager() :
		m_bReady(false),
		m_bUnsavedChanges(false),
		m_unsavedIndex(1)
	{
	}

	CCommandManager::~CCommandManager()
	{
	}
	
	void CCommandManager::Initialize()
	{
		{ // Setup commands.
			CCommandManager::Instance().RegisterCommand(CMD_KEY_UNDO, std::bind(&CCommandManager::Undo, this, std::placeholders::_1));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_REDO, std::bind(&CCommandManager::Redo, this, std::placeholders::_1));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_UNDO_EMPTY, [](const void* params, bool bInverse, size_t sz, u16 align){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_UNDO_FILL, [](const void* params, bool bInverse, size_t sz, u16 align){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_REDO_EMPTY, [](const void* params, bool bInverse, size_t sz, u16 align){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_REDO_FILL, [](const void* params, bool bInverse, size_t sz, u16 align){ return true; });
		}
	}

	void CCommandManager::ProcessActionRegistrationQueue()
	{
		while(!m_actionQueue.empty())
		{
			auto& actionPair = m_actionQueue.front();
			auto elem = m_commandMap.find(actionPair.first);
			assert(elem != m_commandMap.end());
			elem->second.RegisterAction(actionPair.second);

			m_actionQueue.pop();
		}

		m_bReady = true;
	}

	bool CCommandManager::Execute(u64 key, const void* params, size_t sz, u16 align, bool bInverse, bool bRecorderEvent)
	{
		auto elem = m_commandMap.find(key);
		assert(elem != m_commandMap.end());
		
		bool bResult = false;
		auto& props = elem->second.GetProperties();

		if(props.bUndoEnabled && !bInverse)
		{
			if(!bRecorderEvent)
			{ // Reset redo buffer.
				m_record.ResetRedo();
			}
		
			bResult = m_record.Record(&elem->second, params, sz, align);
		}
		else
		{
			bResult = elem->second.Execute(params, bInverse, sz, align);
		}

		if(bResult)
		{
			if(props.bSaveEnabled && !bRecorderEvent)
			{
				MarkUnsavedChanges();
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	bool CCommandManager::RecordUndo(const void* params, size_t sz, u16 align)
	{
		return m_record.RecordUndo(params, sz, align);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Command methods.
	//-----------------------------------------------------------------------------------------------

	bool CCommandManager::Undo(const void* param)
	{
		return m_record.Undo();
	}

	bool CCommandManager::Redo(const void* param)
	{
		return m_record.Redo();
	}
	
	//-----------------------------------------------------------------------------------------------
	// (De)registration methods.
	//-----------------------------------------------------------------------------------------------

	void CCommandManager::RegisterCommand(u64 key, CCommand::Command command, const CCommand::Properties& props)
	{
		auto res = m_commandMap.emplace(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(key, command, props));
		assert(res.second);
	}
	
	void CCommandManager::DeregisterCommand(u64 key)
	{
		m_commandMap.erase(key);
	}

	void CCommandManager::RegisterAction(u64 cmdKey, CCommand::Action action)
	{
		m_actionQueue.push({ cmdKey, action });
	}

	//-----------------------------------------------------------------------------------------------
	// Save methods.
	//-----------------------------------------------------------------------------------------------

	bool CCommandManager::MarkUnsavedChanges()
	{
		if(m_bUnsavedChanges) return false;
		m_bUnsavedChanges = true;
		OnSaveChange();
		return true;
	}

	bool CCommandManager::MarkSavedChanges()
	{
		if(!m_bUnsavedChanges) return false;
		if(++m_unsavedIndex == 0) m_unsavedIndex = 1;
		m_bUnsavedChanges = false;
		OnSaveChange();
		return true;
	}
	
	bool CCommandManager::MarkChangesReverted()
	{
		m_bUnsavedChanges = false;
		OnSaveChange();
		return true;
	}

	void CCommandManager::OnSaveChange()
	{
		for(auto& elem : m_onSaveChangeList)
		{
			elem(m_bUnsavedChanges, m_unsavedIndex);
		}
	}
};
