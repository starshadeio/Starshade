//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CUINull.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUINULL_H
#define CUINULL_H

#include "CUIBase.h"

namespace Actor
{
	class CUINull : public CUIBase
	{
	public:
		static const u32 UI_HASH;

	public:
		CUINull(const CVObject* pObject);
		~CUINull();
		CUINull(const CUINull&) = delete;
		CUINull(CUINull&&) = delete;
		CUINull& operator = (const CUINull&) = delete;
		CUINull& operator = (CUINull&&) = delete;
	};
};

#endif
