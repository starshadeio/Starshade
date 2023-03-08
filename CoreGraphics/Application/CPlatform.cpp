//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CPlatform.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPlatform.h"
#include "CPanel.h"
#include "CSceneManager.h"
#include "../Resources/CResourceManager.h"
#include "../Factory/CFactory.h"
#include "../Graphics/CGraphicsAPI.h"
#include "../Application/CInput.h"
#include <Utilities/CFileSystem.h>
#include <Application/CAppBase.h>
#include <Utilities/CMemoryFree.h>
#include <Utilities/CDebugError.h>
#include <Utilities/CFileUtil.h>

namespace App
{
	CPlatform::CPlatform() :
		m_pGraphicsAPI(nullptr) {
	}

	CPlatform::~CPlatform() { }

	// NOTE: This method will eventually be used to compile a UI script, but for now everything will be hard-coded.
	void CPlatform::Initialize(const Data& baseData)
	{
		m_mData = baseData;

		{ // Create the UI.
			CWUIScript::Data data { };
			data.filename = m_mData.wuiFilename;
			data.platformOnMove = std::bind(&CPlatform::OnMove, this, std::placeholders::_1);
			data.platformOnResize = std::bind(&CPlatform::OnResize, this, std::placeholders::_1);
			m_wuiScript.SetData(data);
			m_wuiScript.Parse();

			// Input setup.
			CInput::Instance().Initialize(m_wuiScript.GetMainPanel());
		}

		{ // Setup and initialize the graphics API.
			Graphics::CGraphicsAPI::Data data { };
			data.bufferCount = 2;
			data.vBlanks = 1;
			data.clearValue.color = Math::Color(0.05f, 0.05f, 0.05f, 1.0f);
			data.clearValue.depthStencil.depth = 1.0f;
			data.clearValue.depthStencil.stencil = 0;
			data.clearValue.format = Graphics::GFX_FORMAT_R8G8B8A8_UNORM;
			data.onInit = std::bind(&CPlatform::LoadAssets, this);
			data.onCompute = std::bind(&CPlatform::Compute, this);
			data.onRender = std::bind(&CPlatform::Render, this);
			data.pPanel = m_wuiScript.GetMainPanel();

			m_pGraphicsAPI = CFactory::Instance().GetGraphicsAPI();
			m_pGraphicsAPI->SetData(data);
			m_pGraphicsAPI->Initialize();
		}

		CSceneManager::Instance().PostInitialize();
	}

	void CPlatform::LoadAssets()
	{
		m_pApp->Create();
		CSceneManager::Instance().Initialize();
	}

	// Method called in the update loop of the target platform's Run(...) method.
	void CPlatform::Frame(bool bInFocus)
	{
		if(bInFocus)
		{
			CInput::Instance().Update();
		}

		m_pApp->Run();
		CSceneManager::Instance().Update();
		CInput::Instance().PostUpdate();

		m_pGraphicsAPI->Render();

		PostRender();
	}

	void CPlatform::Compute()
	{
		CSceneManager::Instance().Compute();
	}

	void CPlatform::Render()
	{
		CSceneManager::Instance().Render();
	}
	
	void CPlatform::PostRender()
	{
		CSceneManager::Instance().PostRender();
	}

	void CPlatform::Release()
	{
		CSceneManager::Instance().Release();
		m_pApp->Destroy();
		CInput::Instance().Release();
		m_wuiScript.Release();
	}

	bool CPlatform::OnQuit(int exitCode)
	{
		return CSceneManager::Instance().OnQuit(exitCode);
	}

	//-----------------------------------------------------------------------------------------------
	// Production script methods.
	//-----------------------------------------------------------------------------------------------
	
	void CPlatform::SaveProductionScripts()
	{
		std::wstring path;
		std::wstring filename;
		std::wstring extension;
		Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(m_mData.wuiFilename.c_str(), path, filename, extension);

		path = Resources::CManager::Instance().GetProductionPath() + path;//.substr(wcslen(Resources::CManager::Instance().GetFilePath()));
		ASSERT_R(Util::CFileSystem::Instance().NewPath((path).c_str()));

		Util::WriteFileUTF8((path + filename + extension).c_str(), m_wuiScript.GetScriptObject().GetCode());

		m_wuiScript.SaveProductionResources();
	}

	//-----------------------------------------------------------------------------------------------
	// Panel adjustment methods.
	//-----------------------------------------------------------------------------------------------

	void CPlatform::OnMove(const CPanel* pPanel)
	{
		CSceneManager::Instance().OnMove(pPanel->GetRect());
	}

	void CPlatform::OnResize(const CPanel* pPanel)
	{
		m_pGraphicsAPI->AdjustToResize();
		CSceneManager::Instance().OnResize(pPanel->GetRect());
		m_pGraphicsAPI->FinalizeResize();
		
		m_pApp->Run();
		CSceneManager::Instance().Update();
		CInput::Instance().PostUpdate();

		m_pGraphicsAPI->Render();

		PostRender();
	}
};
