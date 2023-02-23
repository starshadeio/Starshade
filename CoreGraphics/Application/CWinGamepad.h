//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CWinGamepad.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CWINGAMEPAD_H
#define CWINGAMEPAD_H

#include "CInputGamepad.h"
#include <Windows.h>
#include <Xinput.h>

#pragma comment(lib, "XInput.lib")

namespace App
{
	class CWinGamepad : public CInputGamepad
	{
	public:
		CWinGamepad();
		~CWinGamepad();
		CWinGamepad(const CWinGamepad&) = delete;
		CWinGamepad(CWinGamepad&&) = delete;
		CWinGamepad& operator = (const CWinGamepad&) = delete;
		CWinGamepad& operator = (CWinGamepad&&) = delete;

	public:
		void Update() final;

		void Vibrate(float left, float right) final;

		// Accessors.
		inline bool IsConnected() const final { return m_bConnected; }
		inline u32 GetUserIndex() const final { return m_userIndex; }

	private:
		bool FindFirstConnected();

	private:
		bool m_bConnected;
		u32 m_userIndex;

		XINPUT_STATE m_currentState;
	};
};

#endif
