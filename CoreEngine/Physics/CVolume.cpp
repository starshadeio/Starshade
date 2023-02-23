//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CVolume.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CVolume.h"
#include "CPhysics.h"
#include "CRigidbody.h"
#include "CGJK.h"
#include "../Logic/CTransform.h"
#include "../Utilities/CTimer.h"
//#include <Windows.h>

namespace Physics
{
	CVolume::CVolume(const CVObject* pObject) : CVComponent(pObject), m_sepAxis(Math::SIMD_VEC_FORWARD)
	{
	}

	CVolume::~CVolume() { }
	
	//-----------------------------------------------------------------------------------------------
	// (De)registration methods.
	//-----------------------------------------------------------------------------------------------
	
	void CVolume::Register(const Math::SIMDMatrix& world)
	{
		Recalculate(world);
		OnPhysicsRegistered();
		CPhysics::Instance().RegisterVolume(this);
	}

	void CVolume::Deregister()
	{
		CPhysics::Instance().DeregisterVolume(this);
		OnPhysicsDeregistered();

		if(GetData().pRigidbody)
		{
			GetData().pRigidbody->Reset();
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Solver methods.
	//-----------------------------------------------------------------------------------------------
	
	// Solving for solving against a moving rigidbody.
	bool CVolume::MotionSolver(CVolume* pOther)
	{
		ContactData contactData { };
		Math::SIMDVector r = (pOther->GetSolverVelocity() - GetSolverVelocity());

		if(CGJK::MovingContact(this, pOther, r, contactData.hitPoint, contactData.hitNormal, contactData.interval, pOther->m_sepAxis))
		{
			contactData.pVolume = this;
			contactData.index = 0;

			if(contactData.interval == 0.0f)
			{
				Math::SIMDVector distVec = CGJK::Distance(this, pOther, false);
				contactData.hitNormal = -distVec;
				if(_mm_cvtss_f32(contactData.hitNormal.LengthSq()) > Math::g_EpsilonTol * Math::g_EpsilonTol)
				{
					contactData.hitNormal.Normalize();
				}
			}
			
			pOther->GetRigidbody()->TryToAddContact(contactData);
			return true;
		}

		return false;
	}
	
	// Solving for solving against an idle/resting rigidbody.
	bool CVolume::IdleSolver(CVolume* pOther)
	{
		Math::SIMDVector contactA;
		Math::SIMDVector contactB;
		Math::SIMDVector normal = Math::SIMD_VEC_ZERO;

		if(CGJK::RestingContact(this, pOther, contactA, contactB, normal))
		{
			return pOther->GetRigidbody()->StepIdleSolver(contactB, normal);
		}

		return false;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Internal methods.
	//-----------------------------------------------------------------------------------------------
	
	Math::SIMDVector CVolume::GetVelocity() const
	{
		if(GetData().pRigidbody == nullptr) return Math::SIMD_VEC_ZERO;
		return GetData().pRigidbody->GetVelocity();

	}
	
	Math::SIMDVector CVolume::GetSolverPosition() const
	{
		if(GetData().pRigidbody == nullptr) return m_position;
		return GetData().pRigidbody->GetSolverPosition();
	}

	Math::SIMDVector CVolume::GetSolverVelocity() const
	{
		if(GetData().pRigidbody == nullptr) return Math::SIMD_VEC_ZERO;
		return GetData().pRigidbody->GetSolverVelocity();
	}

	Math::SIMDQuaternion CVolume::GetSolverRotation() const
	{
		if(GetData().pRigidbody == nullptr) return m_rotation;
		return GetData().pRigidbody->GetSolverRotation();
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	void CVolume::Recalculate(const Math::SIMDMatrix& world)
	{
		Math::SIMDMatrix tmp = Math::SIMDMatrix::Transpose(world);
		m_position = _mm_mul_ps(_mm_setr_ps(1.0f, 1.0f, 1.0f, 0.0f), tmp.rows[3]);

		// Assuming rigid matrix where scale is 1.0f.
		float w = sqrtf(1.0f + world.f32[0] + world.f32[5] + world.f32[10]) * 0.5f;
		float inv4w = 1.0f / (4.0f * w);
		float x = (world.f32[9] - world.f32[6]) * inv4w;
		float y = (world.f32[2] - world.f32[8]) * inv4w;
		float z = (world.f32[4] - world.f32[1]) * inv4w;
		m_rotation = Math::SIMDQuaternion(x, y, z, w);

		if(GetData().pRigidbody)
		{
			GetData().pRigidbody->UpdateTransformFromVolume();
		}

		UpdateBounds();
	}
};
