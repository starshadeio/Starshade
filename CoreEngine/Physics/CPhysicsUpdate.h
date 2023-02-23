//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CPhysicsUpdate.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPHYSICSUPDATE_H
#define CPHYSICSUPDATE_H

#include "../Objects/CVComponent.h"
#include "../Objects/CCompBatch.h"
#include <functional>

namespace Physics
{
	class CPhysicsUpdate;

	typedef CCompBatch<CPhysicsUpdate> CPhysicsUpdateBatch;
	typedef CCompBatch<CPhysicsUpdate>::CRef CPhysicsUpdateRef;

	class CPhysicsUpdate : public CVComponent
	{
	public:
		struct Data
		{
			std::function<void()> updateFunc;
		};

	public:
		CPhysicsUpdate();
		CPhysicsUpdate(const CPhysicsUpdate&) = delete;
		CPhysicsUpdate(CPhysicsUpdate&&) = default;
		CPhysicsUpdate& operator = (const CPhysicsUpdate&) = delete;
		CPhysicsUpdate& operator = (CPhysicsUpdate&&) = default;

		void Reset() final;
		bool TryUpdate(u8 frame) final { return CVComponent::TryUpdate(frame); }
		void Update() final;

		// Modifiers.
		inline void SetObject(const CVObject* pObject, CPhysicsUpdateRef* pRef)
		{
			m_pRef = pRef;

			if(pObject)
			{
				m_pObject = pObject;
				AddComponent();
			}
			else if(m_pObject)
			{
				RemoveComponent();
				m_pObject = pObject;
			}
		}

		inline void SetData(const Data& data) { m_data = data; }
		
		// Accessors.
		inline CPhysicsUpdateRef* GetRef() const { return m_pRef; }

	private:
		Data m_data;

		CPhysicsUpdateRef* m_pRef;
	};
};

#endif
