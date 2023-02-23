//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CUINull.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUINull.h"
#include <Math/CMathFNV.h>

namespace Actor
{
	const u32 CUINull::UI_HASH = Math::FNV1a_32("UI_NULL");

	CUINull::CUINull(const CVObject* pObject) :
		CUIBase(pObject, UI_HASH)
	{
	}

	CUINull::~CUINull()
	{
	}
};
