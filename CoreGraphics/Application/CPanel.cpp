//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CPanel.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPanel.h"

namespace App
{
	CPanel::CPanel() :
		m_bIsHovered(false),
		m_bInFocus(true),
		m_screenMode(PSM_WINDOWED),
		m_screenState(PSS_NORMAL) {
	}

	CPanel::~CPanel() { }

	//-----------------------------------------------------------------------------------------------
	// Event methods.
	//-----------------------------------------------------------------------------------------------

	void CPanel::OnMove()
	{
		if(m_data.funcOnMove)
		{
			m_data.funcOnMove(this);
		}
	}

	void CPanel::OnResize()
	{
		if(m_data.funcOnResize)
		{
			m_data.funcOnResize(this);
		}
	}

	bool CPanel::OnHover(bool bHovered)
	{
		if(m_bIsHovered == bHovered) return false;
		m_bIsHovered = bHovered;
		return true;
	}

	bool CPanel::OnFocus(bool bFocused)
	{
		if(m_bInFocus == bFocused) return false;
		m_bInFocus = bFocused;
		return true;
	}
};
