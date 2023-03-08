//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CCommandManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCOMMANDMANAGER_H
#define CCOMMANDMANAGER_H

#include "CCommand.h"
#include "CCommandRecord.h"
#include "../Globals/CGlobals.h"
#include "../Math/CMathFNV.h"
#include <vector>
#include <queue>
#include <unordered_map>

namespace App
{
	class CCommandManager
	{
	public:
		static const u64 CMD_KEY_UNDO = Math::FNV1a_64("undo");
		static const u64 CMD_KEY_REDO = Math::FNV1a_64("redo");
		static const u64 CMD_KEY_UNDO_EMPTY = Math::FNV1a_64("undo_empty");
		static const u64 CMD_KEY_UNDO_FILL = Math::FNV1a_64("undo_fill");
		static const u64 CMD_KEY_REDO_EMPTY = Math::FNV1a_64("redo_empty");
		static const u64 CMD_KEY_REDO_FILL = Math::FNV1a_64("redo_fill");

	public:
		static CCommandManager& Instance()
		{
			static CCommandManager instance;
			return instance;
		}

	private:
		CCommandManager();
		~CCommandManager();
		CCommandManager(const CCommandManager&) = delete;
		CCommandManager(CCommandManager&&) = delete;
		CCommandManager& operator = (const CCommandManager&) = delete;
		CCommandManager& operator = (CCommandManager&&) = delete;

	public:
		void Initialize();
		void ProcessActionRegistrationQueue();

		bool Execute(u64 key, const void* params, size_t sz, u16 align, bool bInverse = false, bool bRecorderEvent = false);

		template<typename T>
		bool Execute(u64 key, const T* params)
		{
			return Execute(key, params, sizeof(T), alignof(T));
		}

		bool Execute(u64 key)
		{
			return Execute(key, nullptr, 0, 0);
		}


		bool RecordUndo(const void* params = nullptr, size_t sz = 0, u16 align = 0);

		void RegisterCommand(u64 key, CCommand::Command command, const CCommand::Properties& props = {});
		void DeregisterCommand(u64 key);

		void RegisterAction(u64 cmdKey, CCommand::Action action);

		bool MarkUnsavedChanges();
		bool MarkSavedChanges();
		bool MarkChangesReverted();

		// Accessors.
		inline bool IsReady() const { return m_bReady; }

		inline bool CanUndo() const { return m_record.CanUndo(); }
		inline bool CanRedo() const { return m_record.CanRedo(); }
		
		inline bool HasUnsavedChanges() const { return m_bUnsavedChanges; }
		inline u64 GetUnsavedIndex() const { return m_unsavedIndex; }

		// Modifiers.
		inline void AddOnSaveChangeCallback(std::function<void(bool, u64)> func) { m_onSaveChangeList.push_back(func); }
		
		inline void ResetRecorder() { m_record.ResetUndo(); m_record.ResetRedo(); }

	private:
		bool Undo(const void* param);
		bool Redo(const void* param);

		void OnSaveChange();

	private:
		bool m_bReady;
		bool m_bUnsavedChanges;
		u64 m_unsavedIndex;

		CCommandRecord m_record;

		std::vector<std::function<void(bool, u64)>> m_onSaveChangeList;

		std::unordered_map<u64, CCommand> m_commandMap;
		std::queue<std::pair<u64, CCommand::Action>> m_actionQueue;
	};
};

#endif
