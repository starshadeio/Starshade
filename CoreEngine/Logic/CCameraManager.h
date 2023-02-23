//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Logic/CCameraManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCAMERAMANAGER_H
#define CCAMERAMANAGER_H

#include "CCamera.h"

namespace Logic
{
	class CCameraManager
	{
	public:
		CCameraManager();
		~CCameraManager();
		CCameraManager(const CCameraManager&) = delete;
		CCameraManager(CCameraManager&) = delete;
		CCameraManager& operator = (const CCameraManager&) = delete;
		CCameraManager& operator = (CCameraManager&) = delete;

		// Accessors.
		inline const Logic::CCamera* GetDefaultCamera() const { return m_pDefault; }
		
		// Modifiers.
		inline void SetDefault(Logic::CCamera* pDefault) { m_pDefault = pDefault; }

	private:
		Logic::CCamera* m_pDefault;
	};
};

#endif
