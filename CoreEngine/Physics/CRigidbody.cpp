//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CRigidbody.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CRigidbody.h"
#include "CPhysics.h"
#include "CVolume.h"
#include "../Utilities/CTimer.h"

namespace Physics
{
	CRigidbody::CRigidbody(const CVObject* pObject) :
		CVComponent(pObject) { }

	CRigidbody::~CRigidbody() { }

	
	void CRigidbody::UpdateTransform(Logic::CTransform& tranform)
	{
		if(!tranform.IsValid())
		{
			return;
		}

		Stamp stamp;
		bool bQueueEmpty = false;

		{ // Update interpolation values.
			std::lock_guard<std::mutex> lk (m_mutex);

			if(m_stampQueue.TryPeekFront(stamp))
			{
				// Update interpolation delta and queue.
				m_interpT += Util::CTimer::Instance().GetDelta() / stamp.delta;
				while(m_interpT >= 1.0f)
				{
					m_stampQueue.TryPopFront(m_lastStamp);
					if(!m_stampQueue.TryPeekFront(stamp)) { break; }
					m_interpT = (m_interpT - 1.0f) * (m_lastStamp.delta / stamp.delta);
				}

				if(m_stampQueue.Size() >= 8)
				{
					// If the queue size is past cap, clear it.
					m_stampQueue.TryPeekBack(m_lastStamp);
					m_stampQueue.Clear();
				}

				bQueueEmpty = m_stampQueue.Empty();
			}
			else
			{
				// If the queue is empty, reset the interpolation delta.
				m_interpT = 0.0f;
			}
		}

		// Update transform interpolation.
		if(!bQueueEmpty)
		{
			tranform.InterpolateRigidbodyPosition(m_lastStamp.position, stamp.position, m_interpT);
			tranform.InterpolateRigidbodyRotation(m_lastStamp.rotation, stamp.rotation, m_interpT);
		}
		else
		{
			tranform.SetRigidbodyPosition(m_lastStamp.position);
			tranform.SetRigidbodyRotation(m_lastStamp.rotation);
			m_interpT = 0.0f;
		}
	}
	
	void CRigidbody::Reset()
	{
		m_velocity = Math::SIMD_VEC_ZERO;
		m_acceleration = Math::SIMD_VEC_ZERO;
		m_forceAccum = Math::SIMD_VEC_ZERO;
		
		m_validNormal = Math::SIMD_VEC_ZERO;
		m_solverPosition = Math::SIMD_VEC_ZERO;
		m_solverVelocity = Math::SIMD_VEC_ZERO;
		m_solverRotation = Math::SIMD_QUAT_IDENTITY;
		m_finalPosition = Math::SIMD_VEC_ZERO;
		m_finalVelocity = Math::SIMD_VEC_ZERO;
		m_finalRotation = Math::SIMD_QUAT_IDENTITY;
		
		m_firstContactPoint = Math::SIMD_VEC_ZERO;
		m_contactList.clear();
		m_bLastHit = false;
		m_bHit = false;
		m_bOnGround = false;

		m_lastStamp = { };
		m_stampQueue.Clear();
		m_interpT = 0.0f;
	}

	void CRigidbody::ResetSolverState()
	{
		m_solverPosition = m_data.pVolume->m_position;
		m_solverRotation = m_data.pVolume->m_rotation;
		m_solverVelocity = m_velocity * Util::CTimer::Instance().GetDelta();
		m_finalPosition = m_data.pVolume->m_position + m_solverVelocity;
		m_finalVelocity = m_solverVelocity;
		m_finalRotation = m_solverRotation;
		m_bLastHit = false;
		m_bHit = false;
	}

	void CRigidbody::Calculate()
	{
		const float delta = Util::CTimer::Instance().GetDelta();
		
		//m_acceleration += m_forceAccum * m_data.invMass;
		m_velocity += (m_acceleration + CPhysics::Instance().GetData().gravity) * delta;
		m_velocity += m_forceAccum * m_data.invMass;

		m_velocity *= powf(m_data.damping, delta);

		// Set adjustable values that can change during the solvers iterations.
		ClearAccumulator();
		m_bOnGround = false;
	}
	
