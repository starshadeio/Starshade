//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CPlayerPawn.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPlayerPawn.h"
#include "CMotor.h"
#include "../Factory/CFactory.h"
#include "../Application/CSceneManager.h"
#include "../Graphics/CGraphicsAPI.h"
#include <Physics/CPhysics.h>
#include <Physics/CVolume.h>

namespace Actor
{
	CPlayerPawn::CPlayerPawn(const wchar_t* pName, u32 viewHash) :
		CPawn(pName, viewHash),
		m_rigidbody(this),
		m_volume(this),
		m_transformBody(this),
		m_transformPivot(nullptr),
		m_transformHead(nullptr),
		m_camera(this, &m_transformHead)
	{
	}

	CPlayerPawn::~CPlayerPawn()
	{
	}
	
	void CPlayerPawn::Initialize()
	{
		// Setup the camera.
		m_transformPivot.SetParent(&m_transformBody);
		m_transformHead.SetParent(&m_transformPivot);

		memset(&m_cameraData, 0, sizeof(m_cameraData));
		m_cameraData.projMode = Logic::CAMERA_PROJ_MODE_PERSPECTIVE;
		m_cameraData.nearView = 0.1f;
		m_cameraData.farView = 100.0f;
		m_cameraData.fov = 65.0f * Math::g_PiOver180;
		
		m_cameraData.screenRect = { 0, 0, static_cast<long>(CFactory::Instance().GetGraphicsAPI()->GetWidth()), static_cast<long>(CFactory::Instance().GetGraphicsAPI()->GetHeight()) };
		m_cameraData.aspectRatio = CFactory::Instance().GetGraphicsAPI()->GetAspectRatio();

		m_camera.SetupProjection(m_cameraData);
		App::CSceneManager::Instance().CameraManager().SetDefault(&m_camera);

		m_transformBody.SetPosition(m_data.startPosition);
		m_transformPivot.SetEuler(Math::SIMDVector(0.0f, m_data.startEuler.y, 0.0f));
		m_transformHead.SetLocalEuler(Math::SIMDVector(m_data.startEuler.x, 0.0f, m_data.startEuler.z));

		m_euler = m_data.startEuler;
		m_position = m_data.startPosition;
		
		{ // Setup play mode rigid body.
			Physics::CRigidbody::Data data { };
			data.SetMass(1.0f);
			data.damping = 0.95f;
			data.pVolume = &m_volume;
			m_rigidbody.SetData(data);
		}

		{ // Setup play mode collider.
			Physics::CVolumeCapsule::Data data { };
			data.radius = 0.4f;
			data.height = std::max(0.0f, 1.85f - data.radius * 2.0f);
			data.offset = -0.75f;
			data.colliderType = Physics::ColliderType::Collider;
			data.pRigidbody = &m_rigidbody;
			m_volume.SetData(data);
		}

		{ // Create a physic updater. This is called from the physics thread.
			Physics::CPhysics::Instance().CreatePhysicsUpdate(&m_physicsUpdate, this);
			Physics::CPhysicsUpdate::Data data { };
			data.updateFunc = std::bind(&CPlayerPawn::PhysicsUpdate, this);
			m_physicsUpdate.pRef->SetData(data);
		}
	}

	void CPlayerPawn::Update()
	{
		if(m_pMotor == nullptr) return;
		m_pMotor->Update();

		if(m_pMotor->UsePhysics())
		{
			m_rigidbody.UpdateTransform(m_transformBody);
		}
		else
		{
			m_transformBody.SetPosition(m_position);
		}

		m_transformPivot.SetEuler(Math::SIMDVector(0.0f, m_euler.y, 0.0f));
		m_transformHead.SetLocalEuler(Math::SIMDVector(m_euler.x, 0.0f, m_euler.z));
	}

	void CPlayerPawn::PhysicsUpdate()
	{
		std::lock_guard<std::mutex> lk(m_motorMutex);
		if(m_pMotor == nullptr) return;
		m_pMotor->PhysicsUpdate(m_rigidbody);
	}

	void CPlayerPawn::Release()
	{
		Physics::CPhysics::Instance().DestroyPhysicsUpdate(&m_physicsUpdate);
	}

	void CPlayerPawn::OnResize(const Math::Rect& rect)
	{
		m_cameraData.screenRect = { 0, 0, rect.w, rect.h };
		m_cameraData.aspectRatio = static_cast<float>(rect.w) / static_cast<float>(rect.h);
		m_camera.SetupProjection(m_cameraData);
	}
	
	//-----------------------------------------------------------------------------------------------
	// File methods.
	//-----------------------------------------------------------------------------------------------

	void CPlayerPawn::SaveToFile(std::ofstream& file) const
	{
	}

	void CPlayerPawn::LoadFromFile(std::ifstream& file)
	{
	}
	
	//-----------------------------------------------------------------------------------------------
	// Input methods.
	//-----------------------------------------------------------------------------------------------

	bool CPlayerPawn::ProcessInput(u32 code, const App::InputCallbackData& callback)
	{
		if(m_pMotor == nullptr) return false;

		if(App::CSceneManager::Instance().UIEventSystem().ProcessInput(code, callback))
		{
			return true;
		}
		else
		{
			return m_pMotor->ProcessInput(code, callback);
		}
	}

	
	//-----------------------------------------------------------------------------------------------
	// Physics methods.
	//-----------------------------------------------------------------------------------------------

	void CPlayerPawn::RegisterPhysics()
	{
		m_volume.Register(m_transformBody.GetWorldMatrix());
	}

	void CPlayerPawn::DeregisterPhysics()
	{
		m_volume.Deregister();
	}
};
