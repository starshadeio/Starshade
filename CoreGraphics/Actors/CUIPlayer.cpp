//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CUIPlayer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIPlayer.h"
#include <Math/CMathFNV.h>

namespace Actor
{
	const u32 CUIPlayer::UI_HASH = Math::FNV1a_32("UI_PLAYER");

	CUIPlayer::CUIPlayer(const CVObject* pObject) :
		CUIBase(pObject, UI_HASH)
	{
	}

	CUIPlayer::~CUIPlayer()
	{
	}
};
