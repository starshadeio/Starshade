//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CPawn.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPAWN_H
#define CPAWN_H

#include "../Application/CInputData.h"
#include <Objects/CVObject.h>
#include <Logic/CTransform.h>
#include <Logic/CCamera.h>
#include <Physics/CVolume.h>
#include <Physics/CRigidbody.h>
#include <unordered_map>
#include <shared_mutex>

namespace Actor
{
	class CPawn : public CVObject
	{
	public:
		CPawn(const wchar_t* pName, u32 viewHash);
		virtual ~CPawn();
		CPawn(const CPawn&) = delete;
		CPawn(CPawn&&) = delete;
		CPawn& operator = (const CPawn&) = delete;
		CPawn& operator = (CPawn&&) = delete;
		
		virtual bool ProcessInput(u32 code, const App::InputCallbackData& callback) { return false; }

		bool RegisterMotor(class CMotor* pMotor);
		bool DeregisterMotor(u32 key);
		bool LoadMotor(u32 key);

		// Structural accessors.
		virtual inline const Logic::CCamera* GetCamera() const { return nullptr; }
		virtual inline const Logic::CTransform* GetHead() const { return nullptr; }
		virtual inline const Logic::CTransform* GetBody() const { return nullptr; }
		virtual inline const Logic::CTransform* GetPivot() const { return nullptr; }

		virtual inline const Physics::CRigidbody* GetRigidbody() const { return nullptr; }
		virtual inline const Physics::CVolume* GetVolume() const { return nullptr; }

		// Accessors.
		inline Math::SIMDVector GetPosition() const { std::shared_lock<std::shared_mutex> lk(m_mutex); return m_position; }
		inline Math::Vector3 GetEulerAngles() const { std::shared_lock<std::shared_mutex> lk(m_mutex); return m_euler; }

		// Modifiers.
		virtual inline void SetPosition(const Math::SIMDVector& position) { std::lock_guard<std::shared_mutex> lk(m_mutex); m_position = position; }
		virtual inline void SetEulerAngles(const Math::Vector3& euler) { std::lock_guard<std::shared_mutex> lk(m_mutex); m_euler = euler; }

	private:
		mutable std::shared_mutex m_mutex;
		
	protected:
		mutable std::mutex m_motorMutex;
		
		Math::Vector3 m_euler;
		Math::SIMDVector m_position;

		std::unordered_map<u32, class CMotor*> m_motorMap;

		class CMotor* m_pMotor;
	};
};

#endif
