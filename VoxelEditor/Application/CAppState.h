//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CAppState.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAPPSTATE_H
#define CAPPSTATE_H

#include <Globals/CGlobals.h>
#include <Math/CMathFNV.h>
#include <mutex>
#include <shared_mutex>

namespace App
{
	class CAppState
	{
	public:
		static const u64 CMD_KEY_TOGGLE_PLAY_SPAWN = Math::FNV1a_64("toggle_play_spawn");
		static const u64 CMD_KEY_TOGGLE_PLAY_AT = Math::FNV1a_64("toggle_play_at");
		static const u64 CMD_KEY_PLAY_SPAWN = Math::FNV1a_64("play_spawn");
		static const u64 CMD_KEY_PLAY_AT = Math::FNV1a_64("play_at");
		static const u64 CMD_KEY_EDIT = Math::FNV1a_64("edit");

	public:
		CAppState();
		~CAppState();
		CAppState(const CAppState&) = delete;
		CAppState(CAppState&&) = delete;
		CAppState& operator = (const CAppState&) = delete;
		CAppState& operator = (CAppState&&) = delete;

		void Initialize();
		void Reset();

		// Accessors.
		inline bool IsPlaying() const { return m_bPlaying; }
		
	private:
		bool TogglePlaySpawn();
		bool TogglePlayAt();
		bool PlaySpawn();
		bool PlayAt();
		bool Edit();

	private:
		Abool m_bPlaying;
	};
};

#endif
