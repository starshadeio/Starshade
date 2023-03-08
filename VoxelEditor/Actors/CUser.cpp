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

