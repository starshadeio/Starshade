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
