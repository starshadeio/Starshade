//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInputMouse.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CInputMouse.h"
#include "CInputLayout.h"
#include "CInput.h"
#include <cassert>

namespace App
{
	CInputMouse::CInputMouse() :
		m_wheel(0.0f) {
	}

	CInputMouse::~CInputMouse() { }

	void CInputMouse::Initialize() { }

	void CInputMouse::Reset()
	{
		m_mouseSet.reset();
	}

	void CInputMouse::Update() { }

	void CInputMouse::PostUpdate()
	{
		m_wheel = 0.0f;
		m_delta = Math::VEC2_ZERO;
	}

	void CInputMouse::Release() { }

	//-----------------------------------------------------------------------------------------------
	// (De)registers.
	//-----------------------------------------------------------------------------------------------

	void CInputMouse::RegisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding)
	{
		assert(binding.device == INPUT_DEVICE_MOUSE);
		const u64 hash = (static_cast<u64>(layoutHash) << 32) | binding.code;
		m_bindingMap[hash].insert({ bindingHash, binding });
	}

	void CInputMouse::DeregisterBinding(u32 layoutHash, u32 bindingHash, const InputBinding& binding)
	{
		assert(binding.device == INPUT_DEVICE_MOUSE);
		const u64 hash = (static_cast<u64>(layoutHash) << 32) | binding.code;
		m_bindingMap[hash].erase(bindingHash);
	}

	//-----------------------------------------------------------------------------------------------
	// Modifiers.
	//-----------------------------------------------------------------------------------------------

	void CInputMouse::SetMouseWheel(float wheel)
	{
		SetMouseAxis(VM_MOUSE_WHEEL, wheel);
		m_wheel = wheel;
	}

	void CInputMouse::SetMousePosition(const Math::Vector2& position)
	{
		if(m_position.x != position.x)
		{
			SetMouseAxis(VM_MOUSE_X, position.x);
			m_position.x = position.x;
		}

		if(m_position.y != position.y)
		{
			SetMouseAxis(VM_MOUSE_Y, position.y);
			m_position.y = position.y;
		}
	}

	void CInputMouse::SetMouseDelta(const Math::Vector2& delta)
	{
		SetMouseAxis(VM_MOUSE_DELTA_X, delta.x);
		SetMouseAxis(VM_MOUSE_DELTA_Y, delta.y);
		m_delta = delta;
	}

	void CInputMouse::SetMouseButton(VM_MOUSE code, bool bPressed)
	{
		if(m_mouseSet[code] == bPressed) { return; }
		m_mouseSet[code].flip();
		
		const u64 hash = (static_cast<u64>(CInput::Instance().GetActiveLayout()->GetHash()) << 32) | code;
		auto elem = m_bindingMap.find(hash);
		if(elem != m_bindingMap.end())
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

	void CInputMouse::SetMouseAxis(VM_MOUSE code, float value)
	{
		const u64 hash = (static_cast<u64>(CInput::Instance().GetActiveLayout()->GetHash()) << 32) | code;
		auto elem = m_bindingMap.find(hash);
		if(elem != m_bindingMap.end())
		{
			for(auto& binding : elem->second)
			{
				InputCallbackData data { };
				data.bPressed = value;
				data.pBinding = &binding.second;
				data.value = value * binding.second.scale;
				CInput::Instance().GetActiveLayout()->GetBindingSet(binding.second.setHash).callback(data);
			}
		}
	}
};
