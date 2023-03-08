//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CCommand.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCOMMAND_H
#define CCOMMAND_H

#include "../Globals/CGlobals.h"
#include <functional>
#include <vector>

namespace App
{
	class CCommand
	{
	public:
		typedef std::function<bool(const void*, bool, size_t, u16)> Command;
		typedef std::function<void(const void*, bool, size_t, u16)> Action;

		struct Properties
		{
			bool bSaveEnabled = false;
			bool bUndoEnabled = false;
		};

	public:
		CCommand(u64 key, Command cmd, const Properties& props);
		~CCommand();
		CCommand(const CCommand&) = delete;
		CCommand(CCommand&&) = delete;
		CCommand& operator = (const CCommand&) = delete;
		CCommand& operator = (CCommand&&) = delete;

		bool Execute(const void* params = nullptr, bool bInverse = false, size_t sz = 0, u16 align = 0);
		void RegisterAction(Action action);

		// Accessors.
		inline u64 GetKey() const { return m_key; }
		inline const Properties& GetProperties() const { return m_properties; }

	private:
		u64 m_key;
		Properties m_properties;

		Command m_command;
		std::vector<Action> m_actionList;
	};
};

#endif
