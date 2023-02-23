//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CSceneManager.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSceneManager.h"
#include "CScene.h"
#include "CView.h"
#include "CPanel.h"
#include "../Utilities/CJobSystem.h"
#include "../Utilities/CDebug.h"
#include "../Resources/CResourceManager.h"
#include "../Factory/CFactory.h"
#include "../Graphics/CGraphicsAPI.h"
#include "../UI/CUISelect.h"
#include <Application/CCoreManager.h>
#include <Physics/CPhysics.h>

namespace App
{
	CSceneManager::CSceneManager() :
		m_postProcessor(L""),
		m_pTargetScene(nullptr),
		m_pGlobalScene(nullptr),
		m_pDefaultScene(nullptr),
		m_pScene(nullptr) {
	}

	CSceneManager::~CSceneManager() { }

	void CSceneManager::Initialize()
	{
		Util::CJobSystem::Instance().Initialize();
		
		m_audioMixer.Intialize();
		Resources::CManager::Instance().Initialize();
		Physics::CPhysics::Instance().Initialize();
		m_universeManager.Initialize();
		
		Util::CDebug::Instance().Initialize();
		
		// Initialize view(s).
		for(auto viewPair : m_viewMap)
		{
			viewPair.second->Initialize();
		}

		// Initialize scene(s).
		m_pGlobalScene->Initialize();
		for(auto& scenePair : m_sceneMap)
		{
			scenePair.second->Initialize();
		}

		m_nodeManager.Initialize();
		App::CCoreManager::Instance().Initialize();

		m_postProcessor.Initialize();

		// Load scenes.
		m_pGlobalScene->Load();

		m_pTargetScene = m_pScene = m_pDefaultScene;
		if(m_pScene)
		{
			m_pScene->Load();
		}

		// Execute any remain jobs that need to be completed prior to initialization ending.
		Util::CJobSystem::Instance().Execute();
	}

	void CSceneManager::PostInitialize()
	{
		m_audioMixer.PostInitialize();

		Resources::CManager::Instance().PostInitialize();
		m_postProcessor.PostInitialize();
		m_renderingSystem.PostInitialize();
		m_computeSystem.PostInitialize();
		m_uiEventSystem.PostInitialize();

		// Post initialize view(s).
		for(auto& viewPair : m_viewMap)
		{
			viewPair.second->PostInitialize();
		}

		// Post initialize scene(s).
		m_pGlobalScene->PostInitialize();
		for(auto& scenePair : m_sceneMap)
		{
			scenePair.second->PostInitialize();
		}
		
		m_nodeManager.PostInitialize();
		App::CCoreManager::Instance().PostInitialize();

		Util::CJobSystem::Instance().Open();
	}

	void CSceneManager::Update()
	{
		// Update.
		m_pGlobalScene->Update();
		if(m_pScene)
		{
			m_pScene->Update();
		}
		
		for(auto view : m_viewLoadedList)
		{
			view->Update();
		}
		
		m_nodeManager.Update();

		// Internal update.
		App::CCoreManager::Instance().Update();
		m_uiEventSystem.Update();

		m_audioMixer.Update();
		m_universeManager.Update();

		// Late update.
		m_pGlobalScene->LateUpdate();
		if(m_pScene)
		{
			m_pScene->LateUpdate();
		}

		for(auto view : m_viewLoadedList)
		{
			view->LateUpdate();
		}
		
		App::CCoreManager::Instance().LateUpdate();
		m_nodeManager.LateUpdate();

		// Internal late update.
		Util::CJobSystem::Instance().Sync();
	}

	void CSceneManager::Compute()
	{
		m_computeSystem.Dispatch();
	}

	void CSceneManager::Render()
	{
		m_renderingSystem.QueueRender(&m_postProcessor, 0);

		m_renderingSystem.RenderAll();
		m_postProcessor.RenderQuad();
	}

