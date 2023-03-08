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

		InputBindingSet Rebind(u32 layoutHash, u32 bindingHash, const InputBindingSet& bindingLast, const InputBinding& primary, const InputBinding& secondary, bool bKeysOnly = true)
		{
			Deregister(layoutHash, bindingHash, bindingLast);

			InputBindingSet newBinding = bindingLast;
			if(bKeysOnly)
			{
				newBinding.bindings[0].device = primary.device;
				newBinding.bindings[0].code = primary.code;
				newBinding.bindings[0].modifier = primary.modifier;

				newBinding.bindings[1].device = secondary.device;
				newBinding.bindings[1].code = secondary.code;
				newBinding.bindings[1].modifier = secondary.modifier;
			}
			else
			{
				newBinding.bindings[0] = primary;
				newBinding.bindings[1] = secondary;
			}

			Register(layoutHash, bindingHash, newBinding);
			return newBinding;
		}

		// Accessors.
		virtual inline const class CInputDevice* GetDevice(INPUT_DEVICE device) const { return nullptr; }

		virtual inline const class CInputKeyboard* GetKeyboard() const { return nullptr; }
		virtual inline const class CInputMouse* GetMouse() const { return nullptr; }
	};
};

#endif
