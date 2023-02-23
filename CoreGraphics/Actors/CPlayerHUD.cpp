//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CPlayerHUD.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPlayerHUD.h"
#include "CUIBase.h"
#include "../Factory/CFactory.h"
#include "../Graphics/CGraphicsAPI.h"
#include <Math/CMathFNV.h>
#include <Utilities/CTimer.h>

namespace Actor
{
	CPlayerHUD::CPlayerHUD(const wchar_t* pName, u32 viewHash) :
		CHUD(pName, viewHash)
	{
	}

	CPlayerHUD::~CPlayerHUD()
	{
	}
	
	void CPlayerHUD::Initialize()
	{
	}
	
	void CPlayerHUD::Update()
	{
		if(m_pUI) m_pUI->Update();
	}
	
	void CPlayerHUD::Release()
	{
	}
	
	void CPlayerHUD::OnResize(const Math::Rect& rect)
	{
		for(auto& elem : m_uiMap)
		{
			elem.second->OnResize(rect);
		}
	}
		
	//-----------------------------------------------------------------------------------------------
	// File methods.
	//-----------------------------------------------------------------------------------------------

	void CPlayerHUD::SaveToFile(std::ofstream& file) const
	{
	}

	void CPlayerHUD::LoadFromFile(std::ifstream& file)
	{
	}
};
