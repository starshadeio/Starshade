//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CApplication.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CApplication.h"
#include "CAppData.h"
#include "CAppReg.h"
#include "../Resources/CAssetManager.h"
#include <Application/CInput.h>
#include <Application/CSceneManager.h>
#include <Application/CAppVersion.h>
#include <Resources/CResourceManager.h>
#include <Physics/CPhysics.h>
#include <Utilities/CTimer.h>
#include <Utilities/CFileSystem.h>

#include <Math/CSIMDMatrix.h>

namespace App
{
	CApplication::CApplication()
	{
		App::CVersion::Instance().SetVersion(0, 3, 0, 0, L"alpha");
	}

	CApplication::~CApplication() { }

	void CApplication::Create()
	{
		App::CAppReg reg;

		CAppBase::Create();

		{ // Setup the resource manager.
			Resources::CManager::Data data { };
			data.cfgPath = L"Config/";
			data.resPath = L"Resources/";
#ifndef PRODUCTION_BUILD
			data.prodPath = L"Production/";
#else
			data.prodPath = L"";
#endif
			data.filename = L"Editor.res";
			Resources::CManager::Instance().SetData(data);
		}

		{ // Setup audio system.
			Audio::CAudioMixer::Data data { };
			CSceneManager::Instance().AudioMixer().SetData(data);
		}

		{ // Setup the physics system.
			Physics::CPhysics::Data data { };
			data.targetFPS = 60.0f;
			data.idleIterations = 4;
			data.rayCastIterations = 6;
			data.gravity = Math::SIMD_VEC_DOWN * 20.0f;
			Physics::CPhysics::Instance().SetData(data);
		}

		{ // Setup input.
			CInput::Instance().CreateLayout(INPUT_HASH_LAYOUT_EDITOR, true);
			CInput::Instance().CreateLayout(INPUT_HASH_LAYOUT_PLAYER, false);
		}
		
		{ // Setup chunk manager.
			Universe::CChunkManager::Data data { };
			data.viewHash = VIEW_HASH_MAIN;
			CSceneManager::Instance().UniverseManager().ChunkManager().SetData(data);
		}
	}
};
