//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CWinDeviceList.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef PLATFORM_WINDOWS

#include "CWinDeviceList.h"
#include "CInputKeyboard.h"
#include "CWinPlatform.h"
#include "CWinPanel.h"
#include "../Factory/CFactory.h"
#include <Utilities/CDebugError.h>
#include <cassert>
#include <unordered_map>

namespace App
{
	static const float MOUSE_WHEEL_SCALE = 1.0f / WHEEL_DELTA;

	static const std::unordered_map<u32, VK_KEYBOARD> VK_MAP = {
		{ VK_LBUTTON, VK_KB_LBUTTON },
		{ VK_RBUTTON, VK_KB_RBUTTON },
		{ VK_MBUTTON, VK_KB_MBUTTON },
		{ VK_XBUTTON1, VK_KB_XBUTTON1 },
		{ VK_XBUTTON2, VK_KB_XBUTTON2 },
		{ VK_CANCEL, VK_KB_CANCEL },
		{ VK_BACK, VK_KB_BACK },
		{ VK_TAB, VK_KB_TAB },
		{ VK_CLEAR, VK_KB_CLEAR },
		{ VK_RETURN, VK_KB_RETURN },
		{ VK_SHIFT, VK_KB_SHIFT },
		{ VK_CONTROL, VK_KB_CONTROL },
		{ VK_MENU, VK_KB_MENU },
		{ VK_PAUSE, VK_KB_PAUSE },
		{ VK_CAPITAL, VK_KB_CAPITAL },
		{ VK_KANA, VK_KB_KANA },
		{ VK_JUNJA, VK_KB_JUNJA },
		{ VK_KANJI, VK_KB_KANJI },
		{ VK_FINAL, VK_KB_FINAL },
		{ VK_IME_ON, VK_KB_IME_ON },
		{ VK_IME_OFF, VK_KB_IME_OFF },
		{ VK_ESCAPE, VK_KB_ESCAPE },
		{ VK_CONVERT, VK_KB_CONVERT },
		{ VK_NONCONVERT, VK_KB_NONCONVERT },
		{ VK_ACCEPT, VK_KB_ACCEPT },
		{ VK_MODECHANGE, VK_KB_MODECHANGE },
		{ VK_SPACE, VK_KB_SPACE },
		{ VK_PRIOR, VK_KB_PRIOR },
		{ VK_NEXT, VK_KB_NEXT },
		{ VK_END, VK_KB_END },
		{ VK_HOME, VK_KB_HOME },
		{ VK_LEFT, VK_KB_LEFT },
		{ VK_UP, VK_KB_UP },
		{ VK_RIGHT, VK_KB_RIGHT },
		{ VK_DOWN, VK_KB_DOWN },
		{ VK_SELECT, VK_KB_SELECT },
		{ VK_PRINT, VK_KB_PRINT },
		{ VK_EXECUTE, VK_KB_EXECUTE },
		{ VK_SNAPSHOT, VK_KB_SNAPSHOT },
		{ VK_INSERT, VK_KB_INSERT },
		{ VK_DELETE, VK_KB_DELETE },
		{ VK_HELP, VK_KB_HELP },
		{ 0x30, VK_KB_0 },
		{ 0x31, VK_KB_1 },
		{ 0x32, VK_KB_2 },
		{ 0x33, VK_KB_3 },
		{ 0x34, VK_KB_4 },
		{ 0x35, VK_KB_5 },
		{ 0x36, VK_KB_6 },
		{ 0x37, VK_KB_7 },
		{ 0x38, VK_KB_8 },
		{ 0x39, VK_KB_9 },
		{ 0x41, VK_KB_A },
		{ 0x42, VK_KB_B },
		{ 0x43, VK_KB_C },
		{ 0x44, VK_KB_D },
		{ 0x45, VK_KB_E },
		{ 0x46, VK_KB_F },
		{ 0x47, VK_KB_G },
		{ 0x48, VK_KB_H },
		{ 0x49, VK_KB_I },
		{ 0x4A, VK_KB_J },
		{ 0x4B, VK_KB_K },
		{ 0x4C, VK_KB_L },
		{ 0x4D, VK_KB_M },
		{ 0x4E, VK_KB_N },
		{ 0x4F, VK_KB_O },
		{ 0x50, VK_KB_P },
		{ 0x51, VK_KB_Q },
		{ 0x52, VK_KB_R },
		{ 0x53, VK_KB_S },
		{ 0x54, VK_KB_T },
		{ 0x55, VK_KB_U },
		{ 0x56, VK_KB_V },
		{ 0x57, VK_KB_W },
		{ 0x58, VK_KB_X },
		{ 0x59, VK_KB_Y },
		{ 0x5A, VK_KB_Z },
		{ VK_LWIN, VK_KB_LWIN },
		{ VK_RWIN, VK_KB_RWIN },
		{ VK_APPS, VK_KB_APPS },
		{ VK_SLEEP, VK_KB_SLEEP },
		{ VK_NUMPAD0, VK_KB_NUMPAD0 },
		{ VK_NUMPAD1, VK_KB_NUMPAD1 },
		{ VK_NUMPAD2, VK_KB_NUMPAD2 },
		{ VK_NUMPAD3, VK_KB_NUMPAD3 },
		{ VK_NUMPAD4, VK_KB_NUMPAD4 },
		{ VK_NUMPAD5, VK_KB_NUMPAD5 },
		{ VK_NUMPAD6, VK_KB_NUMPAD6 },
		{ VK_NUMPAD7, VK_KB_NUMPAD7 },
		{ VK_NUMPAD8, VK_KB_NUMPAD8 },
		{ VK_NUMPAD9, VK_KB_NUMPAD9 },
		{ VK_MULTIPLY, VK_KB_MULTIPLY },
		{ VK_ADD, VK_KB_ADD },
		{ VK_SEPARATOR, VK_KB_SEPARATOR },
		{ VK_SUBTRACT, VK_KB_SUBTRACT },
		{ VK_DECIMAL, VK_KB_DECIMAL },
		{ VK_DIVIDE, VK_KB_DIVIDE },
		{ VK_F1, VK_KB_F1 },
		{ VK_F2, VK_KB_F2 },
		{ VK_F3, VK_KB_F3 },
		{ VK_F4, VK_KB_F4 },
		{ VK_F5, VK_KB_F5 },
		{ VK_F6, VK_KB_F6 },
		{ VK_F7, VK_KB_F7 },
		{ VK_F8, VK_KB_F8 },
		{ VK_F9, VK_KB_F9 },
		{ VK_F10, VK_KB_F10 },
		{ VK_F11, VK_KB_F11 },
		{ VK_F12, VK_KB_F12 },
		{ VK_F13, VK_KB_F13 },
		{ VK_F14, VK_KB_F14 },
		{ VK_F15, VK_KB_F15 },
		{ VK_F16, VK_KB_F16 },
		{ VK_F17, VK_KB_F17 },
		{ VK_F18, VK_KB_F18 },
		{ VK_F19, VK_KB_F19 },
		{ VK_F20, VK_KB_F20 },
		{ VK_F21, VK_KB_F21 },
		{ VK_F22, VK_KB_F22 },
		{ VK_F23, VK_KB_F23 },
		{ VK_F24, VK_KB_F24 },
		{ VK_NUMLOCK, VK_KB_NUMLOCK },
		{ VK_SCROLL, VK_KB_SCROLL },
		{ VK_LSHIFT, VK_KB_LSHIFT },
		{ VK_RSHIFT, VK_KB_RSHIFT },
		{ VK_LCONTROL, VK_KB_LCONTROL },
		{ VK_RCONTROL, VK_KB_RCONTROL },
		{ VK_LMENU, VK_KB_LMENU },
		{ VK_RMENU, VK_KB_RMENU },
		{ VK_BROWSER_BACK, VK_KB_BROWSER_BACK },
		{ VK_BROWSER_FORWARD, VK_KB_BROWSER_FORWARD },
		{ VK_BROWSER_REFRESH, VK_KB_BROWSER_REFRESH },
		{ VK_BROWSER_STOP, VK_KB_BROWSER_STOP },
		{ VK_BROWSER_SEARCH, VK_KB_BROWSER_SEARCH },
		{ VK_BROWSER_FAVORITES, VK_KB_BROWSER_FAVORITES },
		{ VK_BROWSER_HOME, VK_KB_BROWSER_HOME },
		{ VK_VOLUME_MUTE, VK_KB_VOLUME_MUTE },
		{ VK_VOLUME_DOWN, VK_KB_VOLUME_DOWN },
		{ VK_VOLUME_UP, VK_KB_VOLUME_UP },
		{ VK_MEDIA_NEXT_TRACK, VK_KB_MEDIA_NEXT_TRACK },
		{ VK_MEDIA_PREV_TRACK, VK_KB_MEDIA_PREV_TRACK },
		{ VK_MEDIA_STOP, VK_KB_MEDIA_STOP },
		{ VK_MEDIA_PLAY_PAUSE, VK_KB_MEDIA_PLAY_PAUSE },
		{ VK_LAUNCH_MAIL, VK_KB_LAUNCH_MAIL },
		{ VK_LAUNCH_MEDIA_SELECT, VK_KB_LAUNCH_MEDIA_SELECT },
		{ VK_LAUNCH_APP1, VK_KB_LAUNCH_APP1 },
		{ VK_LAUNCH_APP2, VK_KB_LAUNCH_APP2 },
		{ VK_OEM_PLUS, VK_KB_OEM_PLUS },
		{ VK_OEM_COMMA, VK_KB_OEM_COMMA },
		{ VK_OEM_MINUS, VK_KB_OEM_MINUS },
		{ VK_OEM_PERIOD, VK_KB_OEM_PERIOD },
		{ VK_OEM_CLEAR, VK_KB_OEM_CLEAR },
		{ VK_OEM_1, VK_KB_OEM_1 },
		{ VK_OEM_2, VK_KB_OEM_2 },
		{ VK_OEM_3, VK_KB_OEM_3 },
		{ VK_OEM_4, VK_KB_OEM_4 },
		{ VK_OEM_5, VK_KB_OEM_5 },
		{ VK_OEM_6, VK_KB_OEM_6 },
		{ VK_OEM_7, VK_KB_OEM_7 },
		{ VK_OEM_8, VK_KB_OEM_8 },
		{ VK_OEM_102, VK_KB_OEM_102 },
		{ VK_PROCESSKEY, VK_KB_PROCESSKEY },
		{ VK_PACKET, VK_KB_PACKET },
		{ VK_ATTN, VK_KB_ATTN },
		{ VK_CRSEL, VK_KB_CRSEL },
		{ VK_EXSEL, VK_KB_EXSEL },
		{ VK_EREOF, VK_KB_EREOF },
		{ VK_PLAY, VK_KB_PLAY },
		{ VK_ZOOM, VK_KB_ZOOM },
		{ VK_NONAME, VK_KB_NONAME },
		{ VK_PA1, VK_KB_PA1 },
	};

