//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Actors/CUser.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUser.h"
#include "../Application/CEditor.h"
#include "../Application/CAppData.h"
#include "../Application/CApplication.h"
#include "../Application/CGizmoPivot.h"
#include <Actors/CSpawner.h>
#include <Application/CSceneManager.h>
#include <Application/CCommandManager.h>
#include <Application/CInput.h>
#include <Application/CInputDeviceList.h>
#include <Application/CInputKeyboard.h>
#include <Application/CInputMouse.h>
#include <Application/CInputGamepad.h>
#include <Application/CPlatform.h>
#include <Graphics/CGraphicsAPI.h>
#include <Factory/CFactory.h>
#include <Math/CMathFNV.h>
#include <sstream>

#include <Windows.h>

namespace Actor
{
	CUser::CUser(const wchar_t* pName, u32 viewHash) : 
		CPlayer(pName, viewHash),
		m_motorMonitor(&m_pawn)
	{
	}

	CUser::~CUser()
	{
	}
		
	void CUser::Initialize()
	{
		CPlayer::Initialize();

		// Movement.
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_MOVE_FORWARD, std::bind(&CUser::MoveForward, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_UP), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_MOVE_BACKWARD, std::bind(&CUser::MoveBackward, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_DOWN), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_MOVE_UP, std::bind(&CUser::MoveUp, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_PRIOR), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_MOVE_DOWN, std::bind(&CUser::MoveDown, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_NEXT), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_MOVE_LEFT, std::bind(&CUser::MoveLeft, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_LEFT), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_MOVE_RIGHT, std::bind(&CUser::MoveRight, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_RIGHT), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		// Look.
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_LOOK_H, std::bind(&CUser::LookHorizontal, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_DELTA_X), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_LOOK_V, std::bind(&CUser::LookVertical, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_DELTA_Y), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		// Actions.
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_SELECT, std::bind(&CUser::Select, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_LEFT), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_LOOK, std::bind(&CUser::Look, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_RIGHT), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_PAN, std::bind(&CUser::Pan, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_MIDDLE), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_ZOOM, std::bind(&CUser::Zoom, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_MOUSE, App::VM_MOUSE_WHEEL), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_GIZMO_SELECT, std::bind(&CUser::GizmoSelect, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_Q), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_GIZMO_TRANSLATE, std::bind(&CUser::GizmoTranslate, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_W), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_GIZMO_ROTATE, std::bind(&CUser::GizmoRotate, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_E), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_GIZMO_SCALE, std::bind(&CUser::GizmoScale, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_R), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_MODE_ERASE, std::bind(&CUser::ModeErase, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_D), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_MODE_FILL, std::bind(&CUser::ModeFill, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_F), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_MODE_PAINT, std::bind(&CUser::ModePaint, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_G), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_BRUSH_1, std::bind(&CUser::Brush1, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_1), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_BRUSH_2, std::bind(&CUser::Brush2, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_2), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_BRUSH_4, std::bind(&CUser::Brush4, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_3), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		// Project Actions.
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_NEW_PROJECT, std::bind(&CUser::NewProject, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_N, App::INPUT_MODIFIER_FLAG_CTRL), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_OPEN_PROJECT, std::bind(&CUser::OpenProject, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_O, App::INPUT_MODIFIER_FLAG_CTRL), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_SAVE_PROJECT, std::bind(&CUser::SaveProject, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_S, App::INPUT_MODIFIER_FLAG_CTRL), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_SAVE_PROJECT_AS, std::bind(&CUser::SaveProjectAs, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_S, App::INPUT_MODIFIER_FLAG_CTRL_ALT), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_BUILD_PROJECT, std::bind(&CUser::BuildProject, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_B, App::INPUT_MODIFIER_FLAG_CTRL), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		// Edit Actions.
		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_UNDO, std::bind(&CUser::Undo, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_Z, App::INPUT_MODIFIER_FLAG_CTRL), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		App::CInput::Instance().RegisterBinding(&App::INPUT_HASH_LAYOUT_EDITOR, 1,
			App::InputBindingSet(App::INPUT_KEY_REDO, std::bind(&CUser::Redo, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_Y, App::INPUT_MODIFIER_FLAG_CTRL), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		// Global Actions.
		const u32 allLayouts[] = { App::INPUT_HASH_LAYOUT_EDITOR, App::INPUT_HASH_LAYOUT_PLAYER };
		
		App::CInput::Instance().RegisterBinding(allLayouts, 2,
			App::InputBindingSet(App::INPUT_KEY_PLAY_SPAWN, std::bind(&CUser::PlaySpawn, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_P, App::INPUT_MODIFIER_FLAG_CTRL), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(allLayouts, 2,
			App::InputBindingSet(App::INPUT_KEY_PLAY_AT, std::bind(&CUser::PlayAt, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_P), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(allLayouts, 2,
			App::InputBindingSet(App::INPUT_KEY_QUIT, std::bind(&CUser::Quit, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_ESCAPE), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);
		
		App::CInput::Instance().RegisterBinding(allLayouts, 2,
			App::InputBindingSet(App::INPUT_KEY_SCREENSHOT, std::bind(&CUser::Screenshot, this, std::placeholders::_1),
			App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_F5), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
		);

		m_pawn.RegisterMotor(&m_motorMonitor);
		
		{ // Setup monitor motor.
			CMotorMonitor::Data data { };
			data.speed = 3.5f;
			data.lookRate = 0.1f;
			data.panRate = 0.5f;
			data.zoomRate = 0.5f;
			m_motorMonitor.SetData(data);
			m_motorMonitor.Initialize();
		}
	}

	void CUser::PostInitialize()
	{
		CPlayer::PostInitialize();
	}

	void CUser::Release()
	{
		m_motorMonitor.Release();
		CPlayer::Release();
	}
	
	//------------------------------------------------------------------------------------------------
	// File methods.
	//------------------------------------------------------------------------------------------------
		
	void CUser::Save() const
	{
		std::ofstream file(App::CEditor::Instance().Project().GetProjectUserPath() + L"\\user.dat", std::ios::binary);

		assert(file.is_open());
		if(!file.is_open()) { return; }

		file.write(reinterpret_cast<const char*>(m_pawn.GetPosition().ToFloat()), sizeof(float) * 3);
		file.write(reinterpret_cast<const char*>(m_pawn.GetEulerAngles().v), sizeof(float) * 3);
		file.close();
	}

	void CUser::Load()
	{
		Math::Vector3 vec;
		std::ifstream file(App::CEditor::Instance().Project().GetProjectUserPath() + L"\\user.dat", std::ios::binary);

		assert(file.is_open());
		if(!file.is_open()) { return; }

		file.read(reinterpret_cast<char*>(vec.v), sizeof(float) * 3);
		m_pawn.SetPosition(vec);
		file.read(reinterpret_cast<char*>(vec.v), sizeof(float) * 3);
		m_pawn.SetEulerAngles(vec);
		file.close();
	}
	
	//-----------------------------------------------------------------------------------------------
	// Input callbacks.
	//-----------------------------------------------------------------------------------------------
	
	// Movement.
	void CUser::MoveUp(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_MOVE_UP, callback);
	}

	void CUser::MoveDown(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_MOVE_DOWN, callback);
	}

	// Actions.
	void CUser::Select(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_SELECT, callback);
	}
	
	void CUser::Look(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_EDITOR_LOOK, callback);
	}

	void CUser::Pan(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_EDITOR_PAN, callback);
	}

	void CUser::Zoom(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_EDITOR_ZOOM, callback);
	}
	
	void CUser::GizmoSelect(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CGizmoPivot::CMD_KEY_GIZMO_SELECT);
		}
	}
	void CUser::GizmoTranslate(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CGizmoPivot::CMD_KEY_GIZMO_TRANSLATE);
		}
	}
	void CUser::GizmoRotate(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CGizmoPivot::CMD_KEY_GIZMO_ROTATE);
		}
	}
	void CUser::GizmoScale(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CGizmoPivot::CMD_KEY_GIZMO_SCALE);
		}
	}

	void CUser::ModeErase(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_EDITOR_MODE_ERASE, callback);
	}

	void CUser::ModeFill(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_EDITOR_MODE_FILL, callback);
	}

	void CUser::ModePaint(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_EDITOR_MODE_PAINT, callback);
	}
	
	void CUser::Brush1(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_EDITOR_BRUSH_1, callback);
	}
	
	void CUser::Brush2(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_EDITOR_BRUSH_2, callback);
	}
	
	void CUser::Brush4(const App::InputCallbackData& callback)
	{
		m_pawn.ProcessInput(MOTOR_INPUT_EDITOR_BRUSH_4, callback);
	}

	// File Actions.
	void CUser::NewProject(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			//App::CCommandManager::Instance().Execute(Universe::CChunkManager::CMD_KEY_CHUNK_RESET);
			App::CCommandManager::Instance().Execute(App::CProjectManager::CMD_KEY_NEW_PROJECT);
		}
	}

	void CUser::OpenProject(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CProjectManager::CMD_KEY_OPEN_PROJECT);
		}
	}

	void CUser::SaveProject(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CProjectManager::CMD_KEY_SAVE_PROJECT);
		}
	}

	void CUser::SaveProjectAs(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CProjectManager::CMD_KEY_SAVE_PROJECT_AS);
		}
	}
	
	void CUser::BuildProject(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CProjectManager::CMD_KEY_BUILD_PROJECT);
		}
	}

	// Edit Action.
	void CUser::Undo(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CCommandManager::CMD_KEY_UNDO);
		}
	}

	void CUser::Redo(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CCommandManager::CMD_KEY_REDO);
		}
	}

	// Scene Action.
	void CUser::PlaySpawn(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CAppState::CMD_KEY_TOGGLE_PLAY_SPAWN);
		}
	}

	void CUser::PlayAt(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CAppState::CMD_KEY_TOGGLE_PLAY_AT);
		}
	}

	void CUser::Quit(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			App::CCommandManager::Instance().Execute(App::CAppState::CMD_KEY_EDIT);
			//CFactory::Instance().GetPlatform()->Quit();
		}
	}

	void CUser::Screenshot(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			const std::time_t result = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			

			static char buffer[32];
			ctime_s(buffer, 32, &result);

			std::wstringstream wss;
			wss << L"./Screenshots/";
			for(size_t i = 0; buffer[i] != '\n'; ++i)
			{
				if(buffer[i] == ' ' || buffer[i] == ':') wss << L'-';
				else wss << wchar_t(buffer[i]);
			}

			wss << L".png";

			CFactory::Instance().GetGraphicsAPI()->Screenshot(wss.str().c_str());
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CUser::OnPlay(bool bSpawn)
	{
		if(bSpawn)
		{
			CSpawner::Get().Spawn(&m_pawn);
		}

		m_hud.LoadUI(CUIPlayer::UI_HASH);
		m_pawn.LoadMotor(CMotorHumanoid::MOTOR_HASH);
		m_pawn.RegisterPhysics();
	}

	void CUser::OnEdit()
	{
		m_hud.LoadUI(CUIPlayer::UI_HASH);
		m_pawn.DeregisterPhysics();
		m_pawn.LoadMotor(CMotorMonitor::MOTOR_HASH);
	}
};

