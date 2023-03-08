//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CMotor.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMOTOR_H
#define CMOTOR_H

#include "CPawn.h"
#include "../Application/CInputData.h"
#include <Physics/CRigidbody.h>
#include <Objects/CVComponent.h>
#include <fstream>
#include <unordered_set>
#include <cassert>

namespace Actor
{
	class CMotor : public CVComponent
	{
	protected:
		struct BaseData
		{
		public:
			friend class CMotor;

		protected:
			bool bUsePhysics;
		};

	public:
		CMotor(const CVObject* pObject, u32 hash) : 
			CVComponent(pObject),
			m_hash(hash),
			m_pPawn(dynamic_cast<CPawn*>(const_cast<CVObject*>(pObject))),
			m_bLoaded(false) { }

		virtual ~CMotor() { }
		CMotor(const CMotor&) = delete;
		CMotor(CMotor&&) = delete;
		CMotor& operator = (const CMotor&) = delete;
		CMotor& operator = (CMotor&&) = delete;

		virtual void Load() { m_bLoaded = true; }
		virtual void Unload() { m_bLoaded = false; }
		
		void Initialize() override { }

		void Update() override { }
		virtual void PhysicsUpdate(Physics::CRigidbody& rigidbody) { }
		
		void Release() override { }

		virtual void SaveToFile(std::ofstream& file) const { }
		virtual void LoadFromFile(std::ifstream& file) { }

		virtual bool ProcessInput(u32 code, const App::InputCallbackData& callback) { return false; }

		// Accessors.
		inline u32 GetHash() const { return m_hash; }

		inline bool IsLoaded() const { return m_bLoaded; }
		inline bool UsePhysics() const { return GetBaseData().bUsePhysics; }

	protected:
		virtual inline const BaseData& GetBaseData() const = 0;

	protected:
		CPawn* m_pPawn;

	private:
		bool m_bLoaded;
		u32 m_hash;
	};
};

#endif
