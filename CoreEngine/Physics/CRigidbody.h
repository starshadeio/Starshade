//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CRigidbody.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CRIGIDBODY_H
#define CRIGIDBODY_H

#include "CPhysicsData.h"
#include "../Objects/CVComponent.h"
#include "../Logic/CTransform.h"
#include "../Math/CSIMDMatrix.h"
#include "../Utilities/CTSDeque.h"
#include <mutex>
#include <vector>

namespace Physics
{
	class CRigidbody : public CVComponent
	{
	private:
		struct Stamp
		{
			float delta;
			Math::SIMDVector position;
			Math::SIMDQuaternion rotation;
		};

	public:
		struct Data
		{
			float invMass;
			float damping;
			class CVolume* pVolume;

			// Modifiers.
			inline void SetMass(float mass) { invMass = 1.0f / mass; }
		};


	public:
		CRigidbody(const CVObject* pObject);
		~CRigidbody();
		CRigidbody(const CRigidbody&) = delete;
		CRigidbody(CRigidbody&&) = delete;
		CRigidbody& operator = (const CRigidbody&) = delete;
		CRigidbody& operator = (CRigidbody&&) = delete;

		void Reset();
		void ResetSolverState();
		void UpdateTransform(Logic::CTransform& tranform);
		void Calculate();
		bool Response();
		void Apply(std::function<void()> onWasHit);

		void SetupIdleSolver();
		bool StepIdleSolver(const Math::SIMDVector& contact, const Math::SIMDVector& normal);
		void ApplyIdleSolver();

		void UpdateTransformFromVolume();
		bool TryToAddContact(const ContactData& contactData);
		
		// Accessors.
		inline const Math::SIMDVector& GetVelocity() const { return m_velocity; }
		inline const Math::SIMDVector& GetSolverPosition() const { return m_solverPosition; }
		inline const Math::SIMDVector& GetSolverVelocity() const { return m_solverVelocity; }
		inline const Math::SIMDQuaternion& GetSolverRotation() const { return m_solverRotation; }
		inline bool OnGround() const { return m_bOnGround; }
		inline bool IsValid() const { return !m_bLastHit; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

		inline void SetAcceleration(const Math::SIMDVector& acceleration) { m_acceleration = acceleration; }
		inline void SetVelocity(const Math::SIMDVector& velocity) { m_velocity = velocity; }
		inline void AddForce(const Math::SIMDVector& force) { m_forceAccum += force; }

	private:
		void ClearAccumulator();
		void ClearContacts();

	private:
		std::mutex m_mutex;

		Data m_data;

		Math::SIMDVector m_velocity;
		Math::SIMDVector m_acceleration;
		Math::SIMDVector m_forceAccum;
		
		Math::SIMDVector m_validNormal;
		Math::SIMDVector m_solverPosition;
		Math::SIMDVector m_solverVelocity;
		Math::SIMDQuaternion m_solverRotation;
		Math::SIMDVector m_finalPosition;
		Math::SIMDVector m_finalVelocity;
		Math::SIMDQuaternion m_finalRotation;
		
		Math::SIMDVector m_firstContactPoint;
		std::vector<ContactData> m_contactList;
		bool m_bLastHit;
		bool m_bHit;
		bool m_bOnGround;

		Stamp m_lastStamp;
		Util::CDeque<Stamp> m_stampQueue;
		float m_interpT;
	};
};

#endif
