//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Actors/CPlayerPawn.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPlayerPawn.h"
#include "../Application/CAppData.h"
#include "../Application/CApplication.h"
#include <Application/CSceneManager.h>
#include <Application/CInput.h>
#include <Application/CInputDeviceList.h>
#include <Application/CInputKeyboard.h>
#include <Application/CInputMouse.h>
#include <Application/CInputGamepad.h>
#include <Application/CPlatform.h>
#include <Factory/CFactory.h>
#include <Graphics/CDX12Graphics.h>
#include <Physics/CPhysics.h>
#include <Physics/CVolume.h>
#include <Utilities/CTimer.h>
#include <Logic/CCallback.h>

namespace Actor
{
	CPlayerPawn::CPlayerPawn(const wchar_t* pName, u32 sceneHash) :
		CVObject(pName, sceneHash),
		m_bLoaded(false),
		m_focusState(FocusState::None),
		m_euler(0.0f),
		m_panDelta(0.0f),
		m_zoomDelta(0.0f),
		m_rigidbody(this),
		m_volume(this),
		m_selector(L"Player Selector", sceneHash),
		m_transformBody(this),
		m_transformPivot(nullptr),
		m_transformHead(nullptr),
		m_camera(this, &m_transformHead)
	{
		memset(m_velocityAxes, 0, sizeof(m_velocityAxes));
	}

	CPlayerPawn::~CPlayerPawn() { }

	void CPlayerPawn::Initialize()
	{
		const u32 allLayouts[] = { App::INPUT_HASH_LAYOUT_EDITOR, App::INPUT_HASH_LAYOUT_PLAYER };

		// Movement.
		App::CInput::Instance().RegisterBinding(allLayouts, 2,
			App::InputBindingSet(App::INPUT_KEY_MOVE_FORWARD, std::bind(&CPlayerPawn::MoveForward, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_W), App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_UP))
		);

