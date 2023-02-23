//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Utilities/CDebug.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CDebug.h"

namespace Util
{
	CDebug::CDebug()
	{

	}

	CDebug::~CDebug()
	{

	}
	
	void CDebug::Initialize()
	{
		m_debugDraw.Initialize();
	}

	void CDebug::Release()
	{
		m_debugDraw.Release();
	}
};
