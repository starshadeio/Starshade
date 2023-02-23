//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInputDevice.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CINPUTDEVICE_H
#define CINPUTDEVICE_H

#include "CInputData.h"
#include <string>

namespace App
{
	class CInputDevice
	{
	protected:
		CInputDevice() { }
		virtual ~CInputDevice() { }
		CInputDevice(const CInputDevice&) = delete;
		CInputDevice(CInputDevice&&) = delete;
		CInputDevice& operator = (const CInputDevice&) = delete;
		CInputDevice& operator = (CInputDevice&&) = delete;

	public:
		virtual void Initialize() = 0;
		virtual void Reset() = 0;
		virtual void Update() = 0;
		virtual void PostUpdate() = 0;
		virtual void Release() = 0;

		virtual void RegisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding) = 0;
		virtual void DeregisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding) = 0;

		// Accessors.
		virtual inline const std::wstring& GetCodeName(u32 code) const = 0;
	};
};

#endif
