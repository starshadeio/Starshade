//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel App
//
// File: Application/CApplication.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAPPLICATION_H
#define CAPPLICATION_H

#include "CSceneGlobal.h"
#include <Application/CAppBase.h>

namespace App
{
	class CApplication : public CAppBase
	{
	public:
		static CApplication& Instance()
		{
			static CApplication instance;
			return instance;
		}

	private:
		CApplication();
		~CApplication();
		CApplication(const CApplication&) = delete;
		CApplication(CApplication&&) = delete;
		CApplication& operator = (const CApplication&) = delete;
		CApplication& operator = (CApplication&&) = delete;
		
	private:
		void Create();
		
	private:
		CSceneGlobal m_sceneGlobal;
	};
};

#endif
