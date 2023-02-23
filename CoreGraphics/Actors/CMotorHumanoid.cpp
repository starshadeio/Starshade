//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CMotorHumanoid.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMotorHumanoid.h"
#include "../Factory/CFactory.h"
#include "../Application/CPlatform.h"
#include "CSpawner.h"
#include <Math/CMathFNV.h>
#include <Utilities/CTimer.h>
#include <unordered_map>
#include <functional>

namespace Actor
{
	static const std::unordered_map<u32, std::function<void(CMotorHumanoid*, const App::InputCallbackData&)>> HUMANOID_INPUT_MAP =
	{
		{ MOTOR_INPUT_MOVE_FORWARD, [](CMotorHumanoid* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->MoveForward(callback); } },
		{ MOTOR_INPUT_MOVE_BACKWARD, [](CMotorHumanoid* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->MoveBackward(callback); } },
		{ MOTOR_INPUT_MOVE_LEFT, [](CMotorHumanoid* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->MoveLeft(callback); } },
		{ MOTOR_INPUT_MOVE_RIGHT, [](CMotorHumanoid* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->MoveRight(callback); } },

		{ MOTOR_INPUT_LOOK_HORIZONTAL, [](CMotorHumanoid* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->LookHorizontal(callback); } },
		{ MOTOR_INPUT_LOOK_VERTICAL, [](CMotorHumanoid* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->LookVertical(callback); } },

		{ MOTOR_INPUT_JUMP, [](CMotorHumanoid* pHumanoid, const App::InputCallbackData& callback) { pHumanoid->Jump(callback); } },
	};
	
	const u32 CMotorHumanoid::MOTOR_HASH = Math::FNV1a_32("MOTOR_HUMANOID");

	CMotorHumanoid::CMotorHumanoid(const CVObject* pObject) :
		CMotor(pObject, MOTOR_HASH),
		m_velocityAxes{ { 0.0f, 0.0f }, { 0.0f, 0.0f } },
		m_bJump(false),
		m_footstepAudio{ nullptr, nullptr, nullptr }
	{
	}

	CMotorHumanoid::~CMotorHumanoid()
	{
	}
	
	void CMotorHumanoid::Initialize()
	{
		CMotor::Initialize();

		static const u64 hashCodeList[3] = {
			Math::FNV1a_64(L"AUDIO_FOOTSTEP_STONE_001"),
			Math::FNV1a_64(L"AUDIO_FOOTSTEP_STONE_002"),
			Math::FNV1a_64(L"AUDIO_FOOTSTEP_STONE_003"),
		};

		for(size_t i = 0; i < 3; ++i)
		{
			Audio::CAudioSource::Data data { };
			data.volume = 0.5f;
			data.audioClip = hashCodeList[i];
			m_footstepAudio[i].SetData(data);
			m_footstepAudio[i].Initialize();
		}
	}
	
	void CMotorHumanoid::Load()
	{
		CMotor::Load();

		CFactory::Instance().GetPlatform()->LockCursor(CFactory::Instance().GetPlatform()->GetMainPanel());
		CFactory::Instance().GetPlatform()->SetCursorVisibility(false);

		m_mouseDeltaFunc = std::bind(&CMotorHumanoid::AxisLook, this, std::placeholders::_1, std::placeholders::_2);
	}
	
	void CMotorHumanoid::Unload()
	{
		CMotor::Unload();
		memset(m_velocityAxes, 0, sizeof(m_velocityAxes));
	}
	
	void CMotorHumanoid::Update()
	{
		CMotor::Update();

		if(_mm_cvtss_f32(Math::SIMDVector::Dot(m_pPawn->GetTransform()->GetPosition(), Math::SIMD_VEC_UP)) < -64.0f)
		{
			CSpawner::Get().Spawn(m_pPawn);
		}
	}
	
