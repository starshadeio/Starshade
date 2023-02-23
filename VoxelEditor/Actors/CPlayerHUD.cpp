//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Actors/CPlayerHUD.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPlayerHUD.h"

namespace Actor
{
	CPlayerHUD::CPlayerHUD(const wchar_t* pName, u32 sceneHash) : 
		CVObject(pName, sceneHash),
		m_canvas(this),
		m_transform(this)
	{
	}

	CPlayerHUD::~CPlayerHUD()
	{
	}

	void CPlayerHUD::Initialize()
	{
		{ // Setup canvas.
			UI::CUICanvas::Data data { };
			data.style = UI::CanvasStyle::Screen;
			data.pTransform = &m_transform;
		}
	}

	void CPlayerHUD::Update()
	{
	}

	void CPlayerHUD::Release()
	{
	}

	void CPlayerHUD::SaveToFile(std::ofstream& file) const
	{
	}

	void CPlayerHUD::LoadFromFile(std::ifstream& file)
	{
	}

	void CPlayerHUD::OnResize(const App::CPanel* pPanel)
	{
	}
};
