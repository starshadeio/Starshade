//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CCommandRecord.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CCommandRecord.h"
#include "CCommandManager.h"
#include "CCommand.h"
#include <cassert>

namespace App
{
	
	CCommandRecord::CCommandRecord() :
		m_undoSize(0),
		m_undoBuffer(1024 * 1024, 256, 64),
		m_redoBuffer(1024 * 1024, 256, 64)
	{
	}

	CCommandRecord::~CCommandRecord()
	{
	}
	
	bool CCommandRecord::Record(CCommand* pCommand, const void* params, size_t sz, u16 align)
	{
		assert(m_undoSize == 0);

		m_header = { };
		m_header.unsavedIndex = CCommandManager::Instance().HasUnsavedChanges() ? 0 : CCommandManager::Instance().GetUnsavedIndex();
		m_header.key = pCommand->GetKey();

		if(m_header.bHasParams = sz)
		{
			m_undoSize += m_undoBuffer.Push((u8*)params, sz, align);
		}

		bool bResult = pCommand->Execute(params);
		if(!bResult)
		{
			if(m_undoSize)
			{
				m_undoBuffer.Pop([](const u8* pData, size_t sz, u16 align){});
				m_undoSize = 0;
			}
		}

		return bResult;
	}
	
	bool CCommandRecord::RecordUndo(const void* params, size_t sz, u16 align)
	{
		if(m_header.bHasParamsInv = sz)
		{
			m_undoSize += m_undoBuffer.Push((u8*)params, sz, align);
		}

		m_undoSize += m_undoBuffer.Push((u8*)&m_header, sizeof(m_header), alignof(Header));

		m_undoSizeStack.push_back(m_undoSize);
		m_undoSize = 0;

		CCommandManager::Instance().Execute(CanUndo() ? CCommandManager::CMD_KEY_UNDO_FILL : CCommandManager::CMD_KEY_UNDO_EMPTY);

		return true;
	}

	bool CCommandRecord::Undo()
	{
		if(m_undoSizeStack.empty()) return false;
		const size_t targetSize = m_undoSizeStack.back();
		m_undoSizeStack.pop_back();
		if(targetSize > m_undoBuffer.Size())
		{
			ResetUndo();
			return false;
		}

		const u8* pUndo = nullptr;
		size_t undoSz = 0;
		u16 undoAlign = 0;

		const u8* pRedo = nullptr;
		size_t redoSz = 0;
		u16 redoAlign = 0;
		
		Header header = { };

		if(!m_undoBuffer.Pop([&](const u8* pData, size_t sz, u16 align){
			header = *(Header*)pData;
		})) return false;

		if(header.bHasParamsInv)
		{
			if(!m_undoBuffer.Pop([&](const u8* pData, size_t sz, u16 align){
				pUndo = pData;
				undoSz = sz;
				undoAlign = align;
			})) return false;
		}
		
		if(header.bHasParams)
		{
			if(!m_undoBuffer.Pop([&](const u8* pData, size_t sz, u16 align){
				pRedo = pData;
				redoSz = sz;
				redoAlign = align;
			})) return false;
		}
		
		// Check save.
		const u64 lastIndex = header.unsavedIndex;
		header.unsavedIndex = CCommandManager::Instance().HasUnsavedChanges() ? 0 : CCommandManager::Instance().GetUnsavedIndex();
		
		CCommandManager::Instance().Execute(header.key, pUndo, undoSz, undoAlign, true, true);

		if(lastIndex == CCommandManager::Instance().GetUnsavedIndex())
		{
			CCommandManager::Instance().MarkChangesReverted();
		}
		else if(!CCommandManager::Instance().HasUnsavedChanges())
		{
			CCommandManager::Instance().MarkUnsavedChanges();
		}
		
		size_t redoSize = 0;

		header.bHasParamsInv = false;
		if(header.bHasParams = redoSz)
		{
			redoSize += m_redoBuffer.Push(pRedo, redoSz, redoAlign);
		}

		redoSize += m_redoBuffer.Push((u8*)&header, sizeof(header), alignof(Header));

		m_redoSizeStack.push_back(redoSize);
		
		CCommandManager::Instance().Execute(CanUndo() ? CCommandManager::CMD_KEY_UNDO_FILL : CCommandManager::CMD_KEY_UNDO_EMPTY);
		CCommandManager::Instance().Execute(CanRedo() ? CCommandManager::CMD_KEY_REDO_FILL : CCommandManager::CMD_KEY_REDO_EMPTY);

		return true;
	}

	bool CCommandRecord::Redo()
	{
		if(m_redoSizeStack.empty()) return false;
		const size_t targetSize = m_redoSizeStack.back();
		m_redoSizeStack.pop_back();
		if(targetSize > m_redoBuffer.Size())
		{
			ResetRedo();
			return false;
		}

		const u8* pRedo = nullptr;
		size_t redoSz = 0;
		u16 redoAlign = 0;

		Header header;

		if(!m_redoBuffer.Pop([&](const u8* pData, size_t sz, u16 align){
			header = *(Header*)pData;
		})) return false;

		if(header.bHasParams)
		{
			if(!m_redoBuffer.Pop([&](const u8* pData, size_t sz, u16 align){
				pRedo = pData;
				redoSz = sz;
				redoAlign = align;
			})) return false;
		}

		CCommandManager::Instance().Execute(header.key, pRedo, redoSz, redoAlign, false, true);
		
		if(header.unsavedIndex == CCommandManager::Instance().GetUnsavedIndex())
		{
			CCommandManager::Instance().MarkChangesReverted();
		}
		else if(!CCommandManager::Instance().HasUnsavedChanges())
		{
			CCommandManager::Instance().MarkUnsavedChanges();
		}
		
		CCommandManager::Instance().Execute(CanRedo() ? CCommandManager::CMD_KEY_REDO_FILL : CCommandManager::CMD_KEY_REDO_EMPTY);

		return true;
	}

	bool CCommandRecord::ResetUndo()
	{
		if(!CanUndo()) return false;
		m_undoBuffer.Clear();
		m_undoSizeStack.clear();
		m_undoSize = 0;

		CCommandManager::Instance().Execute(CCommandManager::CMD_KEY_UNDO_EMPTY);

		return true;
	}

	bool CCommandRecord::ResetRedo()
	{
		if(!CanRedo()) return false;
		m_redoBuffer.Clear();
		m_redoSizeStack.clear();
		
		CCommandManager::Instance().Execute(CCommandManager::CMD_KEY_REDO_EMPTY);

		return true;
	}

	bool CCommandRecord::CanUndo() const
	{
		return !m_undoBuffer.Empty() && !m_undoSizeStack.empty() && m_undoSizeStack.back() <= m_undoBuffer.Size();
	}

	bool CCommandRecord::CanRedo() const
	{
		return !m_redoBuffer.Empty() && !m_redoSizeStack.empty() && m_redoSizeStack.back() <= m_redoBuffer.Size();
	}
};
