//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInputKeyboard.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CInputKeyboard.h"
#include "CInputLayout.h"
#include "CInput.h"
#include <cassert>

namespace App
{
	CInputKeyboard::CInputKeyboard() :
		m_bAllowModifiers(true),
		m_modifierFlag(INPUT_MODIFIER_FLAG_NONE) {
	}

	CInputKeyboard::~CInputKeyboard() { }

	void CInputKeyboard::Initialize() { }

	void CInputKeyboard::Reset()
	{
		m_keySet.reset();
	}

	void CInputKeyboard::Update() { }
	void CInputKeyboard::PostUpdate() { }
	void CInputKeyboard::Release() { }

	//-----------------------------------------------------------------------------------------------
	// (De)registers.
	//-----------------------------------------------------------------------------------------------

	void CInputKeyboard::RegisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding)
	{
		assert(binding.device == INPUT_DEVICE_KEYBOARD);
		const u64 hash = (static_cast<u64>(layoutHash) << 32) | binding.code;
		m_bindingMap[InputModifierFlagToType(binding.modifier)][hash].insert({ bindingHash, binding });
	}

	void CInputKeyboard::DeregisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding)
	{
		assert(binding.device == INPUT_DEVICE_KEYBOARD);
		const u64 hash = (static_cast<u64>(layoutHash) << 32) | binding.code;
		m_bindingMap[InputModifierFlagToType(binding.modifier)][hash].erase(bindingHash);
	}

	//-----------------------------------------------------------------------------------------------
	// Modifiers.
	//-----------------------------------------------------------------------------------------------

	void CInputKeyboard::SetKey(VK_KEYBOARD code, bool bPressed)
	{
		if(m_keySet[code] == bPressed) { return; }
		m_keySet[code].flip();

		if(m_bAllowModifiers)
		{
			if(bPressed)
			{
				if(code == VK_KB_CONTROL || code == VK_KB_LCONTROL || code == VK_KB_RCONTROL) { m_modifierFlag |= INPUT_MODIFIER_FLAG_CTRL; }
				if(code == VK_KB_MENU || code == VK_KB_LMENU || code == VK_KB_RMENU) { m_modifierFlag |= INPUT_MODIFIER_FLAG_ALT; }
				if(code == VK_KB_SHIFT || code == VK_KB_LSHIFT || code == VK_KB_RSHIFT) { m_modifierFlag |= INPUT_MODIFIER_FLAG_SHIFT; }
			}
			else
			{
				if(code == VK_KB_CONTROL || code == VK_KB_LCONTROL || code == VK_KB_RCONTROL) { m_modifierFlag &= ~INPUT_MODIFIER_FLAG_CTRL; }
				if(code == VK_KB_MENU || code == VK_KB_LMENU || code == VK_KB_RMENU) { m_modifierFlag &= ~INPUT_MODIFIER_FLAG_ALT; }
				if(code == VK_KB_SHIFT || code == VK_KB_LSHIFT || code == VK_KB_RSHIFT) { m_modifierFlag &= ~INPUT_MODIFIER_FLAG_SHIFT; }
			}
		}
		else
		{
			m_modifierFlag = INPUT_MODIFIER_FLAG_NONE;
		}

		const INPUT_MODIFIER_TYPE modifierType = InputModifierFlagToType(m_modifierFlag);
		
		const u64 hash = (static_cast<u64>(CInput::Instance().GetActiveLayout()->GetHash()) << 32) | code;
		auto elem = m_bindingMap[modifierType].find(hash);
		if(elem != m_bindingMap[modifierType].end())
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
};
