//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel App
//
// File: Application/CSceneGlobal.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSceneGlobal.h"
#include "CAppData.h"

#include <Actors/CSpawner.h>
#include <Application/CLocalization.h>
#include <Application/CSceneManager.h>
#include <Application/CCommandManager.h>
#include <Application/CAppVersion.h>
#include <Application/CPlatform.h>
#include <Application/CPanel.h>
#include <Application/CInput.h>
#include <Application/CInputDeviceList.h>
#include <Application/CInputKeyboard.h>
#include <Application/CInputMouse.h>
#include <Application/CInputGamepad.h>
#include <Graphics/CGraphicsAPI.h>
#include <Resources/CResourceManager.h>
#include <Factory/CFactory.h>
#include <Utilities/CTimer.h>
#include <Math/CMathFNV.h>

namespace App
{
	CSceneGlobal::CSceneGlobal() :
		CScene(SCENE_NAME_GLOBAL, SceneType::Global),
		m_player(L"Player", 0) {
	}

	CSceneGlobal::~CSceneGlobal() { }

	void CSceneGlobal::Initialize()
	{
		{ // Register bindings.
			App::CInput::Instance().RegisterBinding(&INPUT_HASH_LAYOUT_PLAYER, 1,
				App::InputBindingSet(App::INPUT_KEY_QUIT, std::bind(&CSceneGlobal::Quit, this, std::placeholders::_1),
				App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_ESCAPE), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
			);
			
			App::CInput::Instance().RegisterBinding(&INPUT_HASH_LAYOUT_PLAYER, 1,
				App::InputBindingSet(App::INPUT_KEY_SCREENSHOT, std::bind(&CSceneGlobal::Screenshot, this, std::placeholders::_1),
				App::InputBinding(App::INPUT_DEVICE_KEYBOARD, App::VK_KB_F5), App::InputBinding(App::INPUT_DEVICE_NONE, 0))
			);
		}
		
		{ // Setup scene manager post processor.
			Graphics::CRenderTexture::Data data { };
			
			memset(&data, 0, sizeof(data));
			data.colorBufferCount = 4;
			data.bUseDepthStencil = true;
			data.width = static_cast<u32>(CFactory::Instance().GetGraphicsAPI()->GetWidth());
			data.height = static_cast<u32>(CFactory::Instance().GetGraphicsAPI()->GetHeight());
			data.downScale = 0;
			data.colorFormat[0] = Graphics::GFX_FORMAT_R32G32B32A32_FLOAT;
			data.colorFormat[1] = Graphics::GFX_FORMAT_R32G32B32A32_FLOAT;
			data.colorFormat[2] = Graphics::GFX_FORMAT_R32G32B32A32_FLOAT;
			data.colorFormat[3] = Graphics::GFX_FORMAT_R32G32B32A32_FLOAT;
			data.depthFormat = Graphics::GFX_FORMAT_D32_FLOAT;

			data.clearColor[0] = CFactory::Instance().GetGraphicsAPI()->GetData().clearValue.color;
			data.clearColor[0].a = 0.0f;
			data.clearColor[1] = Math::Color(0.0f, 0.0f);
			data.clearColor[2] = Math::Color(0.0f, 0.0f);
			data.clearColor[3] = Math::Color(0.0f, 0.0f);

			data.clearDepthStencil.depth = 1.0f;
			data.clearDepthStencil.stencil = 0;

			data.colorTexKey[0] = L"VIEW_NULL_COLOR_0";
			data.colorTexKey[1] = L"VIEW_NULL_COLOR_1";
			data.colorTexKey[2] = L"VIEW_NULL_COLOR_2";
			data.colorTexKey[3] = L"VIEW_NULL_COLOR_3";
			data.depthTexKey = L"VIEW_NULL_DEPTH";

			CSceneManager::Instance().PostProcessor().SetData(data);
		}
		
		{ // Post effects.
			CSceneManager::Instance().PostProcessor().RegisterPostEffect(&m_postSky);
			CSceneManager::Instance().PostProcessor().RegisterPostEffect(&m_postEdge);
			CSceneManager::Instance().PostProcessor().RegisterPostEffect(&m_postSSAO);
			CSceneManager::Instance().PostProcessor().RegisterPostEffect(&m_postLighting);
			CSceneManager::Instance().PostProcessor().RegisterPostEffect(&m_postToneMapping);
			CSceneManager::Instance().PostProcessor().RegisterPostEffect(&m_postFXAA);

			m_postSky.Initialize();
			m_postSky.SetSkyInterpolation(1.0f);

			m_postEdge.Initialize();
			m_postSSAO.Initialize();
			m_postFXAA.Initialize();
			
			{
				Graphics::CPostLighting::Data data { };
				data.pPostSky = &m_postSky;
				data.pPostEdge = &m_postEdge;
				data.pPostSSAO = &m_postSSAO;
				m_postLighting.SetData(data);
				m_postLighting.Initialize();
			}

			m_postToneMapping.Initialize();
		}

		{ // Node loader.
			m_nodeLoader.Initialize();
		}

		{ // Player.
			Actor::CPlayer::Data data { };
			data.inputHash = INPUT_HASH_LAYOUT_PLAYER;
			data.pawnData.speed = 3.5f;
			data.pawnData.jumpPower = 7.5f;
			data.pawnData.startPosition = Math::SIMDVector(0.0f, 2.0f, -10.0f);
			data.pawnData.startEuler = Math::Vector3(0.0f);
			m_player.SetData(data);
			m_player.Initialize();
		}

		{ // Setup the environment.
			Universe::CEnvironment::Data data { };
			data.bActive = true;
			m_environment.SetData(data);
			m_environment.Initialize();
		}
		
		CSceneManager::Instance().UniverseManager().ChunkManager().SetTexture(
			reinterpret_cast<Graphics::CTexture*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_TEXTURE, Math::FNV1a_64(L"TEXTURE_ASSET_PALETTE_0"))));
	}

	void CSceneGlobal::PostInitialize()
	{
		m_postSky.PostInitialize();
		m_postEdge.PostInitialize();
		m_postSSAO.PostInitialize();
		m_postLighting.PostInitialize();
		m_postToneMapping.PostInitialize();
		m_postFXAA.PostInitialize();

		m_nodeLoader.PostInitialize();

		m_player.PostInitialize();

		Actor::CSpawner::Get().Spawn(&m_player.Pawn());
		
		m_player.Pawn().LoadMotor(Actor::CMotorHumanoid::MOTOR_HASH);
		m_player.Pawn().RegisterPhysics();

		CCommandManager::Instance().ProcessActionRegistrationQueue();
	}

	void CSceneGlobal::Update()
	{
		m_player.Update();
		m_environment.Update();
	}

	void CSceneGlobal::Release()
	{
		m_environment.Release();
		m_player.Release();

		m_postFXAA.Release();
		m_postToneMapping.Release();
		m_postLighting.Release();
		m_postSSAO.Release();
		m_postEdge.Release();
		m_postSky.Release();
	}

	void CSceneGlobal::OnResize(const Math::Rect& rect)
	{
		m_player.OnResize(rect);

		m_postSky.OnResize(rect);
		m_postEdge.OnResize(rect);
		m_postSSAO.OnResize(rect);
		m_postLighting.OnResize(rect);
		m_postToneMapping.OnResize(rect);
		m_postFXAA.OnResize(rect);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Input methods.
	//-----------------------------------------------------------------------------------------------

	void CSceneGlobal::Quit(const App::InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CFactory::Instance().GetPlatform()->Quit();
		}
	}

	void CSceneGlobal::Screenshot(const App::InputCallbackData& callback)
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
};
