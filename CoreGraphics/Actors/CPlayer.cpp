//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CPlayer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPlayer.h"
#include "CPlayerPawn.h"
#include "CMotor.h"
#include "../Application/CInput.h"
#include "../Application/CInputDeviceList.h"
#include "../Application/CInputKeyboard.h"
#include "../Application/CInputMouse.h"
#include "../Application/CKeybinds.h"

namespace Actor
{
	CPlayer::CPlayer(const wchar_t* pName, u32 viewHash) : 
		CVObject(pName, viewHash),
		m_pawn(L"Player Pawn", viewHash),
		m_hud(L"Player HUD", viewHash),
		m_motorNull(&m_pawn),
		m_motorHumanoid(&m_pawn),
		m_uiNull(&m_hud),
		m_uiPlayer(&m_hud)
	{
	}

	CPlayer::~CPlayer()
	{
	}

	void CPlayer::Initialize()
	{
		{ // Setup the player pawn.
			m_pawn.SetData(m_data.pawnData);
			m_pawn.Initialize();

			m_pawn.RegisterMotor(&m_motorNull);
			m_pawn.RegisterMotor(&m_motorHumanoid);

			m_motorNull.Initialize();

			{
				CMotorHumanoid::Data data { };
				data.speed = 3.5f;
				data.lookRate = 0.08f;
				data.jumpPower = 7.5f;
				m_motorHumanoid.SetData(data);
				m_motorHumanoid.Initialize();
			}
		}

		{ // Setup the player HUD.
			m_hud.Initialize();

			m_hud.RegisterUI(&m_uiNull);
			m_hud.RegisterUI(&m_uiPlayer);

			m_uiNull.Initialize();
			m_uiPlayer.Initialize();
		}

		App::CInput::Instance().Keybinds()->AddProcessor(std::bind(&CPlayerPawn::ProcessInput, &m_pawn, std::placeholders::_1, std::placeholders::_2));
	}

	void CPlayer::PostInitialize()
	{
		m_pawn.LoadMotor(CMotorNull::MOTOR_HASH);
	}

	void CPlayer::Update()
	{
		m_pawn.Update();
		m_hud.Update();
	}
	
	void CPlayer::Release()
	{
		m_uiPlayer.Release();
		m_uiNull.Release();
		m_hud.Release();

		m_motorHumanoid.Release();
		m_motorNull.Release();
		m_pawn.Release();
	}
		
	void CPlayer::OnResize(const Math::Rect& rect)
	{
		m_pawn.OnResize(rect);
		m_hud.OnResize(rect);
	}
	
	//------------------------------------------------------------------------------------------------
	// File methods.
	//------------------------------------------------------------------------------------------------
	
	void CPlayer::SaveToFile(std::ofstream& file) const
	{
		m_hud.SaveToFile(file);
		m_uiNull.SaveToFile(file);
		m_uiPlayer.SaveToFile(file);

		m_pawn.SaveToFile(file);
		m_motorNull.SaveToFile(file);
		m_motorHumanoid.SaveToFile(file);
	}
	
	void CPlayer::LoadFromFile(std::ifstream& file)
	{
		m_hud.LoadFromFile(file);
		m_uiNull.LoadFromFile(file);
		m_uiPlayer.LoadFromFile(file);

		m_pawn.LoadFromFile(file);
		m_motorNull.LoadFromFile(file);
		m_motorHumanoid.LoadFromFile(file);
	}
};
