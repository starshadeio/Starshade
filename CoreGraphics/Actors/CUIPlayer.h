//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CUIPlayer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUIPLAYER_H
#define CUIPLAYER_H

#include "CUIBase.h"

namespace Actor
{
	class CUIPlayer : public CUIBase
	{
	public:
		static const u32 UI_HASH;

	public:
		CUIPlayer(const CVObject* pObject);
		~CUIPlayer();
		CUIPlayer(const CUIPlayer&) = delete;
		CUIPlayer(CUIPlayer&&) = delete;
		CUIPlayer& operator = (const CUIPlayer&) = delete;
		CUIPlayer& operator = (CUIPlayer&&) = delete;
	};
};

#endif
