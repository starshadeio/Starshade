//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInputData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CINPUTDATA_H
#define CINPUTDATA_H

#include <Globals/CGlobals.h>
#include <Math/CMathFNV.h>
#include <Utilities/CMacroUtil.h>
#include <functional>
#include <string>
#include <unordered_map>

namespace App
{
	enum INPUT_DEVICE
	{
		INPUT_DEVICE_NONE = -1,
		INPUT_DEVICE_KEYBOARD,
		INPUT_DEVICE_MOUSE,
		INPUT_DEVICE_GAMEPAD,
		INPUT_DEVICE_COUNT,
	};

	const std::unordered_map<std::wstring, INPUT_DEVICE> INPUT_DEVICE_MAP = {
		{ L"NONE", INPUT_DEVICE_NONE },
		{ L"KEYBOARD", INPUT_DEVICE_KEYBOARD },
		{ L"MOUSE", INPUT_DEVICE_MOUSE },
		{ L"GAMEPAD", INPUT_DEVICE_GAMEPAD },
	};

	//-----------------------------------------------------------------------------------------------
	// Modifier data.
	//-----------------------------------------------------------------------------------------------

	enum INPUT_MODIFIER_FLAG
	{
		INPUT_MODIFIER_FLAG_NONE = 0x0,
		INPUT_MODIFIER_FLAG_CTRL = 0x1,
		INPUT_MODIFIER_FLAG_ALT = 0x2,
		INPUT_MODIFIER_FLAG_SHIFT = 0x4,
		INPUT_MODIFIER_FLAG_CTRL_ALT = INPUT_MODIFIER_FLAG_CTRL | INPUT_MODIFIER_FLAG_ALT,
		INPUT_MODIFIER_FLAG_CTRL_SHIFT = INPUT_MODIFIER_FLAG_CTRL | INPUT_MODIFIER_FLAG_SHIFT,
		INPUT_MODIFIER_FLAG_ALT_SHIFT = INPUT_MODIFIER_FLAG_ALT | INPUT_MODIFIER_FLAG_SHIFT,
		INPUT_MODIFIER_FLAG_CTRL_ALT_SHIFT = INPUT_MODIFIER_FLAG_CTRL | INPUT_MODIFIER_FLAG_ALT | INPUT_MODIFIER_FLAG_SHIFT,
	};

	DEFINE_ENUMERATOR_FLAG_OPERATORS(INPUT_MODIFIER_FLAG);

	const std::wstring INPUT_MODIFIER_STR[] = {
		L"Ctrl",
		L"Alt",
		L"Shift",
	};

	enum INPUT_MODIFIER_TYPE
	{
		INPUT_MODIFIER_TYPE_NONE,
		INPUT_MODIFIER_TYPE_CTRL,
		INPUT_MODIFIER_TYPE_ALT,
		INPUT_MODIFIER_TYPE_SHIFT,
		INPUT_MODIFIER_TYPE_CTRL_ALT,
		INPUT_MODIFIER_TYPE_CTRL_SHIFT,
		INPUT_MODIFIER_TYPE_ALT_SHIFT,
		INPUT_MODIFIER_TYPE_CTRL_ALT_SHIFT,
		INPUT_MODIFIER_TYPE_COUNT,
	};

	const std::wstring INPUT_MODIFIER_TYPE_STR[] = {
		L"",
		L"Ctrl",
		L"Alt",
		L"Shift",
		L"Ctrl+Alt",
		L"Ctrl+Shift",
		L"Alt+Shift",
		L"Ctrl+Alt+Shift",
	};

	const std::unordered_map<std::wstring, INPUT_MODIFIER_FLAG> INPUT_MODIFIER_MAP = {
		{ L"NONE", INPUT_MODIFIER_FLAG_NONE },
		{ L"CTRL", INPUT_MODIFIER_FLAG_CTRL },
		{ L"ALT", INPUT_MODIFIER_FLAG_ALT },
		{ L"SHIFT", INPUT_MODIFIER_FLAG_SHIFT },
		{ L"CTRL_ALT", INPUT_MODIFIER_FLAG_CTRL_ALT },
		{ L"CTRL_SHIFT", INPUT_MODIFIER_FLAG_CTRL_SHIFT },
		{ L"ALT_SHIFT", INPUT_MODIFIER_FLAG_ALT_SHIFT },
		{ L"CTRL_ALT_SHIFT", INPUT_MODIFIER_FLAG_CTRL_ALT_SHIFT },
	};

	inline INPUT_MODIFIER_TYPE InputModifierFlagToType(INPUT_MODIFIER_FLAG flag)
	{
		switch(flag)
		{
			case App::INPUT_MODIFIER_FLAG_NONE:
				return INPUT_MODIFIER_TYPE_NONE;
			case App::INPUT_MODIFIER_FLAG_CTRL:
				return INPUT_MODIFIER_TYPE_CTRL;
			case App::INPUT_MODIFIER_FLAG_ALT:
				return INPUT_MODIFIER_TYPE_ALT;
			case App::INPUT_MODIFIER_FLAG_SHIFT:
				return INPUT_MODIFIER_TYPE_SHIFT;
			case App::INPUT_MODIFIER_FLAG_CTRL_ALT:
				return INPUT_MODIFIER_TYPE_CTRL_ALT;
			case App::INPUT_MODIFIER_FLAG_CTRL_SHIFT:
				return INPUT_MODIFIER_TYPE_CTRL_SHIFT;
			case App::INPUT_MODIFIER_FLAG_ALT_SHIFT:
				return INPUT_MODIFIER_TYPE_ALT_SHIFT;
			case App::INPUT_MODIFIER_FLAG_CTRL_ALT_SHIFT:
				return INPUT_MODIFIER_TYPE_CTRL_ALT_SHIFT;
			default:
				return INPUT_MODIFIER_TYPE_NONE;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Input binding data.
	//-----------------------------------------------------------------------------------------------

	struct InputBinding
	{
		INPUT_DEVICE device;
		u32 code;
		INPUT_MODIFIER_FLAG modifier;
		float scale;
		u32 setHash;

		InputBinding() { }
		InputBinding(INPUT_DEVICE device, u32 code = 0, INPUT_MODIFIER_FLAG modifier = INPUT_MODIFIER_FLAG_NONE, float scale = 1.0f) :
			device(device),
			code(code),
			modifier(modifier),
			scale(scale)
		{ }
	};

	struct InputCallbackData
	{
		const InputBinding* pBinding;
		float value;
		bool bPressed;
	};

	typedef std::function<void(const InputCallbackData&)> InputCallback;

	struct InputBindingSet
	{
		std::wstring name;
		InputCallback callback;
		InputBinding bindings[2];

		InputBindingSet() { }
		InputBindingSet(const std::wstring& name, const InputCallback& callback, const InputBinding& primary, const InputBinding& secondary)
		{
			this->name = name;
			this->callback = callback;
			bindings[0] = primary;
			bindings[1] = secondary;
			bindings[1].setHash = bindings[0].setHash = Math::FNV1a_32(name.c_str(), name.size());
		}
	};
};

#endif
