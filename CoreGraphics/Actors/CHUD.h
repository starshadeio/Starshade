//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CHUD.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CHUD_H
#define CHUD_H

#include <Globals/CGlobals.h>
#include <Objects/CVObject.h>
#include <unordered_map>

namespace Actor
{
	class CHUD : public CVObject
	{
	public:
		CHUD(const wchar_t* pName, u32 viewHash);
		virtual ~CHUD();
		CHUD(const CHUD&) = delete;
		CHUD(CHUD&&) = delete;
		CHUD& operator = (const CHUD&) = delete;
		CHUD& operator = (CHUD&&) = delete;
		
		bool RegisterUI(class CUIBase* pUI);
		bool DeregisterUI(u32 key);
		bool LoadUI(u32 key);

	protected:
		std::unordered_map<u32, class CUIBase*> m_uiMap;

		class CUIBase* m_pUI;
	};
};

#endif
