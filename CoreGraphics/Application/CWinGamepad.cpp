//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CWinGamepad.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef PLATFORM_WINDOWS

#include "CWinGamepad.h"

namespace App
{
	static const float XINPUT_STICK_TO_FLOAT = 1.0f / 0x7FFF;
	static const float XINPUT_GAMEPAD_LEFT_STICK_DEADZONE_FLOAT = XINPUT_STICK_TO_FLOAT * XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	static const float XINPUT_GAMEPAD_RIGHT_STICK_DEADZONE_FLOAT = XINPUT_STICK_TO_FLOAT * XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

	static const float XINPUT_TRIGGER_TO_FLOAT = 1.0f / 0xFF;
	static const float XINPUT_GAMEPAD_TRIGGER_THRESHOLD_FLOAT = XINPUT_TRIGGER_TO_FLOAT * XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

	CWinGamepad::CWinGamepad() :
		m_bConnected(false),
		m_userIndex(0)
	{
		FindFirstConnected();
	}

	CWinGamepad::~CWinGamepad() { }

	void CWinGamepad::Update()
	{
		if(!m_bConnected) { FindFirstConnected(); }

		if(XInputGetState(m_userIndex, &m_currentState) != ERROR_SUCCESS)
		{
			if(!FindFirstConnected()) { return; }
		}

		SetButton(VK_GP_DPAD_UP, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
		SetButton(VK_GP_DPAD_DOWN, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
		SetButton(VK_GP_DPAD_LEFT, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
		SetButton(VK_GP_DPAD_LEFT, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);

		SetButton(VK_GP_START, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_START);
		SetButton(VK_GP_BACK, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);

		SetButton(VK_GP_ANALOG_LEFT, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
		SetButton(VK_GP_ANALOG_RIGHT, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
		SetButton(VK_GP_SHOULDER_LEFT, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
		SetButton(VK_GP_SHOULDER_RIGHT, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);

		SetButton(VK_GP_SOUTH, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_A);
		SetButton(VK_GP_EAST, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_B);
		SetButton(VK_GP_WEST, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_X);
		SetButton(VK_GP_NORTH, m_currentState.Gamepad.wButtons & XINPUT_GAMEPAD_Y);

		{ // Triggers.
			SetAxis(VK_GP_AXIS_TRIGGER_LEFT, XINPUT_TRIGGER_TO_FLOAT * m_currentState.Gamepad.bLeftTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD_FLOAT);
			SetAxis(VK_GP_AXIS_TRIGGER_RIGHT, XINPUT_TRIGGER_TO_FLOAT * m_currentState.Gamepad.bRightTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD_FLOAT);
		}

		{ // Axes.
			SetAxis(VK_GP_AXIS_LSTICK_X, XINPUT_STICK_TO_FLOAT * m_currentState.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_STICK_DEADZONE_FLOAT);
			SetAxis(VK_GP_AXIS_LSTICK_Y, XINPUT_STICK_TO_FLOAT * m_currentState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_STICK_DEADZONE_FLOAT);

			SetAxis(VK_GP_AXIS_RSTICK_X, XINPUT_STICK_TO_FLOAT * m_currentState.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_STICK_DEADZONE_FLOAT);
			SetAxis(VK_GP_AXIS_RSTICK_Y, XINPUT_STICK_TO_FLOAT * m_currentState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_STICK_DEADZONE_FLOAT);
		}
	}

	void CWinGamepad::Vibrate(float left, float right)
	{
		XINPUT_VIBRATION vibration;
		vibration.wLeftMotorSpeed = static_cast<WORD>(left * 0xFFFF);
		vibration.wRightMotorSpeed = static_cast<WORD>(right * 0xFFFF);
		XInputSetState(m_userIndex, &vibration);
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	bool CWinGamepad::FindFirstConnected()
	{
		for(int i = 0; i < 4; ++i)
		{
			if(XInputGetState(i, &m_currentState) == ERROR_SUCCESS)
			{
				m_bConnected = true;
				m_userIndex = i;
				return true;
			}
		}

		m_bConnected = false;
		return false;
	}
};

#endif
