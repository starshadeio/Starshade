//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Application/CAppBase.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAppBase.h"
#include "../Utilities/CTimer.h"

namespace App
{
	CAppBase::CAppBase() { }
	CAppBase::~CAppBase() { }

	void CAppBase::Create() { }

	void CAppBase::Run()
	{
		Util::CTimer::Instance().Tick();
	}

	void CAppBase::Destroy() { }
};
