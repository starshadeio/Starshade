//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: UI/CPane.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPane.h"
#include <UI/CUIFrame.h>
#include <UI/CUIImage.h>
#include <UI/CUIText.h>
#include <UI/CUIButton.h>
#include <Utilities/CConvertUtil.h>

namespace UI
{
	CPane::CPane(const CVObject* pObject, UI::CUICanvas* pCanvas) :
		m_frame(pObject, pCanvas),
		m_script(pObject, pCanvas)
	{
	}

	CPane::~CPane()
	{
	}
	
	void CPane::Initialize()
	{
		{ // Setup frame.
			CUIFrame::Data data { };
			data.pivot = 0.5f;
			data.anchorMin = 0.0f;
			data.anchorMax = 1.0f;
			m_frame.SetData(data);
			m_frame.Initialize();
		}

		{ // Build UI from script.
			CUIScript::Data data { };
			data.filename = m_data.filename;
			data.pParent = &m_frame;
			m_script.SetData(data);
			m_script.Build();
		}
	}
	
	void CPane::DebugUpdate()
	{
		//m_script.DebugUpdate();
	}
	
	void CPane::Release()
	{
		m_script.Release();
	}
};
