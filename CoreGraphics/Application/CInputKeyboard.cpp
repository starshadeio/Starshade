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
	const std::unordered_map<std::wstring, VK_KEYBOARD> CInputKeyboard::CODE_MAP = {
		{ L"LBUTTON", VK_KB_LBUTTON },
		{ L"RBUTTON", VK_KB_RBUTTON },
		{ L"MBUTTON", VK_KB_MBUTTON },
		{ L"XBUTTON1", VK_KB_XBUTTON1 },
		{ L"XBUTTON2", VK_KB_XBUTTON2 },
		{ L"CANCEL", VK_KB_CANCEL },
		{ L"BACK", VK_KB_BACK },
		{ L"TAB", VK_KB_TAB },
		{ L"CLEAR", VK_KB_CLEAR },
		{ L"RETURN", VK_KB_RETURN },
		{ L"SHIFT", VK_KB_SHIFT },
		{ L"CONTROL", VK_KB_CONTROL },
		{ L"MENU", VK_KB_MENU },
		{ L"PAUSE", VK_KB_PAUSE },
		{ L"CAPITAL", VK_KB_CAPITAL },
		{ L"KANA", VK_KB_KANA },
		{ L"JUNJA", VK_KB_JUNJA },
		{ L"KANJI", VK_KB_KANJI },
		{ L"FINAL", VK_KB_FINAL },
		{ L"IME_ON", VK_KB_IME_ON },
		{ L"IME_OFF", VK_KB_IME_OFF },
		{ L"ESCAPE", VK_KB_ESCAPE },
		{ L"CONVERT", VK_KB_CONVERT },
		{ L"NONCONVERT", VK_KB_NONCONVERT },
		{ L"ACCEPT", VK_KB_ACCEPT },
		{ L"MODECHANGE", VK_KB_MODECHANGE },
		{ L"SPACE", VK_KB_SPACE },
		{ L"PRIOR", VK_KB_PRIOR },
		{ L"NEXT", VK_KB_NEXT },
		{ L"END", VK_KB_END },
		{ L"HOME", VK_KB_HOME },
		{ L"LEFT", VK_KB_LEFT },
		{ L"UP", VK_KB_UP },
		{ L"RIGHT", VK_KB_RIGHT },
		{ L"DOWN", VK_KB_DOWN },
		{ L"SELECT", VK_KB_SELECT },
		{ L"PRINT", VK_KB_PRINT },
		{ L"EXECUTE", VK_KB_EXECUTE },
		{ L"SNAPSHOT", VK_KB_SNAPSHOT },
		{ L"INSERT", VK_KB_INSERT },
		{ L"DELETE", VK_KB_DELETE },
		{ L"HELP", VK_KB_HELP },
		{ L"0", VK_KB_0 },
		{ L"1", VK_KB_1 },
		{ L"2", VK_KB_2 },
		{ L"3", VK_KB_3 },
		{ L"4", VK_KB_4 },
		{ L"5", VK_KB_5 },
		{ L"6", VK_KB_6 },
		{ L"7", VK_KB_7 },
		{ L"8", VK_KB_8 },
		{ L"9", VK_KB_9 },
		{ L"A", VK_KB_A },
		{ L"B", VK_KB_B },
		{ L"C", VK_KB_C },
		{ L"D", VK_KB_D },
		{ L"E", VK_KB_E },
		{ L"F", VK_KB_F },
		{ L"G", VK_KB_G },
		{ L"H", VK_KB_H },
		{ L"I", VK_KB_I },
		{ L"J", VK_KB_J },
		{ L"K", VK_KB_K },
		{ L"L", VK_KB_L },
		{ L"M", VK_KB_M },
		{ L"N", VK_KB_N },
		{ L"O", VK_KB_O },
		{ L"P", VK_KB_P },
		{ L"Q", VK_KB_Q },
		{ L"R", VK_KB_R },
		{ L"S", VK_KB_S },
		{ L"T", VK_KB_T },
		{ L"U", VK_KB_U },
		{ L"V", VK_KB_V },
		{ L"W", VK_KB_W },
		{ L"X", VK_KB_X },
		{ L"Y", VK_KB_Y },
		{ L"Z", VK_KB_Z },
		{ L"LWIN", VK_KB_LWIN },
		{ L"RWIN", VK_KB_RWIN },
		{ L"APPS", VK_KB_APPS },
		{ L"SLEEP", VK_KB_SLEEP },
		{ L"NUMPAD0", VK_KB_NUMPAD0 },
		{ L"NUMPAD1", VK_KB_NUMPAD1 },
		{ L"NUMPAD2", VK_KB_NUMPAD2 },
		{ L"NUMPAD3", VK_KB_NUMPAD3 },
		{ L"NUMPAD4", VK_KB_NUMPAD4 },
		{ L"NUMPAD5", VK_KB_NUMPAD5 },
		{ L"NUMPAD6", VK_KB_NUMPAD6 },
		{ L"NUMPAD7", VK_KB_NUMPAD7 },
		{ L"NUMPAD8", VK_KB_NUMPAD8 },
		{ L"NUMPAD9", VK_KB_NUMPAD9 },
		{ L"MULTIPLY", VK_KB_MULTIPLY },
		{ L"ADD", VK_KB_ADD },
		{ L"SEPARATOR", VK_KB_SEPARATOR },
		{ L"SUBTRACT", VK_KB_SUBTRACT },
		{ L"DECIMAL", VK_KB_DECIMAL },
		{ L"DIVIDE", VK_KB_DIVIDE },
		{ L"F1", VK_KB_F1 },
		{ L"F2", VK_KB_F2 },
		{ L"F3", VK_KB_F3 },
		{ L"F4", VK_KB_F4 },
		{ L"F5", VK_KB_F5 },
		{ L"F6", VK_KB_F6 },
		{ L"F7", VK_KB_F7 },
		{ L"F8", VK_KB_F8 },
		{ L"F9", VK_KB_F9 },
		{ L"F10", VK_KB_F10 },
		{ L"F11", VK_KB_F11 },
		{ L"F12", VK_KB_F12 },
		{ L"F13", VK_KB_F13 },
		{ L"F14", VK_KB_F14 },
		{ L"F15", VK_KB_F15 },
		{ L"F16", VK_KB_F16 },
		{ L"F17", VK_KB_F17 },
		{ L"F18", VK_KB_F18 },
		{ L"F19", VK_KB_F19 },
		{ L"F20", VK_KB_F20 },
		{ L"F21", VK_KB_F21 },
		{ L"F22", VK_KB_F22 },
		{ L"F23", VK_KB_F23 },
		{ L"F24", VK_KB_F24 },
		{ L"NUMLOCK", VK_KB_NUMLOCK },
		{ L"SCROLL", VK_KB_SCROLL },
		{ L"LSHIFT", VK_KB_LSHIFT },
		{ L"RSHIFT", VK_KB_RSHIFT },
		{ L"LCONTROL", VK_KB_LCONTROL },
		{ L"RCONTROL", VK_KB_RCONTROL },
		{ L"LMENU", VK_KB_LMENU },
		{ L"RMENU", VK_KB_RMENU },
		{ L"BROWSER_BACK", VK_KB_BROWSER_BACK },
		{ L"BROWSER_FORWARD", VK_KB_BROWSER_FORWARD },
		{ L"BROWSER_REFRESH", VK_KB_BROWSER_REFRESH },
		{ L"BROWSER_STOP", VK_KB_BROWSER_STOP },
		{ L"BROWSER_SEARCH", VK_KB_BROWSER_SEARCH },
		{ L"BROWSER_FAVORITES", VK_KB_BROWSER_FAVORITES },
		{ L"BROWSER_HOME", VK_KB_BROWSER_HOME },
		{ L"VOLUME_MUTE", VK_KB_VOLUME_MUTE },
		{ L"VOLUME_DOWN", VK_KB_VOLUME_DOWN },
		{ L"VOLUME_UP", VK_KB_VOLUME_UP },
		{ L"MEDIA_NEXT_TRACK", VK_KB_MEDIA_NEXT_TRACK },
		{ L"MEDIA_PREV_TRACK", VK_KB_MEDIA_PREV_TRACK },
		{ L"MEDIA_STOP", VK_KB_MEDIA_STOP },
		{ L"MEDIA_PLAY_PAUSE", VK_KB_MEDIA_PLAY_PAUSE },
		{ L"LAUNCH_MAIL", VK_KB_LAUNCH_MAIL },
		{ L"LAUNCH_MEDIA_SELECT", VK_KB_LAUNCH_MEDIA_SELECT },
		{ L"LAUNCH_APP1", VK_KB_LAUNCH_APP1 },
		{ L"LAUNCH_APP2", VK_KB_LAUNCH_APP2 },
		{ L"PLUS", VK_KB_OEM_PLUS },
		{ L"COMMA", VK_KB_OEM_COMMA },
		{ L"MINUS", VK_KB_OEM_MINUS },
		{ L"PERIOD", VK_KB_OEM_PERIOD },
		{ L"CLEAR", VK_KB_OEM_CLEAR },
		{ L"SEMICOLON", VK_KB_OEM_SEMICOLON },
		{ L"FORWARD_SLASH", VK_KB_OEM_FORWARD_SLASH },
		{ L"TILDE", VK_KB_OEM_TILDE },
		{ L"LBRACKET", VK_KB_OEM_LBRACKET },
		{ L"BACK_SLASH", VK_KB_OEM_BACK_SLASH },
		{ L"RBRACKET", VK_KB_OEM_RBRACKET },
		{ L"QUOTE", VK_KB_OEM_QUOTE },
		{ L"MISC", VK_KB_OEM_MISC },
		{ L"OEM_102", VK_KB_OEM_102 },
		{ L"PROCESSKEY", VK_KB_PROCESSKEY },
		{ L"PACKET", VK_KB_PACKET },
		{ L"ATTN", VK_KB_ATTN },
		{ L"CRSEL", VK_KB_CRSEL },
		{ L"EXSEL", VK_KB_EXSEL },
		{ L"EREOF", VK_KB_EREOF },
		{ L"PLAY", VK_KB_PLAY },
		{ L"ZOOM", VK_KB_ZOOM },
		{ L"NONAME", VK_KB_NONAME },
		{ L"PA1", VK_KB_PA1 }
	};

	CInputKeyboard::CInputKeyboard() :
		m_bAllowModifiers(true),
		m_modifierFlag(INPUT_MODIFIER_FLAG_NONE) {
	}

	CInputKeyboard::~CInputKeyboard() { }

	void CInputKeyboard::Initialize() { }

	void CInputKeyboard::Reset()
	{
		m_keySet.reset();
		m_modifierFlag = INPUT_MODIFIER_FLAG_NONE;
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
