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
		static const char INPUT_KEY_MOVE_FORWARD[];
		static const char INPUT_KEY_MOVE_BACKWARD[];
		static const char INPUT_KEY_MOVE_LEFT[];
		static const char INPUT_KEY_MOVE_RIGHT[];

		static const char INPUT_KEY_LOOK_H[];
		static const char INPUT_KEY_LOOK_V[];
	
		static const char INPUT_KEY_JUMP[];

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
		virtual void MoveForward(const App::InputCallbackData& callback);
		virtual void MoveBackward(const App::InputCallbackData& callback);
		virtual void MoveLeft(const App::InputCallbackData& callback);
		virtual void MoveRight(const App::InputCallbackData& callback);

		virtual void LookHorizontal(const App::InputCallbackData& callback);
		virtual void LookVertical(const App::InputCallbackData& callback);

		virtual void Jump(const App::InputCallbackData& callback);

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
