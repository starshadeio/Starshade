//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CPlayerPawn.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPLAYERPAWN_H
#define CPLAYERPAWN_H

#include "CPawn.h"
#include <Logic/CTransform.h>
#include <Logic/CCamera.h>
#include <Logic/CCallback.h>
#include <Physics/CPhysicsUpdate.h>
#include <Physics/CPhysicsData.h>
#include <Physics/CRigidbody.h>
#include <Physics/CVolumeCapsule.h>

namespace App
{
	class CPanel;
};

namespace Actor
{
	class CPlayerPawn : public CPawn
	{
	public:
		struct Data
		{
			float speed = 1.0f;
			float jumpPower = 1.0f;
			
			Math::Vector3 startEuler;
			Math::SIMDVector startPosition;
		};

	public:
		CPlayerPawn(const wchar_t* pName, u32 viewHash);
		virtual ~CPlayerPawn();
		CPlayerPawn(const CPlayerPawn&) = delete;
		CPlayerPawn(CPlayerPawn&&) = delete;
		CPlayerPawn& operator = (const CPlayerPawn&) = delete;
		CPlayerPawn& operator = (CPlayerPawn&&) = delete;
		
		void Initialize() final;
		void Update() final;
		void Release() final;
		
		void OnResize(const Math::Rect& rect) final;
		
		void SaveToFile(std::ofstream& file) const;
		void LoadFromFile(std::ifstream& file);

		bool ProcessInput(u32 code, const App::InputCallbackData& callback);

		void RegisterPhysics();
		void DeregisterPhysics();

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		
		// Structural accessors.
		inline const Logic::CCamera* GetCamera() const final { return &m_camera; }
		inline const Logic::CTransform* GetHead() const final { return &m_transformHead; }
		inline const Logic::CTransform* GetBody() const final { return &m_transformBody; }
		inline const Logic::CTransform* GetPivot() const final { return &m_transformPivot; }

		inline const Physics::CRigidbody* GetRigidbody() const final { return &m_rigidbody; }
		inline const Physics::CVolume* GetVolume() const final { return &m_volume; }

		inline void SetPosition(const Math::SIMDVector& position) final
		{
			CPawn::SetPosition(position);
			m_transformBody.SetPosition(m_position);
		}

	private:
		void PhysicsUpdate();

	private:
		Data m_data;
		
		Physics::CRigidbody m_rigidbody;
		Physics::CVolumeCapsule m_volume;
		Physics::CPhysicsUpdateRef m_physicsUpdate;

		Logic::CTransform m_transformBody;
		Logic::CTransform m_transformPivot;
		Logic::CTransform m_transformHead;
		Logic::CCamera::Data m_cameraData;
		Logic::CCamera m_camera;
	};
};

#endif
