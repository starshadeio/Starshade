//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CPhysicsWorld.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPHYSICSWORLD_H
#define CPHYSICSWORLD_H

#include "CPhysicsData.h"
#include "../Math/CSIMDMatrix.h"
#include "../Utilities/CTSDeque.h"
#include <unordered_map>

namespace Physics
{
	class CPhysicsWorld
	{
	public:
		CPhysicsWorld();
		~CPhysicsWorld();
		CPhysicsWorld(const CPhysicsWorld&) = delete;
		CPhysicsWorld(CPhysicsWorld&&) = delete;
		CPhysicsWorld& operator = (const CPhysicsWorld&) = delete;
		CPhysicsWorld& operator = (CPhysicsWorld&&) = delete;
		
		void AddVolume(class CVolume* pVolume);
		void RemoveVolume(class CVolume* pVolume);
		void UpdateVolume(class CVolume* pVolume, const Math::SIMDMatrix& world);

		void UpdateForceFields();
		void UpdateRigidbodies();
		void Solve(u32 idleIterations, u32 rayCastIterations);

		void CastRay(const QueryRay& queryRay);

	private:
		std::unordered_map<u64, class CVolume*> m_volumeMap;
		std::unordered_map<u64, class CVolume*> m_colliderMap;
		std::unordered_map<u64, class CVolume*> m_rigidbodyMap;
		std::unordered_map<u64, class CVolume*> m_forceFieldMap;
		std::unordered_map<u64, class CVolume*> m_rayCastMap;
	};
};

#endif
