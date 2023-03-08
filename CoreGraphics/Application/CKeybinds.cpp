//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CKeybinds.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CKeybinds.h"
#include "CInput.h"
#include "CInputDeviceList.h"
#include "CInputKeyboard.h"
#include "CInputMouse.h"
#include "CInputGamepad.h"
#include <Math/CMathFNV.h>

namespace App
{
	const wchar_t CKeybinds::INPUT_KEY_MOVE_FORWARD[] = L"move_forward";
	const wchar_t CKeybinds::INPUT_KEY_MOVE_BACKWARD[] = L"move_backward";
	const wchar_t CKeybinds::INPUT_KEY_MOVE_LEFT[] = L"move_left";
	const wchar_t CKeybinds::INPUT_KEY_MOVE_RIGHT[] = L"move_right";

	const wchar_t CKeybinds::INPUT_KEY_LOOK_H[] = L"look_horizontal";
	const wchar_t CKeybinds::INPUT_KEY_LOOK_V[] = L"look_vertical";
	
	const wchar_t CKeybinds::INPUT_KEY_JUMP[] = L"jump";
	
	const wchar_t CKeybinds::INPUT_KEY_SELECT[] = L"select";

	CKeybinds::CKeybinds()
	{
	}

	CKeybinds::~CKeybinds()
	{
	}

	void CKeybinds::Initialize()
	{
		RegisterKeybinds();

		{ // Setup and read config file.
			Util::CConfigFile::Data data { };
			data.filename = m_configFilename;
			data.processConfig = std::bind(&CKeybinds::ProcessConfig, this, std::placeholders::_1, std::placeholders::_2);
			m_configFile.SetData(data);
			m_configFile.Parse();
		}

		CInput::Instance().SetKeybinds(this);
	}

	void CKeybinds::RegisterKeybinds()
	{
		// Movement.
		RegisterKeybinding(&m_inputHashDefault, 1, INPUT_KEY_MOVE_FORWARD, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_W), InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_UP));
		
		RegisterKeybinding(&m_inputHashDefault, 1, INPUT_KEY_MOVE_BACKWARD, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_S), InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_DOWN));
		
		RegisterKeybinding(&m_inputHashDefault, 1, INPUT_KEY_MOVE_LEFT, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_A), InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_LEFT));
		
		RegisterKeybinding(&m_inputHashDefault, 1, INPUT_KEY_MOVE_RIGHT, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_D), InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_RIGHT));
		
		// Look.
		RegisterKeybinding(&m_inputHashDefault, 1, INPUT_KEY_LOOK_H, nullptr, 
			InputBinding(INPUT_DEVICE_MOUSE, VM_MOUSE_DELTA_X), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&m_inputHashDefault, 1, INPUT_KEY_LOOK_V, nullptr, 
			InputBinding(INPUT_DEVICE_MOUSE, VM_MOUSE_DELTA_Y), InputBinding(INPUT_DEVICE_NONE, 0));

		// Actions.
		RegisterKeybinding(&m_inputHashDefault, 1, INPUT_KEY_JUMP, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_SPACE), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&m_inputHashDefault, 1, INPUT_KEY_SELECT, nullptr, 
			InputBinding(INPUT_DEVICE_MOUSE, VM_MOUSE_LEFT), InputBinding(INPUT_DEVICE_NONE, 0));
	}

	// Method for registering a key binding w/ the generic callback handler.
	void CKeybinds::RegisterKeybinding(const u32* pLayoutHashList, u32 layoutHashCount, const std::wstring& key, InputCallback func,
		const InputBinding& primary, const InputBinding& secondary)
	{
		CInput::Instance().RegisterBinding(pLayoutHashList, layoutHashCount,
			InputBindingSet(key, std::bind(&CKeybinds::Callback, this, std::placeholders::_1, func), primary, secondary)
		);
	}

	//-----------------------------------------------------------------------------------------------
	// Callback methods.
	//-----------------------------------------------------------------------------------------------
	
	void CKeybinds::Callback(const InputCallbackData& callback, InputCallback func)
	{
		if(func) func(callback);
		for(auto elem : m_processorList)
		{
			if(elem(callback.pBinding->setHash, callback))
			{
				break;
			}
		}
	}
		
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	void CKeybinds::ProcessConfig(const std::wstring& prop, const std::vector<std::vector<std::wstring>>& elemList)
	{
		std::wstring layout;
		size_t layoutOffset = prop.size();
		if(prop.back() == ']')
		{
			while(layoutOffset > 0 && prop[layoutOffset - 1] != '[')
			{
				--layoutOffset;
			}

			layout = prop.substr(layoutOffset, prop.size() - layoutOffset - 1);
		}

		auto ExtractBinding = [](const std::vector<std::wstring>& elems, InputBinding& binding) {
			if(elems.size() > 0) binding.device = INPUT_DEVICE_MAP.find(elems[0])->second;
			if(elems.size() > 1) 
			{
				switch(binding.device)
				{
					case INPUT_DEVICE_KEYBOARD:
						binding.code = CInputKeyboard::CODE_MAP.find(elems[1])->second;
						break;
					case INPUT_DEVICE_MOUSE:
						binding.code = CInputMouse::CODE_MAP.find(elems[1])->second;
						break;
					case INPUT_DEVICE_GAMEPAD:
						binding.code = CInputGamepad::CODE_MAP.find(elems[1])->second;
						break;
					default:
						break;
				}
			}
			
			if(elems.size() > 2) binding.modifier = INPUT_MODIFIER_MAP.find(elems[2])->second;
		};
		
		InputBinding primary(INPUT_DEVICE_NONE);
		InputBinding secondary(INPUT_DEVICE_NONE);

		if(elemList.size() > 0) ExtractBinding(elemList[0], primary);
		if(elemList.size() > 1) ExtractBinding(elemList[1], secondary);

		if(layoutOffset > 0 && layoutOffset != prop.size())
		{
			CInput::Instance().Rebind(Math::FNV1a_32(layout.c_str(), layout.size()), Math::FNV1a_32(prop.substr(0, layoutOffset - 1).c_str()), primary, secondary);
		}
		else
		{
			CInput::Instance().Rebind(Math::FNV1a_32(prop.c_str()), primary, secondary);
		}
	}
};
