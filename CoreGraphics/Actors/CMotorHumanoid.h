//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CMotorHumanoid.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMOTORHUMANOID_H
#define CMOTORHUMANOID_H

#include "CMotor.h"
#include "../Audio/CAudioSource.h"
#include "../Application/CInputData.h"
#include <Math/CMathVector2.h>
#include <mutex>
#include <functional>

namespace Actor
{
	class CMotorHumanoid : public CMotor
	{
	public:
		static const u32 MOTOR_HASH;

	public:
		struct Data : BaseData
		{
			float speed = 1.0f;
			float jumpPower = 1.0f;
			Math::Vector2 lookRate = 1.0f;

			Data()
			{
				BaseData::bUsePhysics = true;
			}
		};

	public:
		CMotorHumanoid(const CVObject* pObject);
		~CMotorHumanoid();
		CMotorHumanoid(const CMotorHumanoid&) = delete;
		CMotorHumanoid(CMotorHumanoid&&) = delete;
		CMotorHumanoid& operator = (const CMotorHumanoid&) = delete;
		CMotorHumanoid& operator = (CMotorHumanoid&&) = delete;

		void Initialize() final;
		void Load() final;
		void Unload() final;

		void Update() final;
		void PhysicsUpdate(Physics::CRigidbody& rigidbody) final;
		
		void Release() final;

		bool ProcessInput(u32 code, const App::InputCallbackData& callback) final;

		void MoveForward(const App::InputCallbackData& callback);
		void MoveBackward(const App::InputCallbackData& callback);
		void MoveLeft(const App::InputCallbackData& callback);
		void MoveRight(const App::InputCallbackData& callback);

		void LookHorizontal(const App::InputCallbackData& callback);
		void LookVertical(const App::InputCallbackData& callback);

		void Jump(const App::InputCallbackData& callback);

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	protected:
		void AxisLook(size_t axis, float delta);

		inline const BaseData& GetBaseData() const final { return m_data; }

	private:
		Data m_data;

		std::mutex m_mutex;
		Math::Vector2 m_velocityAxes[2];
		Abool m_bJump;

		std::function<void(size_t, float)> m_mouseDeltaFunc;

		Audio::CAudioSource m_footstepAudio[3];
	};
};

#endif
