//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CChunkWorkspace.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCHUNKWORKSPACE_H
#define CCHUNKWORKSPACE_H

#include "CChunkGrid.h"
#include <Math/CMathFNV.h>
#include <fstream>

namespace Universe
{
	class CChunkWorkspace
	{
	public:
		static const u64 CMD_KEY_WORKSPACE_TOGGLE = Math::FNV1a_64("workspace_toggle");
		static const u64 CMD_KEY_WORKSPACE_OFF = Math::FNV1a_64("workspace_off");
		static const u64 CMD_KEY_WORKSPACE_ON = Math::FNV1a_64("workspace_on");
		static const u64 CMD_KEY_WORKSPACE_RESET = Math::FNV1a_64("workspace_reset");

	private:
		static const u64 CMD_KEY_WORKSPACE_SET = Math::FNV1a_64("workspace_set");

	public:
		struct Data
		{
			
		};

	public:
		CChunkWorkspace();
		~CChunkWorkspace();
		CChunkWorkspace(const CChunkWorkspace&) = delete;
		CChunkWorkspace(CChunkWorkspace&&) = delete;
		CChunkWorkspace& operator = (const CChunkWorkspace&) = delete;
		CChunkWorkspace& operator = (CChunkWorkspace&&) = delete;

		void Initialize();
		void PostInitialize();
		void Update();
		void LateUpdate();
		void Release();

		void Save(std::ofstream& file) const;
		void Load(std::ifstream& file);

		void OnPlay();
		void OnEdit();

		// Accessors.
		inline bool IsActive() const { return m_bActive; }

	private:
		bool WorkspaceToggle();
		bool WorkspaceOff();
		bool WorkspaceOn();
		bool WorkspaceReset();
		bool WorkspaceSet(const void* param, bool bInverse);

		void OnCmdGizmoWorkspace();
		void OnCmdGizmoWorkspaceOff();

	private:
		bool m_bActive;
		CChunkGrid m_grid;
	};
};

#endif
