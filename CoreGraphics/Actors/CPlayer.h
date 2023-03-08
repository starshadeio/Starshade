//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CPlayer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPLAYER_H
#define CPLAYER_H

#include "CPlayerPawn.h"
#include "CPlayerHUD.h"
#include "CMotorNull.h"
#include "CMotorHumanoid.h"
#include "CUINull.h"
#include "CUIPlayer.h"
#include "../Application/CInputData.h"
#include <Objects/CVObject.h>
#include <Globals/CGlobals.h>

namespace Actor
{
	class CPlayer : public CVObject
	{
	public:
		struct Data
		{
			u32 inputHash;
			CPlayerPawn::Data pawnData;
		};

	public:
		CPlayer(const wchar_t* pName, u32 viewHash);
		virtual ~CPlayer();
		CPlayer(const CPlayer&) = delete;
		CPlayer(CPlayer&&) = delete;
		CPlayer& operator = (const CPlayer&) = delete;
		CPlayer& operator = (CPlayer&&) = delete;
		
		void Initialize() override;
		void PostInitialize() override;
		void Update() final;
		void Release() override;
		
		void OnResize(const Math::Rect& rect) final;
		
		void SaveToFile(std::ofstream& file) const override;
		void LoadFromFile(std::ifstream& file) override;

		// Accessors.
		inline CPlayerPawn& Pawn() { return m_pawn; }
		inline CPlayerHUD& HUD() { return m_hud; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	protected:
		Data m_data;
		CPlayerPawn m_pawn;
		CPlayerHUD m_hud;

		CMotorNull m_motorNull;
		CMotorHumanoid m_motorHumanoid;
		CUINull m_uiNull;
		CUIPlayer m_uiPlayer;
	};
};

#endif
