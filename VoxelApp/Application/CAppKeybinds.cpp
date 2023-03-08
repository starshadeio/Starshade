//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel App
//
// File: Application/CAppKeybinds.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAppKeybinds.h"
#include "CAppData.h"
#include <Application/CInput.h>
#include <Application/CInputDeviceList.h>
#include <Application/CInputKeyboard.h>
#include <Application/CInputMouse.h>
#include <Application/CInputGamepad.h>
#include <Application/CCommandManager.h>
#include <Application/CPlatform.h>
#include <Graphics/CGraphicsAPI.h>
#include <Factory/CFactory.h>
#include <Resources/CResourceManager.h>
#include <chrono>
#include <sstream>

namespace App
{
	const wchar_t CAppKeybinds::INPUT_KEY_QUIT[] = L"quit";
	const wchar_t CAppKeybinds::INPUT_KEY_SCREENSHOT[] = L"screenshot";

	CAppKeybinds::CAppKeybinds()
	{
		m_inputHashDefault = INPUT_HASH_LAYOUT_PLAYER;
	}

	CAppKeybinds::~CAppKeybinds()
	{
	}
	
	void CAppKeybinds::RegisterKeybinds()
	{
		m_configFilename = *reinterpret_cast<std::wstring*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_CONFIG, Math::FNV1a_64(L"CONFIG_KEYBINDS_APP")));

		CKeybinds::RegisterKeybinds();

		RegisterKeybinding(&INPUT_HASH_LAYOUT_PLAYER, 1, INPUT_KEY_QUIT, std::bind(&CAppKeybinds::Quit, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_ESCAPE), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_PLAYER, 1, INPUT_KEY_SCREENSHOT, std::bind(&CAppKeybinds::Screenshot, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_F5), InputBinding(INPUT_DEVICE_NONE, 0));
	}
		
	//-----------------------------------------------------------------------------------------------
	// Input callbacks.
	//-----------------------------------------------------------------------------------------------
	
	// Movement.
	void CAppKeybinds::Quit(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CFactory::Instance().GetPlatform()->Quit();
		}
	}

	void CAppKeybinds::Screenshot(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			const std::time_t result = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

			static char buffer[32];
			ctime_s(buffer, 32, &result);

			std::wstringstream wss;
			wss << L"./Screenshots/";
			for(size_t i = 0; buffer[i] != '\n'; ++i)
			{
				if(buffer[i] == ' ' || buffer[i] == ':') wss << L'-';
				else wss << wchar_t(buffer[i]);
			}

			wss << L".png";

			CFactory::Instance().GetGraphicsAPI()->Screenshot(wss.str().c_str());
		}
	}
};
