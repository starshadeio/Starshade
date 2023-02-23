//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Actors/CPlayerHUD.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPLAYERHUD_H
#define CPLAYERHUD_H

#include <UI/CUICanvas.h>
#include <UI/CUITransform.h>
#include <Objects/CVObject.h>

namespace Actor
{
	class CPlayerHUD : public CVObject
	{
	public:
		struct Data
		{
		};

	public:
		CPlayerHUD(const wchar_t* pName, u32 sceneHash);
		~CPlayerHUD();

		CPlayerHUD(const CPlayerHUD&) = delete;
		CPlayerHUD(CPlayerHUD&&) = delete;
		CPlayerHUD& operator = (const CPlayerHUD&) = delete;
		CPlayerHUD& operator = (CPlayerHUD&&) = delete;

		void Initialize() final;
		void Update() final;
		void Release() final;
		
		void SaveToFile(std::ofstream& file) const;
		void LoadFromFile(std::ifstream& file);
		
		void OnResize(const App::CPanel* pPanel) final;
		
		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		Data m_data;

		UI::CUICanvas m_canvas;
		UI::CUITransform m_transform;
	};
};

#endif
