//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CPlayerHUD.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPLAYERHUD_H
#define CPLAYERHUD_H

#include "CHUD.h"
#include <fstream>

namespace Actor
{
	class CPlayerHUD : public CHUD
	{
	public:
		CPlayerHUD(const wchar_t* pName, u32 viewHash);
		~CPlayerHUD();
		CPlayerHUD(const CPlayerHUD&) = delete;
		CPlayerHUD(CPlayerHUD&&) = delete;
		CPlayerHUD& operator = (const CPlayerHUD&) = delete;
		CPlayerHUD& operator = (CPlayerHUD&&) = delete;

		void Initialize() final;
		void Update() final;
		void Release() final;
		
		void OnResize(const Math::Rect& rect) final;
		
		void SaveToFile(std::ofstream& file) const override;
		void LoadFromFile(std::ifstream& file) override;

	private:
	};
};

#endif
