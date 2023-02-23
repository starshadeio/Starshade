//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CMotorMonitor.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMOTORMONITOR_H
#define CMOTORMONITOR_H

//#include "CPlayerSelector.h"
#include <Actors/CMotor.h>
#include <Math/CMathVector2.h>
#include <functional>

namespace Actor
{
	class CMotorMonitor : public CMotor
	{
	public:
		static const u32 MOTOR_HASH;

	private:
		enum class FocusState
		{
			None,
			Look,
			Pan,
		};
		
	public:
		struct Data : BaseData
		{
			float speed = 1.0f;
			Math::Vector2 lookRate = 1.0f;
			Math::Vector2 panRate = 1.0f;
			float zoomRate = 1.0f;

			Data()
			{
				BaseData::bUsePhysics = false;
			}
		};

	public:
		CMotorMonitor(const CVObject* pObject);
		~CMotorMonitor();
		CMotorMonitor(const CMotorMonitor&) = delete;
		CMotorMonitor(CMotorMonitor&&) = delete;
		CMotorMonitor& operator = (const CMotorMonitor&) = delete;
		CMotorMonitor& operator = (CMotorMonitor&&) = delete;

		void Load() final;
		void Unload() final;

		void Update() final;

		bool ProcessInput(u32 code, const App::InputCallbackData& callback) final;
		
		void MoveForward(const App::InputCallbackData& callback);
		void MoveBackward(const App::InputCallbackData& callback);
		void MoveUp(const App::InputCallbackData& callback);
		void MoveDown(const App::InputCallbackData& callback);
		void MoveLeft(const App::InputCallbackData& callback);
		void MoveRight(const App::InputCallbackData& callback);

		void LookHorizontal(const App::InputCallbackData& callback);
		void LookVertical(const App::InputCallbackData& callback);

		void Select(const App::InputCallbackData& callback);
		void Look(const App::InputCallbackData& callback);
		void Pan(const App::InputCallbackData& callback);
		void Zoom(const App::InputCallbackData& callback);
		void ModeErase(const App::InputCallbackData& callback);
		void ModeFill(const App::InputCallbackData& callback);
		void ModePaint(const App::InputCallbackData& callback);
		void Brush1(const App::InputCallbackData& callback);
		void Brush2(const App::InputCallbackData& callback);
		void Brush4(const App::InputCallbackData& callback);

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		
	protected:
		void AxisLook(size_t axis, float delta);
		void AxisPan(size_t axis, float delta);

		inline const BaseData& GetBaseData() const final { return m_data; }

	private:
		Data m_data;
		
		FocusState m_focusState;
		Math::Vector2 m_velocityAxes[3];
		Math::Vector2 m_panDelta;
		float m_zoomDelta;

		std::function<void(size_t, float)> m_mouseDeltaFunc;
	};

	const u32 MOTOR_INPUT_EDITOR_LOOK = 0x100;
	const u32 MOTOR_INPUT_EDITOR_PAN = 0x101;
	const u32 MOTOR_INPUT_EDITOR_ZOOM = 0x102;
	const u32 MOTOR_INPUT_EDITOR_MODE_ERASE = 0x103;
	const u32 MOTOR_INPUT_EDITOR_MODE_FILL = 0x104;
	const u32 MOTOR_INPUT_EDITOR_MODE_PAINT = 0x105;
	const u32 MOTOR_INPUT_EDITOR_BRUSH_1 = 0x106;
	const u32 MOTOR_INPUT_EDITOR_BRUSH_2 = 0x107;
	const u32 MOTOR_INPUT_EDITOR_BRUSH_4 = 0x108;
};

#endif
