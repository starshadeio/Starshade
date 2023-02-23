//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CPhysics.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPHYSICS_H
#define CPHYSICS_H

#include "CPhysicsWorld.h"
#include "CPhysicsUpdate.h"
#include "CPhysicsData.h"
#include "../Globals/CGlobals.h"
#include "../Objects/CVObject.h"
#include "../Utilities/CTSDeque.h"
#include <future>
#include <mutex>
#include <unordered_map>

namespace Physics
{
	class CPhysics
	{
	public:
		friend class CVolume;

	public:
		struct Data
		{
			float targetFPS;
			u32 idleIterations;
			u32 rayCastIterations;
			Math::SIMDVector gravity;
		};

	public:
		static CPhysics& Instance()
		{
			static CPhysics instance;
			return instance;
		}

	private:
		CPhysics();
		~CPhysics();
		CPhysics(const CPhysics&) = delete;
		CPhysics(CPhysics&&) = delete;
		CPhysics& operator = (const CPhysics&) = delete;
		CPhysics& operator = (CPhysics&&) = delete;

	public:
		void Initialize();
		void Halt();
		void Release();

		void CastRay(const QueryRay& query);

		void MarkObjectAsDirty(const CVObject* pObject, const Math::SIMDMatrix& world);

		// Registrars.
		inline void CreatePhysicsUpdate(CPhysicsUpdateRef* pRef, const CVObject* pObject) { m_physicsUpdateBatch.Pull(pRef, pObject); }
		inline void DestroyPhysicsUpdate(CPhysicsUpdateRef* pRef) { m_physicsUpdateBatch.Free(pRef); }

		// Accessors.
		inline const Data& GetData() const { return m_data; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void RegisterVolume(class CVolume* pVolume);
		void DeregisterVolume(class CVolume* pVolume);

	private:
		void PhysicsThread(std::promise<void> p);

		void ProcessColliderUpdates();
		void ProcessQueries();

	private:
		mutable std::mutex m_mutex;

		Data m_data;

		CPhysicsUpdateBatch m_physicsUpdateBatch;
		std::unordered_map<u64, class CVolume*> m_volumeMap;
		Util::CTSDeque<class CVolume*> m_insertionQueue;
		Util::CTSDeque<class CVolume*> m_deletionQueue;
		Util::CTSDeque<std::pair<class CVolume*, Math::SIMDMatrix>> m_dirtyQueue;
		Util::CTSDeque<QueryRay> m_queryRayQueue;

		Abool m_exitFlag;
		std::future<void> m_futureExit;

		CPhysicsWorld m_physicsWorld;
	};
};

#endif
