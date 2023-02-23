//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInputDeviceList.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CINPUTDEVICELIST_H
#define CINPUTDEVICELIST_H

#include "CInputData.h"

namespace App
{
	class CInputDeviceList
	{
	public:
		CInputDeviceList() { }
		virtual ~CInputDeviceList() { }
		CInputDeviceList(const CInputDeviceList&) = delete;
		CInputDeviceList(CInputDeviceList&&) = delete;
		CInputDeviceList& operator = (const CInputDeviceList&) = delete;
		CInputDeviceList& operator = (CInputDeviceList&&) = delete;

		virtual void Initialize(class CPanel* pPanel) = 0;
		virtual void Reset() = 0;
		virtual void Update() = 0;
		virtual void PostUpdate() = 0;
		virtual void Release() = 0;

		virtual void Register(u32 layoutHash, u32 bindingHash, const InputBindingSet& bindingSet) = 0;
		virtual void Deregister(u32 layoutHash, u32 bindingHash, const InputBindingSet& bindingSet) = 0;

		// Accessors.
		virtual inline const class CInputDevice* GetDevice(INPUT_DEVICE device) const { return nullptr; }

		virtual inline const class CInputKeyboard* GetKeyboard() const { return nullptr; }
		virtual inline const class CInputMouse* GetMouse() const { return nullptr; }
	};
};

#endif
