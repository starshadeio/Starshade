//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CCommandRecord.h
//
//-------------------------------------------------------------------------------------------------

#ifndef COMMANDRECORD_H
#define COMMANDRECORD_H

#include "CCommandBuffer.h"
#include <vector>
#include <stack>

namespace App
{
	class CCommandRecord
	{
	public:
		struct Header
		{
			bool bHasParams = false;
			bool bHasParamsInv = false;
			u64 unsavedIndex;
			u64 key;
		};

	public:
		CCommandRecord();
		~CCommandRecord();
		CCommandRecord(const CCommandRecord&) = delete;
		CCommandRecord(CCommandRecord&&) = delete;
		CCommandRecord& operator = (const CCommandRecord&) = delete;
		CCommandRecord& operator = (CCommandRecord&&) = delete;

		bool Record(class CCommand* pCommand, const void* params, size_t sz, u16 align);
		bool RecordUndo(const void* params, size_t sz, u16 align);

		bool Undo();
		bool Redo();

		bool ResetUndo();
		bool ResetRedo();

		bool CanUndo() const;
		bool CanRedo() const;

	private:
		Header m_header;

		size_t m_undoSize;
		std::vector<size_t> m_undoSizeStack;
		std::vector<size_t> m_redoSizeStack;

		CCommandBuffer m_undoBuffer;
		CCommandBuffer m_redoBuffer;
	};
};

#endif