	void CMotorHumanoid::PhysicsUpdate(Physics::CRigidbody& rigidbody)
	{
		// Calculate target velocity.
		Math::Vector3 targetVelocity, velocitySum;
		float eulerY;

		{
			std::lock_guard<std::mutex> lk(m_mutex);
			velocitySum = Math::Vector3(m_velocityAxes[0].x + m_velocityAxes[0].y, 0.0f, m_velocityAxes[1].x + m_velocityAxes[1].y);
			eulerY = m_pPawn->GetEulerAngles().y;
		}

		const float len = Math::Vector3::LengthSq(velocitySum);
		if(len > 1.0f)
		{
			targetVelocity = velocitySum / sqrtf(len);
		}
		else
		{
			targetVelocity = velocitySum;
		}
				
		Math::Vector3 right(cosf(eulerY), 0.0f, -sinf(eulerY));
		Math::Vector3 forward(sinf(eulerY), 0.0f, cosf(eulerY));
		targetVelocity = (right * targetVelocity.x + forward * targetVelocity.z) * m_data.speed;
		
		Math::Vector3 lastVel = *(Math::Vector3*)rigidbody.GetVelocity().ToFloat();
		static Math::Vector3 lastLastVel = lastVel;

		static float dist = 0.0f;
		if(!rigidbody.OnGround())
		{
			targetVelocity = Math::Vector3::Lerp(Math::Vector3(lastVel[0], 0.0f, lastVel[2]), 
				targetVelocity, std::min(1.0f, Util::CTimer::Instance().GetDelta() * 2.0f));
			dist = -1.0f;
		}
		else
		{
			if(dist < 0.0f)
			{
				m_footstepAudio[rand() % 3].Play(std::min(0.5f, fabsf(lastLastVel.y) * 0.0667f));
				dist = 0.0f;
			}
			else
			{
				dist += std::max(0.0f, lastVel.Length() - 1e-5f) * Util::CTimer::Instance().GetDelta();
				if(dist > 1.75f)
				{
					m_footstepAudio[rand() % 3].Play(0.33f);
					dist = 0.0f;
				}
			}
		}

		// Apply velocity and forces to rigidbody.
		rigidbody.SetVelocity(Math::SIMD_VEC_UP * lastVel[1] + targetVelocity);

		if(m_bJump)
		{
			if(rigidbody.OnGround())
			{
				m_footstepAudio[rand() % 3].Play(0.9f);
				rigidbody.AddForce(Math::SIMD_VEC_UP * m_data.jumpPower);
			}

			m_bJump = false;
		}

		lastLastVel = lastVel;
	}
	
	bool CMotorHumanoid::ProcessInput(u32 code, const App::InputCallbackData& callback)
	{
		auto elem = HUMANOID_INPUT_MAP.find(code);
		if(elem != HUMANOID_INPUT_MAP.end())
		{
			elem->second(this, callback);
			return true;
		}

		return false;
	}
	
	void CMotorHumanoid::Release()
	{
		for(size_t i = 0; i < 3; ++i)
		{
			m_footstepAudio[i].Release();
		}

		CMotor::Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Input methods.
	//-----------------------------------------------------------------------------------------------
	
	// Motion.
	void CMotorHumanoid::MoveForward(const App::InputCallbackData& callback)
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_velocityAxes[1].x = callback.value;
	}

	void CMotorHumanoid::MoveBackward(const App::InputCallbackData& callback)
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_velocityAxes[1].y = -callback.value;
	}

	void CMotorHumanoid::MoveLeft(const App::InputCallbackData& callback)
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_velocityAxes[0].x = -callback.value;
	}

	void CMotorHumanoid::MoveRight(const App::InputCallbackData& callback)
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_velocityAxes[0].y = callback.value;
	}

	// Look.
	void CMotorHumanoid::LookHorizontal(const App::InputCallbackData& callback)
	{
		if(m_mouseDeltaFunc) { m_mouseDeltaFunc(0, callback.value); }
	}

	void CMotorHumanoid::LookVertical(const App::InputCallbackData& callback)
	{
		if(m_mouseDeltaFunc) { m_mouseDeltaFunc(1, callback.value); }
	}

	// Action.
	void CMotorHumanoid::Jump(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			m_bJump = true;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	void CMotorHumanoid::AxisLook(size_t axis, float delta)
	{
		std::lock_guard<std::mutex> lk(m_mutex);
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
};
