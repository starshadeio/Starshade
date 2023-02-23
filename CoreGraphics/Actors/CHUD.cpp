//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CHUD.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CHUD.h"
#include "CUIBase.h"

namespace Actor
{
	CHUD::CHUD(const wchar_t* pName, u32 viewHash) :
		CVObject(pName, viewHash),
		m_pUI(nullptr)
	{
	}

	CHUD::~CHUD()
	{
	}
		
	//-----------------------------------------------------------------------------------------------
	// UI methods.
	//-----------------------------------------------------------------------------------------------

	bool CHUD::RegisterUI(CUIBase* pUI)
	{
		return m_uiMap.insert({ pUI->GetHash(), pUI }).second;
	}
		
	bool CHUD::DeregisterUI(u32 key)
	{
		return m_uiMap.erase(key) == 1;
	}
		
	bool CHUD::LoadUI(u32 key)
	{
		auto elem = m_uiMap.find(key);
		if(elem != m_uiMap.end())
		{
			if(m_pUI) m_pUI->Unload();
			m_pUI = elem->second;
			m_pUI->Load();

			return true;
		}

		return false;
	}
};