	bool CRigidbody::Response()
	{
		m_bHit |= m_bLastHit = !m_contactList.empty();
		if(m_bLastHit)
		{
			if(_mm_cvtss_f32(m_contactList[0].hitNormal.LengthSq()) < 1e-5f)
			{
				m_contactList[0].hitNormal = m_validNormal;
			}
			
			float dist;

			// Postion.
			m_validNormal = m_contactList[0].hitNormal;
			Math::SIMDVector velocity = m_solverVelocity * m_contactList[0].interval;
			m_solverPosition += velocity + m_contactList[0].hitNormal * (/*m_data.pVolume->GetSkinDepth() + */1e-3f);

			dist = _mm_cvtss_f32(m_contactList[0].hitNormal.Dot(m_finalPosition - m_solverPosition));
			m_finalPosition -= m_contactList[0].hitNormal * dist;

			// Velocity.
			m_solverVelocity = m_finalPosition - m_solverPosition;
			
			{
				bool bGroundContact;
				m_bOnGround |= bGroundContact = _mm_cvtss_f32(Math::SIMDVector::Dot(m_contactList[0].hitNormal, Math::SIMD_VEC_UP)) > 0.25f;

				if(bGroundContact && _mm_cvtss_f32(Math::SIMDVector::Dot(Math::SIMD_VEC_UP, m_solverVelocity)) < 0.0f)
				{
					m_solverVelocity -= Math::SIMD_VEC_UP * _mm_cvtss_f32(Math::SIMD_VEC_UP.Dot(m_solverVelocity));
				}
			}

			dist = _mm_cvtss_f32(m_contactList[0].hitNormal.Dot(m_finalVelocity));
			m_finalVelocity -= m_contactList[0].hitNormal * dist;

			ClearContacts();

			if(_mm_cvtss_f32(Math::SIMDVector::Dot(m_velocity, m_solverVelocity)) < 0.0f)
			{
				m_solverVelocity = 0.0f;
				m_finalPosition = m_solverPosition;
				return false;
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	void CRigidbody::Apply(std::function<void()> onWasHit)
	{
		const float delta = Util::CTimer::Instance().GetDelta();
		
		Math::SIMDVector lastPos = m_data.pVolume->m_position;

		if(m_bLastHit)
		{
			// Final transformation might still be invalid, so set their values to what was last value.
			m_finalPosition = m_solverPosition;
			m_finalRotation = m_solverRotation;
			m_finalVelocity = m_solverVelocity;
			m_bLastHit = false;
		}
		
		m_velocity = m_finalVelocity / delta;
		m_data.pVolume->m_position = m_solverPosition = m_finalPosition;
		m_data.pVolume->m_rotation = m_solverRotation = m_finalRotation;
		m_data.pVolume->UpdateBounds();

		if(onWasHit) onWasHit();

		if(_mm_cvtss_f32((m_data.pVolume->m_position - lastPos).LengthSq()) > 0.0f)
		{
			Stamp nextStamp;
			nextStamp.delta = delta;
			nextStamp.position = m_data.pVolume->GetPosition();
			nextStamp.rotation = m_data.pVolume->GetRotation();

			// Update transition stamp;
			std::lock_guard<std::mutex> lk (m_mutex);
			m_stampQueue.PushBack(nextStamp);
		}

		ClearContacts();
	}

	void CRigidbody::SetupIdleSolver()
	{
		m_solverVelocity = m_velocity;
		m_solverPosition = m_data.pVolume->m_position;
		m_solverRotation = m_data.pVolume->m_rotation;
	}

	bool CRigidbody::StepIdleSolver(const Math::SIMDVector& contact, const Math::SIMDVector& normal)
	{
		bool bGroundContact;
		m_bOnGround |= bGroundContact = _mm_cvtss_f32(Math::SIMDVector::Dot(normal, Math::SIMD_VEC_UP)) > 0.25f;

		m_solverPosition += contact;
		if(bGroundContact && _mm_cvtss_f32(Math::SIMDVector::Dot(Math::SIMD_VEC_UP, m_solverVelocity)) < 0.0f)
		{
			m_solverVelocity -= Math::SIMD_VEC_UP * _mm_cvtss_f32(Math::SIMD_VEC_UP.Dot(m_solverVelocity));
		}

		return _mm_cvtss_f32(contact.Dot(contact)) > 1e-10f;
	}

	void CRigidbody::ApplyIdleSolver()
	{
		m_velocity = m_solverVelocity;
		m_data.pVolume->m_position = m_solverPosition;
		m_data.pVolume->m_rotation = m_solverRotation;
		m_data.pVolume->UpdateBounds();
	}
	
	//-----------------------------------------------------------------------------------------------
	// Internal methods.
	//-----------------------------------------------------------------------------------------------
	
	void CRigidbody::ClearAccumulator()
	{
		m_forceAccum = Math::SIMD_VEC_ZERO;
	}
	
	void CRigidbody::ClearContacts()
	{
		m_contactList.clear();
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CRigidbody::UpdateTransformFromVolume()
	{
		Reset();

		Stamp nextStamp;
		m_lastStamp.delta = nextStamp.delta = Util::CTimer::Instance().GetDelta();
		m_finalPosition = m_solverPosition = m_lastStamp.position = nextStamp.position = m_data.pVolume->GetPosition();
		m_finalRotation = m_solverRotation = m_lastStamp.rotation = nextStamp.rotation = m_data.pVolume->GetRotation();

		// Update transition stamp;
		std::lock_guard<std::mutex> lk (m_mutex);
		m_stampQueue.PushBack(nextStamp);
	}

	// Method for attempting to add contact data to the rigidbodies contact list.
	bool CRigidbody::TryToAddContact(const ContactData& contactData)
	{
		if(m_contactList.empty())
		{
			m_contactList.push_back(contactData);
			return true;
		}
		else if(m_contactList[0].interval >= contactData.interval)
		{
			// Clear the list if contactData.interval is smaller than what's already in the list.
			if(m_contactList[0].interval > contactData.interval)
			{
				m_contactList.clear();
			}

			m_contactList.push_back(contactData);
			return true;
		}

		return false;
	}
};
