//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInputGamepad.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CINPUTGAMEPAD_H
#define CINPUTGAMEPAD_H

#include "CInputDevice.h"
#include "CInputData.h"
#include <unordered_map>
#include <bitset>

namespace App
{
	enum VK_GAMEPAD
	{
		VK_GP_SOUTH,
		VK_GP_EAST,
		VK_GP_WEST,
		VK_GP_NORTH,
		VK_GP_SHOULDER_LEFT,
		VK_GP_SHOULDER_RIGHT,
		VK_GP_BACK,
		VK_GP_START,
		VK_GP_ANALOG_LEFT,
		VK_GP_ANALOG_RIGHT,
		VK_GP_DPAD_LEFT,
		VK_GP_DPAD_RIGHT,
		VK_GP_DPAD_UP,
		VK_GP_DPAD_DOWN,

		VK_GP_AXIS_TRIGGER_LEFT,
		VK_GP_AXIS_TRIGGER_RIGHT,
		VK_GP_AXIS_LSTICK_X,
		VK_GP_AXIS_LSTICK_Y,
		VK_GP_AXIS_RSTICK_X,
		VK_GP_AXIS_RSTICK_Y,
	};
	
	const std::wstring VK_GAMEPAD_STR[] = {
		L"GP South",
		L"GP East",
		L"GP West",
		L"GP North",
		L"GP LShoulder",
		L"GP RShoulder",
		L"GP Back",
		L"GP Start",
		L"GP LAnalog",
		L"GP RAnalog",
		L"GP DPad Left",
		L"GP DPad Right",
		L"GP DPad Up",
		L"GP DPad Down",

		L"GP LTrigger",
		L"GP RTrigger",
		L"GP LStick X",
		L"GP LStick Y",
		L"GP RStick X",
		L"GP RStick Y",
	};

	class CInputGamepad : public CInputDevice
	{
	public:
		CInputGamepad();
		~CInputGamepad();
		CInputGamepad(const CInputGamepad&) = delete;
		CInputGamepad(CInputGamepad&&) = delete;
		CInputGamepad& operator = (const CInputGamepad&) = delete;
		CInputGamepad& operator = (CInputGamepad&&) = delete;

		void Initialize() final;
		void Reset() final;
		void PostUpdate() final;
		void Release() final;

		void RegisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding) final;
		void DeregisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding) final;

		void SetButton(VK_GAMEPAD code, bool bPressed);
		void SetAxis(VK_GAMEPAD code, float value, float deadzone);
		virtual void Vibrate(float left, float right) { }

		// Accessors.
		inline const std::wstring& GetCodeName(u32 code) const final { return VK_GAMEPAD_STR[code]; }

		virtual inline bool IsConnected() const = 0;
		virtual inline u32 GetUserIndex() const = 0;

	private:
		std::bitset<24> m_buttonSet;
		std::unordered_map<u64, std::unordered_map<u32, InputBinding>> m_bindingMap;
	};
};

#endif
