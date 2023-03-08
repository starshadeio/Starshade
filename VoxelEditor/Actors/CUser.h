//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Actors/CUser.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUSER_H
#define CUSER_H

#include "CMotorMonitor.h"
#include <Actors/CPlayer.h>

namespace Actor
{
	class CUser : public CPlayer
	{
	public:
		CUser(const wchar_t* pName, u32 viewHash);
		~CUser();
		CUser(const CUser&) = delete;
		CUser(CUser&&) = delete;
		CUser& operator = (const CUser&) = delete;
		CUser& operator = (CUser&&) = delete;
		
		void Initialize() final;
		void PostInitialize() final;
		void Release() final;

		void OnPlay(bool bSpawn);
		void OnEdit();

		void Save() const;
		void Load();

	private:
		CMotorMonitor m_motorMonitor;
	};
};

#endif
