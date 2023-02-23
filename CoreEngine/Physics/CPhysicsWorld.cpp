//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CPhysicsWorld.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPhysicsWorld.h"
#include "CVolume.h"
#include "CRigidbody.h"
#include "CForceField.h"
#include "../Objects/CVObject.h"
#include <Windows.h>

namespace Physics
{
	CPhysicsWorld::CPhysicsWorld() { }

	CPhysicsWorld::~CPhysicsWorld() { }
		
	//-----------------------------------------------------------------------------------------------
	// Volume (de)registration methods.
	//-----------------------------------------------------------------------------------------------

	void CPhysicsWorld::AddVolume(CVolume* pVolume)
	{
		m_volumeMap.insert({ pVolume->GetVObject()->GetHash(), pVolume });
		if(pVolume->AllowsRays()) m_rayCastMap.insert({ pVolume->GetVObject()->GetHash(), pVolume });
		if(pVolume->IsCollider()) m_colliderMap.insert({ pVolume->GetVObject()->GetHash(), pVolume });
		if(pVolume->GetRigidbody()) m_rigidbodyMap.insert({ pVolume->GetVObject()->GetHash(), pVolume });
		if(pVolume->GetForceField()) m_forceFieldMap.insert({ pVolume->GetVObject()->GetHash(), pVolume });
	}

	void CPhysicsWorld::RemoveVolume(CVolume* pVolume)
	{
		m_volumeMap.erase(pVolume->GetVObject()->GetHash());
		if(pVolume->AllowsRays()) m_rayCastMap.erase(pVolume->GetVObject()->GetHash());
		if(pVolume->IsCollider()) m_colliderMap.erase(pVolume->GetVObject()->GetHash());
		if(pVolume->GetRigidbody()) m_rigidbodyMap.erase(pVolume->GetVObject()->GetHash());
		if(pVolume->GetForceField()) m_forceFieldMap.erase(pVolume->GetVObject()->GetHash());
	}
	
	//-----------------------------------------------------------------------------------------------
	// Recalculation.
	//-----------------------------------------------------------------------------------------------

	void CPhysicsWorld::UpdateVolume(CVolume* pVolume, const Math::SIMDMatrix& world)
	{
		pVolume->Recalculate(world);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Update methods.
	//-----------------------------------------------------------------------------------------------

	void CPhysicsWorld::UpdateForceFields()
	{
		for(auto elem : m_forceFieldMap)
		{
			elem.second->GetForceField()->PhysicsUpdate();
		}
	}

	void CPhysicsWorld::UpdateRigidbodies()
	{
		for(auto rigidbody : m_rigidbodyMap)
		{
			rigidbody.second->GetRigidbody()->Calculate();
			rigidbody.second->GetRigidbody()->SetupIdleSolver();
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Solver.
	//-----------------------------------------------------------------------------------------------
	
	void CPhysicsWorld::Solve(u32 idleIterations, u32 rayCastIterations)
	{
		bool bAnyResponse;
		u32 i, j;

		// Idle solver iterations.
		for(i = 0; i < idleIterations; ++i)
		{
			bAnyResponse = false;
			for(auto rigidbody : m_rigidbodyMap)
			{
				for(auto collider : m_colliderMap)
				{
					if(collider.second != rigidbody.second)
					{
						bAnyResponse |= collider.second->IdleSolver(rigidbody.second);
					}
				}
			}

			if(!bAnyResponse) { break; }
		}
		
		// Apply idle iterations, and prep for ray cast iterations.
		for(auto rigidbody : m_rigidbodyMap)
		{
			rigidbody.second->GetRigidbody()->ApplyIdleSolver();
			rigidbody.second->GetRigidbody()->ResetSolverState();
		}

		// Perform ray cast iterations.
		for(j = 0; j < rayCastIterations; ++j)
		{
			bAnyResponse = false;
			for(auto rigidbody : m_rigidbodyMap)
			{
				if(_mm_cvtss_f32(rigidbody.second->GetRigidbody()->GetVelocity().LengthSq()) > 1e-10f)
				{
					for(auto collider : m_colliderMap)
					{
						if(collider.second != rigidbody.second)
						{
							collider.second->MotionSolver(rigidbody.second);
						}
					}

					bAnyResponse |= rigidbody.second->GetRigidbody()->Response();
				}
			}

			if(!bAnyResponse) { break; }
		}

		// If max ray cast iterations are reached, reset and perform a single final ray cast for rigidbodies that are still invalid.
		if(j >= rayCastIterations)
		{
			for(auto rigidbody : m_rigidbodyMap)
			{
				if(rigidbody.second->GetRigidbody()->IsValid()) continue;
				if(_mm_cvtss_f32(rigidbody.second->GetRigidbody()->GetVelocity().LengthSq()) > 1e-10f)
				{
					rigidbody.second->GetRigidbody()->ResetSolverState();

					for(auto collider : m_colliderMap)
					{
						if(collider.second != rigidbody.second)
						{
							collider.second->MotionSolver(rigidbody.second);
						}
					}

					rigidbody.second->GetRigidbody()->Response();
				}
			}
		}
		
		// Apply ray cast adjustments, and perform some idle iterations to make sure that are resting in the right spopt
		//  TODO: Try to remove the need for the addition idle iterations in the future.
		for(auto rigidbody : m_rigidbodyMap)
		{
			rigidbody.second->GetRigidbody()->Apply([&](){
				rigidbody.second->GetRigidbody()->SetupIdleSolver();
				
				bool bSolved = false;
				for(i = 0; !bSolved && i < idleIterations; ++i)
				{
					bSolved = true;

					for(auto collider : m_colliderMap)
					{
						if(collider.second != rigidbody.second)
						{
							bSolved &= !collider.second->IdleSolver(rigidbody.second);
						}
					}
				}
			
				rigidbody.second->GetRigidbody()->ApplyIdleSolver();
			});
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Query methods.
	//-----------------------------------------------------------------------------------------------

	void CPhysicsWorld::CastRay(const QueryRay& queryRay)
	{
		std::vector<RaycastInfo> res;
		RaycastInfo info;
		for(auto elem : m_rayCastMap)
		{
			if(elem.second->RayTest(queryRay, info))
			{
				res.push_back(info);
			}
		}

		queryRay.callback(res);
	}
};
