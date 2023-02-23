//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CPhysics.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPhysics.h"
#include "CVolume.h"
#include "../Utilities/CTimer.h"
#include <thread>

namespace Physics
{
	CPhysics::CPhysics() :
		m_exitFlag(false),
		m_physicsUpdateBatch(4, 4)
	{}

	CPhysics::~CPhysics()
	{}

	void CPhysics::Initialize()
	{
		std::promise<void> p;
		m_futureExit = p.get_future();
		std::thread t(&CPhysics::PhysicsThread, this, std::move(p));
		t.detach();
	}

	void CPhysics::Halt()
	{
		m_exitFlag = true;
		m_futureExit.wait();
	}
	
	void CPhysics::Release()
	{
		m_physicsUpdateBatch.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Queries.
	//-----------------------------------------------------------------------------------------------

	void CPhysics::CastRay(const QueryRay& query)
	{
		QueryRay q = query;
		m_queryRayQueue.PushBack(q);
	}

	//-----------------------------------------------------------------------------------------------
	// Utilities.
	//-----------------------------------------------------------------------------------------------

	void CPhysics::MarkObjectAsDirty(const CVObject* pObject, const Math::SIMDMatrix& world)
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		auto elem = m_volumeMap.find(pObject->GetHash());
		if(elem != m_volumeMap.end())
		{
			std::pair<Physics::CVolume*, Math::SIMDMatrix> pair = { elem->second, world };
			m_dirtyQueue.PushBack(pair);
		}
	}

	//-----------------------------------------------------------------------------------------------
	// (De)registers.
	//-----------------------------------------------------------------------------------------------

	void CPhysics::RegisterVolume(CVolume* pVolume)
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_volumeMap.insert({ pVolume->GetVObject()->GetHash(), pVolume });
		m_insertionQueue.PushBack(pVolume);
	}

	void CPhysics::DeregisterVolume(CVolume* pVolume)
	{
		std::lock_guard<std::mutex> lk(m_mutex);
		m_volumeMap.erase(pVolume->GetVObject()->GetHash());
		m_deletionQueue.PushBack(pVolume);
	}

	//-----------------------------------------------------------------------------------------------
	// Main physics thread.
	//-----------------------------------------------------------------------------------------------

	void CPhysics::PhysicsThread(std::promise<void> p)
	{
		Util::CTimer::Instance().SetTargetFrameRate(m_data.targetFPS);

		while(!m_exitFlag)
		{
			Util::CTimer::Instance().Tick();
			
			// Process external collider updates.
			ProcessColliderUpdates();
			
			// Physics updates.
			// Update game-driven rigidbodies.
			m_physicsUpdateBatch.Update();

			// Update phantoms.
			// Update forces, apply impulses and adjust constraints.
			m_physicsWorld.UpdateForceFields();
			m_physicsWorld.UpdateRigidbodies();
			
			// Step the simulation.
			m_physicsWorld.Solve(m_data.idleIterations, m_data.rayCastIterations);

			// Update physics-driven game objects.
			// Query phantoms.

			// Perform collision cast queries.
			ProcessQueries();
		}

		p.set_value();
	}

	//-----------------------------------------------------------------------------------------------
	// Processor and query methods.
	//-----------------------------------------------------------------------------------------------

	void CPhysics::ProcessColliderUpdates()
	{
		{ // Process deleted colliders.
			CVolume* pVolume;
			while(m_deletionQueue.TryPopFront(pVolume))
			{
				m_physicsWorld.RemoveVolume(pVolume);
			}
		}

		{ // Process inserted colliders.
			CVolume* pVolume;
			while(m_insertionQueue.TryPopFront(pVolume))
			{
				m_physicsWorld.AddVolume(pVolume);
			}
		}

		{ // Process dirty colliders.
			std::pair<CVolume*, Math::SIMDMatrix> elem;
			while(m_dirtyQueue.TryPopFront(elem))
			{
				m_physicsWorld.UpdateVolume(elem.first, elem.second);
			}
		}
	}

	void CPhysics::ProcessQueries()
	{
		{ // Process rays.
			QueryRay ray;
			while(m_queryRayQueue.TryPopFront(ray))
			{
				m_physicsWorld.CastRay(ray);
			}
		}
	}
};
