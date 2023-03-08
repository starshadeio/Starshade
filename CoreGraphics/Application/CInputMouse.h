//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInputMouse.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CINPUTMOUSE_H
#define CINPUTMOUSE_H

#include "CInputDevice.h"
#include "CInputData.h"
#include <Math/CMathVector2.h>
#include <unordered_map>
#include <bitset>

namespace App
{
	// Mouse id constants.
	enum VM_MOUSE : u32
	{
		VM_MOUSE_LEFT,
		VM_MOUSE_RIGHT,
		VM_MOUSE_MIDDLE,
		VM_MOUSE_4,
		VM_MOUSE_5,
		VM_MOUSE_WHEEL_UP,
		VM_MOUSE_WHEEL_DOWN,
		VM_MOUSE_WHEEL,
		VM_MOUSE_DBL_LEFT,
		VM_MOUSE_DBL_RIGHT,
		VM_MOUSE_DBL_MIDDLE,
		VM_MOUSE_X,
		VM_MOUSE_Y,
		VM_MOUSE_DELTA_X,
		VM_MOUSE_DELTA_Y,
	};
	
	const std::wstring VK_MOUSE_STR[] = {
		L"Left Mouse",
		L"Right Mouse",
		L"Middle Mouse",
		L"Mouse (4)",
		L"Mouse (5)",
		L"Wheel Up",
		L"Wheel Down",
		L"Wheel",
		L"Double Left",
		L"Double Right",
		L"Double Middle",
		L"X",
		L"Y",
		L"Delta X",
		L"Delta Y",
	};

	class CInputMouse : public CInputDevice
	{
	public:
		static const std::unordered_map<std::wstring, VM_MOUSE> CODE_MAP;

	public:
		CInputMouse();
		~CInputMouse();
		CInputMouse(const CInputMouse&) = delete;
		CInputMouse(CInputMouse&&) = delete;
		CInputMouse& operator = (const CInputMouse&) = delete;
		CInputMouse& operator = (CInputMouse&&) = delete;

		void Initialize() final;
		void Reset() final;
		void Update() final;
		void PostUpdate() final;
		void Release() final;

		void RegisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding) final;
		void DeregisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding) final;

		void SetMouseWheel(float wheel);
		void SetMousePosition(const Math::Vector2& position);
		void SetMouseDelta(const Math::Vector2& delta);
		void SetMouseButton(VM_MOUSE code, bool bPressed);
		void SetMouseAxis(VM_MOUSE code, float value);

		// Accessors.
		inline const std::wstring& GetCodeName(u32 code) const final { return VK_MOUSE_STR[code]; }

		inline const Math::Vector2& GetMousePosition() const { return m_position; }

	private:
		Math::Vector2 m_position;
		Math::Vector2 m_delta;
		float m_wheel;

		std::bitset<16> m_mouseSet;

		std::unordered_map<u64, std::unordered_map<u32, InputBinding>> m_bindingMap;
	};
};

#endif