		App::CInput::Instance().RegisterBinding(allLayouts, 2,
			App::InputBindingSet(App::INPUT_KEY_MOVE_BACKWARD, std::bind(&CPlayerPawn::MoveBackward, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_S), App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_DOWN))
		);

		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_MOVE_UP, std::bind(&CPlayerPawn::MoveUp, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_Q), App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_NEXT))
		);

		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_MOVE_DOWN, std::bind(&CPlayerPawn::MoveDown, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_E), App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_PRIOR))
		);

		App::CInput::Instance().RegisterBinding(allLayouts, 2,
			App::InputBindingSet(App::INPUT_KEY_MOVE_LEFT, std::bind(&CPlayerPawn::MoveLeft, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_A), App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_LEFT))
		);

		App::CInput::Instance().RegisterBinding(allLayouts, 2,
			App::InputBindingSet(App::INPUT_KEY_MOVE_RIGHT, std::bind(&CPlayerPawn::MoveRight, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_D), App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_RIGHT))
		);

		// Look.
		App::CInput::Instance().RegisterBinding(allLayouts, 2,
			App::InputBindingSet(App::INPUT_KEY_MOUSE_H, std::bind(&CPlayerPawn::MouseHorizontal, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_DELTA_X), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		App::CInput::Instance().RegisterBinding(allLayouts, 2,
			App::InputBindingSet(App::INPUT_KEY_MOUSE_V, std::bind(&CPlayerPawn::MouseVertical, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_DELTA_Y), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		// Actions.
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_SELECT, std::bind(&CPlayerPawn::Select, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_LEFT), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_LOOK, std::bind(&CPlayerPawn::Look, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_RIGHT), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_PAN, std::bind(&CPlayerPawn::Pan, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_MIDDLE), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_ZOOM, std::bind(&CPlayerPawn::Zoom, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_WHEEL), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_MODE, std::bind(&CPlayerPawn::Mode, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_R), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(allLayouts, 2,
			App::InputBindingSet(App::INPUT_KEY_PLAY, std::bind(&CPlayerPawn::Play, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_G), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_PLAYER, 1,
			App::InputBindingSet(App::INPUT_KEY_JUMP, std::bind(&CPlayerPawn::Jump, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_SPACE), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(allLayouts, 2,
			App::InputBindingSet(App::INPUT_KEY_QUIT, std::bind(&CPlayerPawn::Quit, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_ESCAPE), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		// Setup the camera.
		m_transformPivot.SetParent(&m_transformBody);
		m_transformHead.SetParent(&m_transformPivot);

		memset(&m_cameraData, 0, sizeof(m_cameraData));
		m_cameraData.projMode = Logic::CAMERA_PROJ_MODE_PERSPECTIVE;
		m_cameraData.nearView = 0.1f;
		m_cameraData.farView = 100.0f;
		m_cameraData.fov = 65.0f * Math::g_PiOver180;
		m_cameraData.aspectRatio = CFactory::Instance().GetGraphicsAPI()->GetAspectRatio();
		m_camera.SetupProjection(m_cameraData);
		App::CSceneManager::Instance().CameraManager().SetDefault(&m_camera);

		m_transformBody.SetPosition(Math::SIMDVector(0.0f, 18.0f, -32.0f));
		m_transformHead.SetLocalEuler(Math::SIMDVector(m_euler.x = Math::g_PiOver180 * 35.0f, 0.0f, m_euler.z));

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

		{ // Create player selector.
			Actor::CPlayerSelector::Data data { };
			m_selector.SetData(data);
			m_selector.Initialize();
		}
	}

	void CPlayerPawn::Update()
	{
		const bool bPlaying = App::CApplication::Instance().State().IsPlaying();

		m_selector.Update(bPlaying);
		if(bPlaying)
		{
			PlayUpdate();
		}
		else
		{
			EditUpdate();
		}
	}

	void CPlayerPawn::LateUpdate()
	{
		if(m_bLoaded)
		{
			UpdatePlayModeState(App::CApplication::Instance().State().IsPlaying());
			m_bLoaded = false;
		}
	}
	
	// Method for updating the player in edit mode.
	void CPlayerPawn::EditUpdate()
	{
		// Calculate target velocity.
		Math::Vector3 targetVelocity;
		const Math::Vector3 velocitySum = Math::Vector3(m_velocityAxes[0].x + m_velocityAxes[0].y, m_velocityAxes[1].x + m_velocityAxes[1].y, m_velocityAxes[2].x + m_velocityAxes[2].y);
		const float len = Math::Vector3::LengthSq(velocitySum);
		if(len > 1.0f)
		{
			targetVelocity = velocitySum / sqrtf(len);
		}
		else
		{
			targetVelocity = velocitySum;
		}

		targetVelocity *= m_data.speed * Util::CTimer::Instance().GetDelta();

		m_transformBody.SetPosition(m_transformBody.GetPosition() +
			m_transformHead.GetRight() * (targetVelocity.x + m_panDelta.x * Util::CTimer::Instance().GetDelta()) +
			m_transformHead.GetUp() * (targetVelocity.y + m_panDelta.y * Util::CTimer::Instance().GetDelta()) +
			m_transformHead.GetForward() * (targetVelocity.z + m_zoomDelta)
		);

		m_panDelta = 0;
		m_zoomDelta = 0.0f;

		m_transformPivot.SetEuler(Math::SIMDVector(0.0f, m_euler.y, 0.0f));
		m_transformHead.SetLocalEuler(Math::SIMDVector(m_euler.x, 0.0f, m_euler.z));

		{ // Calculate picking ray.
			const Math::Vector2 mousePos = App::CInput::Instance().GetDeviceList()->GetMouse()->GetMousePosition();
			Math::SIMDVector coord(
				(mousePos.x / CFactory::Instance().GetGraphicsAPI()->GetWidth() - 0.5f) * 2.0f / m_camera.GetProjectionMatrix().rows[0].m128_f32[0],
				-(mousePos.y / CFactory::Instance().GetGraphicsAPI()->GetHeight() - 0.5f) * 2.0f  / m_camera.GetProjectionMatrix().rows[1].m128_f32[1],
				1.0f
			);

			coord = m_camera.GetViewMatrixInv().TransformNormal(coord).Normalized();

			m_selector.SetRay(m_transformBody.GetPosition(), coord);
		}
	}
	
	// Method for updating the player in play mode.
	void CPlayerPawn::PlayUpdate()
	{
		m_rigidbody.UpdateTransform(m_transformBody);
		
		m_transformPivot.SetEuler(Math::SIMDVector(0.0f, m_euler.y, 0.0f));
		m_transformHead.SetLocalEuler(Math::SIMDVector(m_euler.x, 0.0f, m_euler.z));
	}

	// Physics thread update.
	void CPlayerPawn::PhysicsUpdate()
	{
		if(App::CApplication::Instance().State().IsPlaying())
		{
			// Calculate target velocity.
			Math::Vector3 targetVelocity, velocitySum;
			float eulerY;

			{
				std::lock_guard<std::mutex> lk(m_velocityMutex);
				velocitySum = Math::Vector3(m_velocityAxes[0].x + m_velocityAxes[0].y, m_velocityAxes[1].x + m_velocityAxes[1].y, m_velocityAxes[2].x + m_velocityAxes[2].y);
				eulerY = m_euler.y;
			}

			const float len = Math::Vector3::LengthSq(velocitySum);
			if(len > 1.0f)
			{
				targetVelocity = velocitySum / sqrtf(len);
			}
			else
			{
				targetVelocity = velocitySum;
			}
				
			Math::Vector3 right(cosf(eulerY), 0.0f, -sinf(eulerY));
			Math::Vector3 forward(sinf(eulerY), 0.0f, cosf(eulerY));
			targetVelocity = (right * targetVelocity.x + forward * targetVelocity.z) * m_data.speed;
			
			if(!m_rigidbody.OnGround())
			{
				targetVelocity = Math::Vector3::Lerp(Math::Vector3(m_rigidbody.GetVelocity()[0], 0.0f, m_rigidbody.GetVelocity()[2]), 
					targetVelocity, std::min(1.0f, Util::CTimer::Instance().GetDelta() * 2.0f));
			}

			// Apply velocity and forces to rigidbody.
			m_rigidbody.SetVelocity(Math::SIMD_VEC_UP * m_rigidbody.GetVelocity()[1] + targetVelocity);

			if(m_bJump)
			{
				if(m_rigidbody.OnGround())
				{
					m_rigidbody.AddForce(Math::SIMD_VEC_UP * m_data.jumpPower);
				}

				m_bJump = false;
			}
		}
		else
		{
			m_selector.PhysicsUpdate();
		}
	}

	void CPlayerPawn::Release()
	{
		m_selector.Release();

		Physics::CPhysics::Instance().DestroyPhysicsUpdate(&m_physicsUpdate);
	}

	void CPlayerPawn::OnResize(const App::CPanel* pPanel)
	{
		m_cameraData.aspectRatio = CFactory::Instance().GetGraphicsAPI()->GetAspectRatio();
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
		m_bLoaded = true;
	}

	//-----------------------------------------------------------------------------------------------
	// Input Callbacks.
	//-----------------------------------------------------------------------------------------------

	// Movement.
	void CPlayerPawn::MoveForward(const App::InputCallbackData& callback)
	{
		std::lock_guard<std::mutex> lk(m_velocityMutex);
		m_velocityAxes[2].x = callback.value;
	}

	void CPlayerPawn::MoveBackward(const App::InputCallbackData& callback)
	{
		std::lock_guard<std::mutex> lk(m_velocityMutex);
		m_velocityAxes[2].y = -callback.value;
	}

	void CPlayerPawn::MoveUp(const App::InputCallbackData& callback)
	{
		std::lock_guard<std::mutex> lk(m_velocityMutex);
		m_velocityAxes[1].x = callback.value;
	}

	void CPlayerPawn::MoveDown(const App::InputCallbackData& callback)
	{
		std::lock_guard<std::mutex> lk(m_velocityMutex);
		m_velocityAxes[1].y = -callback.value;
	}

	void CPlayerPawn::MoveLeft(const App::InputCallbackData& callback)
	{
		std::lock_guard<std::mutex> lk(m_velocityMutex);
		m_velocityAxes[0].x = -callback.value;
	}

	void CPlayerPawn::MoveRight(const App::InputCallbackData& callback)
	{
		std::lock_guard<std::mutex> lk(m_velocityMutex);
		m_velocityAxes[0].y = callback.value;
	}

	// Look.
	void CPlayerPawn::MouseHorizontal(const App::InputCallbackData& callback)
	{
		if(m_mouseDeltaFunc) { m_mouseDeltaFunc(0, callback.value); }
	}

	void CPlayerPawn::MouseVertical(const App::InputCallbackData& callback)
	{
		if(m_mouseDeltaFunc) { m_mouseDeltaFunc(1, callback.value); }
	}

	// Actions.
	void CPlayerPawn::Select(const App::InputCallbackData& callback) 
	{
		m_selector.OnSelect(callback);
	}

	void CPlayerPawn::Look(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			if(m_focusState != FocusState::None) { return; }

			CFactory::Instance().GetPlatform()->LockCursor(CFactory::Instance().GetPlatform()->GetMainPanel());
			CFactory::Instance().GetPlatform()->SetCursorVisibility(false);
			m_mouseDeltaFunc = std::bind(&CPlayerPawn::AxisLook, this, std::placeholders::_1, std::placeholders::_2);
			m_focusState = FocusState::Look;
		}
		else
		{
			if(m_focusState != FocusState::Look) { return; }
			CFactory::Instance().GetPlatform()->UnlockCursor();
			CFactory::Instance().GetPlatform()->SetCursorVisibility(true);
			m_mouseDeltaFunc = nullptr;
			m_focusState = FocusState::None;
		}
	}

	void CPlayerPawn::Pan(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			if(m_focusState != FocusState::None) { return; }

			CFactory::Instance().GetPlatform()->LockCursor(CFactory::Instance().GetPlatform()->GetMainPanel());
			CFactory::Instance().GetPlatform()->SetCursorVisibility(false);
			m_mouseDeltaFunc = std::bind(&CPlayerPawn::AxisPan, this, std::placeholders::_1, std::placeholders::_2);
			m_focusState = FocusState::Pan;
		}
		else
		{
			if(m_focusState != FocusState::Pan) { return; }

			CFactory::Instance().GetPlatform()->UnlockCursor();
			CFactory::Instance().GetPlatform()->SetCursorVisibility(true);
			m_mouseDeltaFunc = nullptr;
			m_focusState = FocusState::None;
		}
	}

	void CPlayerPawn::Zoom(const App::InputCallbackData& callback)
	{
		m_zoomDelta += callback.value * m_data.zoomRate;
	}

	void CPlayerPawn::Mode(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			m_selector.ToggleSelectionType();
		}
	}
	
	void CPlayerPawn::Play(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			UpdatePlayModeState(App::CApplication::Instance().State().TogglePlayMode());
		}
	}

	void CPlayerPawn::Jump(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			m_bJump = true;
		}
	}

	void CPlayerPawn::Quit(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CFactory::Instance().GetPlatform()->Quit();
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	void CPlayerPawn::AxisLook(size_t axis, float delta)
	{
		delta *= Math::g_PiOver180;
		if(axis == 0)
		{
			std::lock_guard<std::mutex> lk(m_velocityMutex);
			m_euler.y = fmod(m_euler.y + delta * m_data.lookRate.x, Math::g_2Pi);
		}
		else
		{
			std::lock_guard<std::mutex> lk(m_velocityMutex);
			m_euler.x = Math::Clamp(m_euler.x + delta * m_data.lookRate.y, -Math::g_PiOver2, Math::g_PiOver2);
		}
	}
	
	void CPlayerPawn::AxisPan(size_t axis, float delta)
	{
		if(axis == 0)
		{
			m_panDelta.x -= m_data.panRate.x * delta;
		}
		else
		{
			m_panDelta.y += m_data.panRate.y * delta;
		}
	}

	void CPlayerPawn::UpdatePlayModeState(bool bPlay)
	{
		if(bPlay)
		{
			CFactory::Instance().GetPlatform()->LockCursor(CFactory::Instance().GetPlatform()->GetMainPanel());
			CFactory::Instance().GetPlatform()->SetCursorVisibility(false);

			m_velocityAxes[1] = 0.0f;
			m_mouseDeltaFunc = std::bind(&CPlayerPawn::AxisLook, this, std::placeholders::_1, std::placeholders::_2);

			m_volume.Register(m_transformBody.GetWorldMatrix());
		}
		else
		{
			m_volume.Deregister();
			CFactory::Instance().GetPlatform()->UnlockCursor();
			CFactory::Instance().GetPlatform()->SetCursorVisibility(true);

			m_mouseDeltaFunc = nullptr;
		}
	}
};
