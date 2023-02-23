//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CVolume.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CVOLUME_H
#define CVOLUME_H

#include "CPhysicsData.h"
#include "../Objects/CVComponent.h"
#include "../Math/CSIMDVector.h"
#include "../Math/CMathVector3.h"
#include "../Math/CSIMDMatrix.h"
#include "../Logic/CTransform.h"
#include <functional>
#include <vector>

namespace Physics
{
	class CVolume : public CVComponent
	{
	private:
		friend class CRigidbody;

	protected:
		struct mData
		{
			bool bAllowRays;
			float skinDepth = 1e-2f;
			ColliderType colliderType;
			class CRigidbody* pRigidbody;
			class CForceField* pForceField;
			std::function<void(CVolume*, CVolume*)> onCollision;
		};

	public:
		CVolume(const CVObject* pObject);
		virtual ~CVolume();

		CVolume(const CVolume&) = delete;
		CVolume(CVolume&&) = delete;
		CVolume& operator = (const CVolume&) = delete;
		CVolume& operator = (CVolume&&) = delete;

		void Register(const Math::SIMDMatrix& world);
		void Deregister();
		virtual bool MotionSolver(CVolume* pOther);
		virtual bool IdleSolver(CVolume* pOther);

		// We are always assuming a rigid world matrix for physics colliders.
		void Recalculate(const Math::SIMDMatrix& world);
		
		virtual bool RayTest(const QueryRay& query, RaycastInfo& info) const { return false; }
		virtual Math::SIMDVector SupportPoint(const Math::SIMDVector& dir, const CVolume* pVolumeA, float inset = 0.0f) const { return Math::SIMD_VEC_ZERO; }

		// Accessors.
		inline const CVObject* GetVObject() const { return m_pObject; }

		inline bool AllowsRays() const { return GetData().bAllowRays; }
		inline bool IsCollider() const { return GetData().colliderType != ColliderType::None; }
		inline float GetSkinDepth() const { return GetData().skinDepth; }

		inline const Math::Vector3& GetMinExtents() const { return m_minExtents; }
		inline const Math::Vector3& GetMaxExtents() const { return m_maxExtents; }

		inline ColliderType GetColliderType() const { return GetData().colliderType; }
		inline class CRigidbody* GetRigidbody() const { return GetData().pRigidbody; }
		inline class CForceField* GetForceField() const { return GetData().pForceField; }

	protected:
		virtual inline const mData& GetData() const = 0;

		// Accessors
		inline const Math::SIMDVector& GetPosition() const { return m_position; }
		inline const Math::SIMDQuaternion& GetRotation() const { return m_rotation; }

	public:
		Math::SIMDVector GetVelocity() const;
		Math::SIMDVector GetSolverPosition() const;
		Math::SIMDVector GetSolverVelocity() const;
		Math::SIMDQuaternion GetSolverRotation() const;
		
	private:
		virtual void UpdateBounds() { m_minExtents = m_maxExtents = 0.0f; }

		// Modifiers.
		inline void AddTranslation(const Math::SIMDVector& translation) { m_position += translation; }
		inline void AddRotation(const Math::SIMDQuaternion& rotation) { m_rotation *= rotation; }

	protected:
		Math::Vector3 m_minExtents;
		Math::Vector3 m_maxExtents;

	private:
		Math::SIMDVector m_position;
		Math::SIMDQuaternion m_rotation;
		
		Math::SIMDVector m_sepAxis;
	};
};

#endif
