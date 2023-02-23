//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CMotorMonitor.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMotorMonitor.h"
#include "../Application/CEditor.h"
#include "../Application/CNodeSelect.h"
#include <Factory/CFactory.h>
#include <Application/CPlatform.h>
#include <Application/CInput.h>
#include <Application/CInputDeviceList.h>
#include <Application/CInputMouse.h>
#include <Application/CCommandManager.h>
#include <Graphics/CGraphicsAPI.h>
#include <Utilities/CTimer.h>
#include <unordered_map>
#include <functional>

namespace Actor
{
	static const std::unordered_map<u32, std::function<void(CMotorMonitor*, const App::InputCallbackData&)>> MONITOR_INPUT_MAP =
	{
		{ MOTOR_INPUT_MOVE_FORWARD, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->MoveForward(callback); } },
		{ MOTOR_INPUT_MOVE_BACKWARD, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->MoveBackward(callback); } },
		{ MOTOR_INPUT_MOVE_UP, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->MoveUp(callback); } },
		{ MOTOR_INPUT_MOVE_DOWN, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->MoveDown(callback); } },
		{ MOTOR_INPUT_MOVE_LEFT, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->MoveLeft(callback); } },
		{ MOTOR_INPUT_MOVE_RIGHT, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->MoveRight(callback); } },

		{ MOTOR_INPUT_LOOK_HORIZONTAL, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->LookHorizontal(callback); } },
		{ MOTOR_INPUT_LOOK_VERTICAL, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->LookVertical(callback); } },

		{ MOTOR_INPUT_SELECT, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->Select(callback); } },
		{ MOTOR_INPUT_EDITOR_LOOK, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->Look(callback); } },
		{ MOTOR_INPUT_EDITOR_PAN, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->Pan(callback); } },
		{ MOTOR_INPUT_EDITOR_ZOOM, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->Zoom(callback); } },
		{ MOTOR_INPUT_EDITOR_MODE_ERASE, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->ModeErase(callback); } },
		{ MOTOR_INPUT_EDITOR_MODE_FILL, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->ModeFill(callback); } },
		{ MOTOR_INPUT_EDITOR_MODE_PAINT, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->ModePaint(callback); } },
		{ MOTOR_INPUT_EDITOR_BRUSH_1, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->Brush1(callback); } },
		{ MOTOR_INPUT_EDITOR_BRUSH_2, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->Brush2(callback); } },
		{ MOTOR_INPUT_EDITOR_BRUSH_4, [](CMotorMonitor* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->Brush4(callback); } },
	};
	
	const u32 CMotorMonitor::MOTOR_HASH = Math::FNV1a_32("MOTOR_MONITOR");

	CMotorMonitor::CMotorMonitor(const CVObject* pObject) :
		CMotor(pObject, MOTOR_HASH),
		m_focusState(FocusState::None),
		m_velocityAxes{ { 0.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f } },
		m_panDelta(0.0f),
		m_zoomDelta(0.0f)
	{
	}

	CMotorMonitor::~CMotorMonitor()
	{
	}

	void CMotorMonitor::Load()
	{
		CMotor::Load();
		
		// Update cursor state.
		CFactory::Instance().GetPlatform()->UnlockCursor();
		CFactory::Instance().GetPlatform()->SetCursorVisibility(true);

		m_mouseDeltaFunc = nullptr;
	}
	
	void CMotorMonitor::Unload()
	{
		CMotor::Unload();
		memset(m_velocityAxes, 0, sizeof(m_velocityAxes));
	}
	
	void CMotorMonitor::Update()
	{
		// Calculate target velocity.
		Math::Vector3 targetVelocity;
		const Math::Vector3 velocitySum = Math::Vector3(m_velocityAxes[0].x + m_velocityAxes[0].y, m_velocityAxes[1].x + m_velocityAxes[1].y, m_velocityAxes[2].x + m_velocityAxes[2].y);
		const float len = Math::Vector3::LengthSq(velocitySum);
		if(len > 1.0f)
		{
			targetVelocity = velocitySum / sqrtf(len);
		}
		else
		{
			targetVelocity = velocitySum;
		}

		targetVelocity *= m_data.speed * Util::CTimer::Instance().GetDelta();

		m_pPawn->SetPosition(m_pPawn->GetBody()->GetPosition() +
			m_pPawn->GetHead()->GetRight() * (targetVelocity.x + m_panDelta.x * Util::CTimer::Instance().GetDelta()) +
			m_pPawn->GetHead()->GetUp() * (targetVelocity.y + m_panDelta.y * Util::CTimer::Instance().GetDelta()) +
			m_pPawn->GetHead()->GetForward() * (targetVelocity.z) +
			App::CEditor::Instance().Nodes().GetSelector().GetScreenRay().GetDirection() * m_zoomDelta
		);

		m_panDelta = 0;
		m_zoomDelta = 0.0f;
	}
	
	bool CMotorMonitor::ProcessInput(u32 code, const App::InputCallbackData& callback)
	{
		auto elem = MONITOR_INPUT_MAP.find(code);
		if(elem != MONITOR_INPUT_MAP.end())
		{
			elem->second(this, callback);
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------------------------------
	// Input methods.
	//-----------------------------------------------------------------------------------------------
	
	// Motion.
	void CMotorMonitor::MoveForward(const App::InputCallbackData& callback)
	{
		m_velocityAxes[2].x = callback.value;
	}

	void CMotorMonitor::MoveBackward(const App::InputCallbackData& callback)
	{
		m_velocityAxes[2].y = -callback.value;
	}

	void CMotorMonitor::MoveUp(const App::InputCallbackData& callback)
	{
		m_velocityAxes[1].x = callback.value;
	}

	void CMotorMonitor::MoveDown(const App::InputCallbackData& callback)
	{
		m_velocityAxes[1].y = -callback.value;
	}

	void CMotorMonitor::MoveLeft(const App::InputCallbackData& callback)
	{
		m_velocityAxes[0].x = -callback.value;
	}

	void CMotorMonitor::MoveRight(const App::InputCallbackData& callback)
	{
		m_velocityAxes[0].y = callback.value;
	}

	// Look.
	void CMotorMonitor::LookHorizontal(const App::InputCallbackData& callback)
	{
		if(m_mouseDeltaFunc) { m_mouseDeltaFunc(0, callback.value); }
	}

	void CMotorMonitor::LookVertical(const App::InputCallbackData& callback)
	{
		if(m_mouseDeltaFunc) { m_mouseDeltaFunc(1, callback.value); }
	}

	// Action.
	void CMotorMonitor::Select(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CNodeSelect::CMD_KEY_SELECT_PRESSED, &callback.bPressed);
		}
		else
		{
			App::CCommandManager::Instance().Execute(App::CNodeSelect::CMD_KEY_SELECT_RELEASED, &callback.bPressed);
		}
		
		App::CCommandManager::Instance().Execute(App::CNodeSelect::CMD_KEY_SELECT_ACTION, &callback.bPressed);
	}

	void CMotorMonitor::Look(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			if(m_focusState != FocusState::None) { return; }

			CFactory::Instance().GetPlatform()->LockCursor(CFactory::Instance().GetPlatform()->GetMainPanel());
			CFactory::Instance().GetPlatform()->SetCursorVisibility(false);
			m_mouseDeltaFunc = std::bind(&CMotorMonitor::AxisLook, this, std::placeholders::_1, std::placeholders::_2);
			m_focusState = FocusState::Look;
		}
		else
		{
			if(m_focusState != FocusState::Look) { return; }
			CFactory::Instance().GetPlatform()->UnlockCursor();
			CFactory::Instance().GetPlatform()->SetCursorVisibility(true);
			m_mouseDeltaFunc = nullptr;
			m_focusState = FocusState::None;
		}
	}
	
	void CMotorMonitor::Pan(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			if(m_focusState != FocusState::None) { return; }

			CFactory::Instance().GetPlatform()->LockCursor(CFactory::Instance().GetPlatform()->GetMainPanel());
			CFactory::Instance().GetPlatform()->SetCursorVisibility(false);
			m_mouseDeltaFunc = std::bind(&CMotorMonitor::AxisPan, this, std::placeholders::_1, std::placeholders::_2);
			m_focusState = FocusState::Pan;
		}
		else
		{
			if(m_focusState != FocusState::Pan) { return; }

			CFactory::Instance().GetPlatform()->UnlockCursor();
			CFactory::Instance().GetPlatform()->SetCursorVisibility(true);
			m_mouseDeltaFunc = nullptr;
			m_focusState = FocusState::None;
		}
	}
	
	void CMotorMonitor::Zoom(const App::InputCallbackData& callback)
	{
		m_zoomDelta += callback.value * m_data.zoomRate;
	}
	
	void CMotorMonitor::ModeErase(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CNodeSelect::CMD_KEY_SELECT_ERASE);
		}
	}
	
	void CMotorMonitor::ModeFill(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CNodeSelect::CMD_KEY_SELECT_FILL);
		}
	}
	
	void CMotorMonitor::ModePaint(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CNodeSelect::CMD_KEY_SELECT_PAINT);
		}
	}
	
	void CMotorMonitor::Brush1(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CNodeSelect::CMD_KEY_SELECT_BRUSH_1);
		}
	}
	
	void CMotorMonitor::Brush2(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CNodeSelect::CMD_KEY_SELECT_BRUSH_2);
		}
	}
	
	void CMotorMonitor::Brush4(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CNodeSelect::CMD_KEY_SELECT_BRUSH_4);
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	void CMotorMonitor::AxisLook(size_t axis, float delta)
	{
		Math::Vector3 euler = m_pPawn->GetEulerAngles();

		delta *= Math::g_PiOver180;
		if(axis == 0)
		{
			
			euler.y = fmod(euler.y + delta * m_data.lookRate.x, Math::g_2Pi);
		}
		else
		{
			euler.x = Math::Clamp(euler.x - delta * m_data.lookRate.y, -Math::g_PiOver2, Math::g_PiOver2);
		}

		m_pPawn->SetEulerAngles(euler);
	}
	
	void CMotorMonitor::AxisPan(size_t axis, float delta)
	{
		if(axis == 0)
		{
			m_panDelta.x -= m_data.panRate.x * delta;
		}
		else
		{
			m_panDelta.y += m_data.panRate.y * delta;
		}
	}
};
