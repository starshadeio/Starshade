//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Actors/CPlayerPawn.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPLAYERPAWN_H
#define CPLAYERPAWN_H

#include "CPlayerSelector.h"
#include <Objects/CVObject.h>
#include <Logic/CCamera.h>
#include <Logic/CTransform.h>
#include <Logic/CCallback.h>
#include <Physics/CPhysicsUpdate.h>
#include <Physics/CPhysicsData.h>
#include <Physics/CVolumeCapsule.h>
#include <Physics/CVolumeSphere.h>
#include <Physics/CVolumeOBB.h>
#include <Physics/CRigidbody.h>
#include <Math/CMathVector2.h>
#include <Math/CSIMDRay.h>
#include <Application/CInputData.h>
#include <functional>
#include <mutex>
#include <fstream>

namespace Actor
{
	class CPlayerPawn : public CVObject
	{
	private:
		enum class FocusState
		{
			None,
			Look,
			Pan,
		};
		
	public:
		struct Data
		{
			float speed = 1.0f;
			float jumpPower = 1.0f;
			Math::Vector2 lookRate = 1.0f;
			Math::Vector2 panRate = 1.0f;
			float zoomRate = 1.0f;
		};

	public:
		CPlayerPawn(const wchar_t* pName, u32 sceneHash);
		~CPlayerPawn();

		CPlayerPawn(const CPlayerPawn&) = delete;
		CPlayerPawn(CPlayerPawn&&) = delete;
		CPlayerPawn& operator = (const CPlayerPawn&) = delete;
		CPlayerPawn& operator = (CPlayerPawn&&) = delete;

		void Initialize() final;
		void Update() final;
		void LateUpdate() final;
		void Release() final;
		
		void SaveToFile(std::ofstream& file) const;
		void LoadFromFile(std::ifstream& file);

		void OnResize(const App::CPanel* pPanel) final;
		
		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void EditUpdate();
		void PlayUpdate();

		void PhysicsUpdate();

	private:
		void MoveForward(const App::InputCallbackData& callback);
		void MoveBackward(const App::InputCallbackData& callback);
		void MoveUp(const App::InputCallbackData& callback);
		void MoveDown(const App::InputCallbackData& callback);
		void MoveLeft(const App::InputCallbackData& callback);
		void MoveRight(const App::InputCallbackData& callback);

		void MouseHorizontal(const App::InputCallbackData& callback);
		void MouseVertical(const App::InputCallbackData& callback);

		void Select(const App::InputCallbackData& callback);
		void Look(const App::InputCallbackData& callback);
		void Pan(const App::InputCallbackData& callback);
		void Zoom(const App::InputCallbackData& callback);
		void Mode(const App::InputCallbackData& callback);
		void Play(const App::InputCallbackData& callback);
		void Jump(const App::InputCallbackData& callback);
		void Quit(const App::InputCallbackData& callback);

		void AxisLook(size_t axis, float delta);
		void AxisPan(size_t axis, float delta);

		void UpdatePlayModeState(bool bPlay);

	private:
		std::mutex m_mutex;
		std::mutex m_pickedMutex;
		std::mutex m_velocityMutex;

		bool m_bLoaded;

		FocusState m_focusState;
		Math::Vector2 m_velocityAxes[3];
		Math::Vector3 m_euler;
		Math::Vector2 m_panDelta;

		Abool m_bJump;
		float m_zoomDelta;

		std::function<void(size_t, float)> m_mouseDeltaFunc;

		Data m_data;
		
		Physics::CRigidbody m_rigidbody;
		Physics::CVolumeCapsule m_volume;

		Logic::CTransform m_transformBody;
		Logic::CTransform m_transformPivot;
		Logic::CTransform m_transformHead;
		Logic::CCamera::Data m_cameraData;
		Logic::CCamera m_camera;
		Physics::CPhysicsUpdateRef m_physicsUpdate;

		CPlayerSelector m_selector;
	};
};

#endif