	CWinDeviceList::CWinDeviceList() : m_pWinPanel(nullptr)
	{
		m_pInputDevice[INPUT_DEVICE_KEYBOARD] = &m_keyboard;
		m_pInputDevice[INPUT_DEVICE_MOUSE] = &m_mouse;
		m_pInputDevice[INPUT_DEVICE_GAMEPAD] = &m_gamepad;
	}

	CWinDeviceList::~CWinDeviceList() { }

	void CWinDeviceList::Initialize(CPanel* pPanel)
	{
		RAWINPUTDEVICE Rid[2];

		m_pWinPanel = dynamic_cast<CWinPanel*>(pPanel);

		// Mouse.
		Rid[0].usUsagePage = 0x01;
		Rid[0].usUsage = 0x02;
		Rid[0].dwFlags = RIDEV_INPUTSINK;
		Rid[0].hwndTarget = m_pWinPanel->GetWindowHandle();

		// Keyboard.
		Rid[1].usUsagePage = 0x01;
		Rid[1].usUsage = 0x06;
		Rid[1].dwFlags = RIDEV_INPUTSINK;
		Rid[1].hwndTarget = m_pWinPanel->GetWindowHandle();

		ASSERT_R(RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])));

		for(int i = 0; i < INPUT_DEVICE_COUNT; ++i)
		{
			m_pInputDevice[i]->Initialize();
		}

		dynamic_cast<CWinPlatform*>(CFactory::Instance().GetPlatform())->
			AddMessageCallback(std::bind(&CWinDeviceList::MessageHandler, this, std::placeholders::_1));

		GetUserSettings();
	}

	void CWinDeviceList::Reset()
	{
		for(int i = 0; i < INPUT_DEVICE_COUNT; ++i)
		{
			m_pInputDevice[i]->Reset();
		}
	}

	void CWinDeviceList::Update()
	{
		{ // Calcualte mouse position.
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(m_pWinPanel->GetWindowHandle(), &pt);
			m_mouse.SetMousePosition(Math::Vector2(static_cast<float>(pt.x), static_cast<float>(pt.y)));
		}

		for(int i = 0; i < INPUT_DEVICE_COUNT; ++i)
		{
			m_pInputDevice[i]->Update();
		}

		m_gamepad.Update();
	}

	void CWinDeviceList::PostUpdate()
	{
		for(int i = 0; i < INPUT_DEVICE_COUNT; ++i)
		{
			m_pInputDevice[i]->PostUpdate();
		}

		m_gamepad.PostUpdate();
	}

	void CWinDeviceList::Release()
	{
		for(int i = 0; i < INPUT_DEVICE_COUNT; ++i)
		{
			m_pInputDevice[i]->Release();
		}
	}

	void CWinDeviceList::Register(u32 layoutHash, u32 bindingHash, const InputBindingSet& bindingSet)
	{
		for(int i = 0; i < 2; ++i)
		{
			switch(bindingSet.bindings[i].device)
			{
				case INPUT_DEVICE_KEYBOARD:
					m_pInputDevice[INPUT_DEVICE_KEYBOARD]->RegisterBinding(layoutHash, bindingHash, bindingSet.bindings[i]);
					break;
				case INPUT_DEVICE_MOUSE:
					m_pInputDevice[INPUT_DEVICE_MOUSE]->RegisterBinding(layoutHash, bindingHash, bindingSet.bindings[i]);
					break;
				case INPUT_DEVICE_GAMEPAD:
					m_pInputDevice[INPUT_DEVICE_GAMEPAD]->RegisterBinding(layoutHash, bindingHash, bindingSet.bindings[i]);
					break;
				default:
					break;
			}
		}
	}

	void CWinDeviceList::Deregister(u32 layoutHash, u32 bindingHash, const InputBindingSet& bindingSet)
	{
		for(int i = 0; i < 2; ++i)
		{
			switch(bindingSet.bindings[i].device)
			{
				case INPUT_DEVICE_KEYBOARD:
					m_pInputDevice[INPUT_DEVICE_KEYBOARD]->DeregisterBinding(layoutHash, bindingHash, bindingSet.bindings[i]);
					break;
				case INPUT_DEVICE_MOUSE:
					m_pInputDevice[INPUT_DEVICE_MOUSE]->DeregisterBinding(layoutHash, bindingHash, bindingSet.bindings[i]);
					break;
				case INPUT_DEVICE_GAMEPAD:
					m_pInputDevice[INPUT_DEVICE_GAMEPAD]->DeregisterBinding(layoutHash, bindingHash, bindingSet.bindings[i]);
					break;
				default:
					break;
			}
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Message Handler.
	//-----------------------------------------------------------------------------------------------

	void CWinDeviceList::MessageHandler(const MSG& msg)
	{
		switch(msg.message)
		{
			case WM_SETTINGCHANGE:
				GetUserSettings();
				break;
				// Make sure 'CS_DBLCLKS' is defined in the 'WNDCLASSEX' style variable
				// to enable double clicking.
			case WM_LBUTTONDBLCLK:
				m_mouse.SetMouseButton(VM_MOUSE_DBL_LEFT, true);
				break;
			case WM_RBUTTONDBLCLK:
				m_mouse.SetMouseButton(VM_MOUSE_DBL_RIGHT, true);
				break;
			case WM_MBUTTONDBLCLK:
				m_mouse.SetMouseButton(VM_MOUSE_DBL_MIDDLE, true);
				break;
			case WM_MOUSEWHEEL:
			{
				float wheel = static_cast<float>(GET_WHEEL_DELTA_WPARAM(msg.wParam)) * MOUSE_WHEEL_SCALE;
				m_mouse.SetMouseWheel(wheel);
				m_mouse.SetMouseButton(VM_MOUSE_WHEEL_UP, wheel > 0.0f);
				m_mouse.SetMouseButton(VM_MOUSE_WHEEL_DOWN, wheel < 0.0f);
			} break;
			case WM_INPUT:
			{ // Update with raw input.
				u32 size = 0;

				u32 ret = GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));
				assert(ret >= 0);
				if(size == 0) break;

				ret = GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, m_pRawInputData, &size, sizeof(RAWINPUTHEADER));
				assert(ret >= 0);
				RAWINPUT* pRawInput = (RAWINPUT*)m_pRawInputData;

				if(pRawInput->header.dwType == RIM_TYPEKEYBOARD)
				{
					// Raw keystroke activated.
					const bool bPressed = (pRawInput->data.keyboard.Flags & 1) ? false : true;
					if(!bPressed || m_pWinPanel->InFocus())
					{
						const auto& elem = VK_MAP.find((u32)pRawInput->data.keyboard.VKey);
						if(elem != VK_MAP.end())
						{
							m_keyboard.SetKey(elem->second, bPressed);
						}
					}
				}
				else if(pRawInput->header.dwType == RIM_TYPEMOUSE)
				{
					// Raw mouse motion?
					if(m_pWinPanel->InFocus())
					{
						m_mouse.SetMouseDelta(Math::Vector2(static_cast<float>(pRawInput->data.mouse.lLastX), static_cast<float>(pRawInput->data.mouse.lLastY)));
					}

					if(pRawInput->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
					{
						if(m_pWinPanel->IsHovered()) m_mouse.SetMouseButton(VM_MOUSE_LEFT, true);

					}
					else if(pRawInput->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
					{
						m_mouse.SetMouseButton(VM_MOUSE_LEFT, false);

					}
					else if(pRawInput->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
					{
						if(m_pWinPanel->IsHovered()) m_mouse.SetMouseButton(VM_MOUSE_RIGHT, true);

					}
					else if(pRawInput->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
					{
						m_mouse.SetMouseButton(VM_MOUSE_RIGHT, false);

					}
					else if(pRawInput->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
					{
						if(m_pWinPanel->IsHovered()) m_mouse.SetMouseButton(VM_MOUSE_MIDDLE, true);

					}
					else if(pRawInput->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
					{
						m_mouse.SetMouseButton(VM_MOUSE_MIDDLE, false);

					}
					else if(pRawInput->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
					{
						if(m_pWinPanel->IsHovered()) m_mouse.SetMouseButton(VM_MOUSE_4, true);

					}
					else if(pRawInput->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
					{
						m_mouse.SetMouseButton(VM_MOUSE_4, false);

					}
					else if(pRawInput->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
					{
						if(m_pWinPanel->IsHovered()) m_mouse.SetMouseButton(VM_MOUSE_5, true);

					}
					else if(pRawInput->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
					{
						m_mouse.SetMouseButton(VM_MOUSE_5, false);

					}
					else if(pRawInput->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
					{
						if(m_pWinPanel->InFocus())
						{
							const float wheelDelta = (float)(short)pRawInput->data.mouse.usButtonData * MOUSE_WHEEL_SCALE;
							m_mouse.SetMouseWheel(wheelDelta);
							m_mouse.SetMouseButton(VM_MOUSE_WHEEL_UP, wheelDelta > 0.0f);
							m_mouse.SetMouseButton(VM_MOUSE_WHEEL_DOWN, wheelDelta < 0.0f);
						}
					}

				}
				else if(pRawInput->header.dwType == RIM_TYPEHID)
				{
				} break;
			}
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	// Method for extracting needed user settings.
	void CWinDeviceList::GetUserSettings() { }
};

#endif
