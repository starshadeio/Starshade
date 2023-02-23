//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CSceneGlobal.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSceneGlobal.h"
#include "CAppData.h"
#include "CEditor.h"
#include "../Resources/CAssetManager.h"
#include <Application/CLocalization.h>
#include <Application/CSceneManager.h>
#include <Graphics/CGraphicsAPI.h>
#include <Application/CAppVersion.h>
#include <Application/CPlatform.h>
#include <Application/CPanel.h>
#include <Resources/CResourceManager.h>
#include <Factory/CFactory.h>
#include <Utilities/CTimer.h>
#include <Math/CMathFNV.h>

namespace App
{
	CSceneGlobal::CSceneGlobal() :
		CScene(SCENE_NAME_GLOBAL, SceneType::Global) {
	}

	CSceneGlobal::~CSceneGlobal() { }

	void CSceneGlobal::Initialize()
	{
		{ // Setup localization.
			CLocalization::Instance().Load(Math::FNV1a_64(L"LOC_EN"));
		}

		{ // Setup scene manager post processor.
			Graphics::CRenderTexture::Data data { };

			memset(&data, 0, sizeof(data));
			data.colorBufferCount = 1;
			data.bUseDepthStencil = true;
			data.width = static_cast<u32>(CFactory::Instance().GetGraphicsAPI()->GetWidth());
			data.height = static_cast<u32>(CFactory::Instance().GetGraphicsAPI()->GetHeight());
			data.downScale = 0;
			data.colorFormat[0] = Graphics::GFX_FORMAT_R32G32B32A32_FLOAT;
			data.depthFormat = Graphics::GFX_FORMAT_D32_FLOAT;

			data.clearColor[0] = CFactory::Instance().GetGraphicsAPI()->GetData().clearValue.color;
			data.clearColor[0].a = 0.0f;

			data.clearDepthStencil.depth = 1.0f;
			data.clearDepthStencil.stencil = 0;

			data.colorTexKey[0] = L"VIEW_NULL_COLOR_0";
			data.depthTexKey = L"VIEW_NULL_DEPTH";

			CSceneManager::Instance().PostProcessor().SetData(data);
		}

		Resources::CAssets::Instance().Initialize();
		CEditor::Instance().Initialize();
	}

	void CSceneGlobal::PostInitialize()
	{
		Resources::CAssets::Instance().PostInitialize();
		CEditor::Instance().PostInitialize();

		// This is where production grade resources can be generated.
		//Resources::CManager::Instance().BuildProductionResources();
	}

	void CSceneGlobal::Update()
	{
		CEditor::Instance().Update();
	}

	void CSceneGlobal::LateUpdate() 
	{
		CEditor::Instance().LateUpdate();
	}

	bool CSceneGlobal::OnQuit(int exitCode)
	{
		return CEditor::Instance().OnQuit(exitCode);
	}

	void CSceneGlobal::Release()
	{
		CEditor::Instance().Release();
		Resources::CAssets::Instance().Release();
	}

	void CSceneGlobal::OnResize(const Math::Rect& rect)
	{
		CEditor::Instance().OnResize(rect);
	}
};
