//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CWinDeviceList.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CWINDEVICELIST_H
#define CWINDEVICELIST_H

#include "CInputData.h"
#include "CInputDeviceList.h"
#include "CInputKeyboard.h"
#include "CInputMouse.h"
#include "CWinGamepad.h"
#include <Globals/CGlobals.h>
#include <Windows.h>

namespace App
{
	class CWinDeviceList : public CInputDeviceList
	{
	public:
		CWinDeviceList();
		~CWinDeviceList();
		CWinDeviceList(const CWinDeviceList&) = delete;
		CWinDeviceList(CWinDeviceList&&) = delete;
		CWinDeviceList& operator = (const CWinDeviceList&) = delete;
		CWinDeviceList& operator = (CWinDeviceList&&) = delete;

		void Initialize(class CPanel* pPanel) final;
		void Reset() final;
		void Update() final;
		void PostUpdate() final;
		void Release() final;

		void Register(u32 layoutHash, u32 bindingHash, const InputBindingSet& bindingSet) final;
		void Deregister(u32 layoutHash, u32 bindingHash, const InputBindingSet& bindingSet) final;

		// Accessors.
		inline const class CInputDevice* GetDevice(INPUT_DEVICE device) const final { return m_pInputDevice[device]; }

		inline const class CInputKeyboard* GetKeyboard() const final { return &m_keyboard; }
		inline const class CInputMouse* GetMouse() const final { return &m_mouse; }

	private:
		void GetUserSettings();

		void MessageHandler(const MSG& msg);

	private:
		u8 m_pRawInputData[256];

		CInputKeyboard m_keyboard;
		CInputMouse m_mouse;
		CWinGamepad m_gamepad;
		CInputDevice* m_pInputDevice[INPUT_DEVICE_COUNT];

		class CWinPanel* m_pWinPanel;
	};
};

#endif
