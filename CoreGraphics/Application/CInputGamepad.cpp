//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInputGamepad.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CInputGamepad.h"
#include "CInputLayout.h"
#include "CInput.h"
#include <cassert>
#include <Math/CMathFloat.h>

namespace App
{
	const std::unordered_map<std::wstring, VK_GAMEPAD> CInputGamepad::CODE_MAP = {
		{ L"SOUTH", VK_GP_SOUTH },
		{ L"EAST", VK_GP_EAST },
		{ L"WEST", VK_GP_WEST },
		{ L"NORTH", VK_GP_NORTH },
		{ L"SHOULDER_LEFT", VK_GP_SHOULDER_LEFT },
		{ L"SHOULDER_RIGHT", VK_GP_SHOULDER_RIGHT },
		{ L"BACK", VK_GP_BACK },
		{ L"START", VK_GP_START },
		{ L"ANALOG_LEFT", VK_GP_ANALOG_LEFT },
		{ L"ANALOG_RIGHT", VK_GP_ANALOG_RIGHT },
		{ L"DPAD_LEFT", VK_GP_DPAD_LEFT },
		{ L"DPAD_RIGHT", VK_GP_DPAD_RIGHT },
		{ L"DPAD_UP", VK_GP_DPAD_UP },
		{ L"DPAD_DOWN", VK_GP_DPAD_DOWN },

		{ L"AXIS_TRIGGER_LEFT", VK_GP_AXIS_TRIGGER_LEFT },
		{ L"AXIS_TRIGGER_RIGHT", VK_GP_AXIS_TRIGGER_RIGHT },
		{ L"AXIS_LSTICK_X", VK_GP_AXIS_LSTICK_X },
		{ L"AXIS_LSTICK_Y", VK_GP_AXIS_LSTICK_Y },
		{ L"AXIS_RSTICK_X", VK_GP_AXIS_RSTICK_X },
		{ L"AXIS_RSTICK_Y", VK_GP_AXIS_RSTICK_Y }
	};

	CInputGamepad::CInputGamepad() { }
	CInputGamepad::~CInputGamepad() { }

	void CInputGamepad::Initialize() { }

	void CInputGamepad::Reset()
	{
		m_buttonSet.reset();
	}

	void CInputGamepad::PostUpdate() { }
	void CInputGamepad::Release() { }

	//-----------------------------------------------------------------------------------------------
	// (De)registers.
	//-----------------------------------------------------------------------------------------------

	void CInputGamepad::RegisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding)
	{
		assert(binding.device == INPUT_DEVICE_GAMEPAD);
		const u64 hash = (static_cast<u64>(layoutHash) << 32) | binding.code;
		m_bindingMap[hash].insert({ bindingHash, binding });
	}

	void CInputGamepad::DeregisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding)
	{
		assert(binding.device == INPUT_DEVICE_GAMEPAD);
		const u64 hash = (static_cast<u64>(layoutHash) << 32) | binding.code;
		m_bindingMap[hash].erase(bindingHash);
	}

	//-----------------------------------------------------------------------------------------------
	// Modifiers.
	//-----------------------------------------------------------------------------------------------

	void CInputGamepad::SetButton(VK_GAMEPAD code, bool bPressed)
	{
		if(m_buttonSet[code] == bPressed) { return; }
		m_buttonSet[code].flip();

		const u64 hash = (static_cast<u64>(CInput::Instance().GetActiveLayout()->GetHash()) << 32) | code;
		auto elem = m_bindingMap.find(hash);
		if(elem != m_bindingMap.end())
		{
			for(auto& binding : elem->second)
			{
				InputCallbackData data { };
				data.bPressed = bPressed;
				data.pBinding = &binding.second;
				data.value = bPressed ? binding.second.scale : 0.0f;
				CInput::Instance().GetActiveLayout()->GetBindingSet(binding.second.setHash).callback(data);
			}
		}
	}

	void CInputGamepad::SetAxis(VK_GAMEPAD code, float value, float deadzone)
	{
		if(fabsf(value) < deadzone)
		{
			if(m_buttonSet[code]) { m_buttonSet[code].flip(); }
			value = 0.0f;
		}
		else
		{
			if(!m_buttonSet[code]) { m_buttonSet[code].flip(); }
			value = (fabsf(value) - deadzone) / (1.0f - deadzone) * Math::Sign(value);
		}

		
		const u64 hash = (static_cast<u64>(CInput::Instance().GetActiveLayout()->GetHash()) << 32) | code;
		auto elem = m_bindingMap.find(hash);
		if(elem != m_bindingMap.end())
		{
			for(auto& binding : elem->second)
			{
				InputCallbackData data { };
				data.bPressed = value;
				data.pBinding = &binding.second;
				data.value = value * binding.second.scale;
				CInput::Instance().GetActiveLayout()->GetBindingSet(binding.second.setHash).callback(data);
			}
		}
	}
};