	void CSceneManager::PostRender()
	{
		m_garbage.Process();

		bool bRequireSync = false;

		// Check if a new scene should be loaded.
		if(m_pScene != m_pTargetScene) {
			Util::CJobSystem::Instance().JobGraphics([this](){
				if(m_pScene) { m_pScene->Unload(); }
				if(m_pTargetScene)
				{
						m_pTargetScene->Load();
				}
			}, false);

			m_pScene = m_pTargetScene;
			bRequireSync = true;
		}

		if(!m_viewUnloadQueue.empty() || !m_viewLoadQueue.empty())
		{
			Util::CJobSystem::Instance().JobGraphics([this](){
				// Unload queued views.
				while(!m_viewUnloadQueue.empty())
				{
					size_t i = 0;
					for(size_t j = 0; j < m_viewLoadedList.size(); j++)
					{
						if(m_viewLoadedList[j] != m_viewUnloadQueue.front())
						{
							m_viewLoadedList[i++] = m_viewUnloadQueue.front();
						}
					}

					m_viewLoadedList.resize(i);
					m_viewUnloadQueue.front()->Unload();
					m_viewUnloadQueue.pop();
				}

				// Load queued views.
				while(!m_viewLoadQueue.empty())
				{
					m_viewLoadedList.push_back(m_viewLoadQueue.front());
					m_viewLoadQueue.front()->Load();
					m_viewLoadQueue.pop();
				}
			}, false);

			bRequireSync = true;
		}

		if(bRequireSync)
		{
			CFactory::Instance().GetGraphicsAPI()->Sync();
		}

		Util::CJobSystem::Instance().Process();
	}

	bool CSceneManager::OnQuit(int exitCode)
	{
		for(auto view : m_viewLoadedList)
		{
			if(!view->OnQuit(exitCode))
			{
				return false;
			}
		}

		if(m_pScene)
		{
			if(!m_pScene->OnQuit(exitCode))
			{
				return false;
			}
		}
		
		if(!m_pGlobalScene->OnQuit(exitCode))
		{
			return false;
		}

		return true;
	}

	void CSceneManager::Release()
	{
		if(m_pScene)
		{
			m_pScene->Unload();
		}

		m_pGlobalScene->Unload();
		
		Physics::CPhysics::Instance().Halt();
		Util::CJobSystem::Instance().Close();

		App::CCoreManager::Instance().Release();
		m_nodeManager.Release();

		for(auto& viewPair : m_viewMap)
		{
			viewPair.second->Release();
		}

		for(auto& scenePair : m_sceneMap)
		{
			scenePair.second->Release();
		}

		m_pGlobalScene->Release();
		m_pGlobalScene = m_pDefaultScene = nullptr;

		m_postProcessor.Release();
		Util::CDebug::Instance().Release();
		m_universeManager.Release();
		
		m_garbage.Release();
		Util::CJobSystem::Instance().Release();

		m_audioMixer.Release();
		Physics::CPhysics::Instance().Release();
		Resources::CManager::Instance().Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Panel adjustment methods.
	//-----------------------------------------------------------------------------------------------

	void CSceneManager::OnMove(const Math::Rect& rect)
	{
		if(m_pGlobalScene)
		{
			m_pGlobalScene->OnMove(rect);
		}

		if(m_pScene)
		{
			m_pScene->OnMove(rect);
		}
	}

	void CSceneManager::OnResize(const Math::Rect& rect)
	{
		Util::CJobSystem::Instance().JobGraphics([=](){
			m_postProcessor.OnResize(rect);

			if(m_pGlobalScene)
			{
				m_pGlobalScene->OnResize(rect);
			}

			if(m_pScene)
			{
				m_pScene->OnResize(rect);
			}

		}, false);

		
		Util::CJobSystem::Instance().Process();
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CSceneManager::RegisterScene(CScene* pScene, u32 hash, SceneType sceneType)
	{
		if(sceneType == SceneType::Global)
		{
			m_pGlobalScene = pScene;
		}
		else
		{
			m_sceneMap.insert({ hash, pScene });
			if(sceneType == SceneType::Default)
			{
				m_pDefaultScene = pScene;
			}
		}
	}

	bool CSceneManager::LoadScene(u32 hash)
	{
		auto elem = m_sceneMap.find(hash);
		if(elem != m_sceneMap.end())
		{
			m_pTargetScene = elem->second;
			return true;
		}

		return false;
	}
	
	void CSceneManager::RegisterView(CView* pView, u32 hash)
	{
		m_viewMap.insert({ hash, pView });
	}

	bool CSceneManager::LoadView(u32 hash)
	{
		auto elem = m_viewMap.find(hash);
		assert(elem != m_viewMap.end());
		if(elem->second->IsLoaded()) { return false; }
		m_viewLoadQueue.push(elem->second);
		return true;
	}

	bool CSceneManager::UnloadView(u32 hash)
	{
		auto elem = m_viewMap.find(hash);
		assert(elem != m_viewMap.end());
		if(elem->second->IsLoaded()) { return false; }
		m_viewUnloadQueue.push(elem->second);
		return true;
	}
};
