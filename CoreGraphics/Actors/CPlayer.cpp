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

namespace Actor
{
	const char CPlayer::INPUT_KEY_MOVE_FORWARD[] = "Move Forward";
	const char CPlayer::INPUT_KEY_MOVE_BACKWARD[] = "Move Backward";
	const char CPlayer::INPUT_KEY_MOVE_LEFT[] = "Move Left";
	const char CPlayer::INPUT_KEY_MOVE_RIGHT[] = "Move Right";

	const char CPlayer::INPUT_KEY_LOOK_H[] = "Look Horizontal";
	const char CPlayer::INPUT_KEY_LOOK_V[] = "Look Vertical";
	
	const char CPlayer::INPUT_KEY_JUMP[] = "Jump";

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
		// Movement.
		App::CInput::Instance().RegisterBinding(&m_data.inputHash, 1,
			App::InputBindingSet(INPUT_KEY_MOVE_FORWARD, std::bind(&CPlayer::MoveForward, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_W), App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_UP))
		);

		App::CInput::Instance().RegisterBinding(&m_data.inputHash, 1,
			App::InputBindingSet(INPUT_KEY_MOVE_BACKWARD, std::bind(&CPlayer::MoveBackward, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_S), App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_DOWN))
		);

		App::CInput::Instance().RegisterBinding(&m_data.inputHash, 1,
			App::InputBindingSet(INPUT_KEY_MOVE_LEFT, std::bind(&CPlayer::MoveLeft, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_A), App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_LEFT))
		);

		App::CInput::Instance().RegisterBinding(&m_data.inputHash, 1,
			App::InputBindingSet(INPUT_KEY_MOVE_RIGHT, std::bind(&CPlayer::MoveRight, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_D), App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_RIGHT))
		);

		// Look.
		App::CInput::Instance().RegisterBinding(&m_data.inputHash, 1,
			App::InputBindingSet(INPUT_KEY_LOOK_H, std::bind(&CPlayer::LookHorizontal, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_DELTA_X), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		App::CInput::Instance().RegisterBinding(&m_data.inputHash, 1,
			App::InputBindingSet(INPUT_KEY_LOOK_V, std::bind(&CPlayer::LookVertical, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_DELTA_Y), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		// Actions.
		App::CInput::Instance().RegisterBinding(&m_data.inputHash, 1,
			App::InputBindingSet(INPUT_KEY_JUMP, std::bind(&CPlayer::Jump, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_SPACE), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

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

	//-----------------------------------------------------------------------------------------------
	// Input callbacks.
	//-----------------------------------------------------------------------------------------------
	
	void CPlayer::MoveForward(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_MOVE_FORWARD, callback);
	}

	void CPlayer::MoveBackward(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_MOVE_BACKWARD, callback);
	}

	void CPlayer::MoveLeft(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_MOVE_LEFT, callback);
	}

	void CPlayer::MoveRight(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_MOVE_RIGHT, callback);
	}

	void CPlayer::LookHorizontal(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_LOOK_HORIZONTAL, callback);
	}

	void CPlayer::LookVertical(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_LOOK_VERTICAL, callback);
	}

	void CPlayer::Jump(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_JUMP, callback);
	}
};
