//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CSceneManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSCENEMANAGER_H
#define CSCENEMANAGER_H

#include "CScene.h"
#include "CView.h"
#include "CNodeManager.h"
#include "CSceneFileIO.h"

#include "../Graphics/CRenderingSystem.h"
#include "../Graphics/CComputeSystem.h"
#include "../Graphics/CPostProcessor.h"
#include "../UI/CUIEventSystem.h"
#include "../Audio/CAudioMixer.h"
#include "../Universe/CUniverseManager.h"
#include "../Utilities/CGarbage.h"

#include <Objects/CCompBatch.h>
#include <Globals/CGlobals.h>
#include <Logic/CTransform.h>
#include <Logic/CCameraManager.h>
#include <unordered_map>
#include <queue>
#include <vector>

namespace App
{
	class CSceneManager
	{
	private:
		CSceneManager();
		~CSceneManager();
		CSceneManager(const CSceneManager&) = delete;
		CSceneManager(CSceneManager&&) = delete;
		CSceneManager& operator = (const CSceneManager&) = delete;
		CSceneManager& operator = (CSceneManager&&) = delete;

	public:
		static CSceneManager& Instance()
		{
			static CSceneManager instance;
			return instance;
		}

		void Initialize();
		void PostInitialize();
		void Update();
		void Compute();
		void Render();
		void PostRender();
		bool OnQuit(int exitCode);
		void Release();

		void OnMove(const Math::Rect& rect);
		void OnResize(const Math::Rect& rect);

		void RegisterScene(CScene* pScene, u32 hash, SceneType sceneType);
		bool LoadScene(u32 hash);

		void RegisterView(CView* pView, u32 hash);
		bool LoadView(u32 hash);
		bool UnloadView(u32 hash);

		// Accessors.
		inline u32 GetLoadSceneHash() const { return m_pScene ? m_pScene->GetHash() : 0; }

		inline const CView* GetView(u32 hash) const { return m_viewMap.find(hash)->second; }

		inline CNodeManager& NodeManager() { return m_nodeManager; }
		inline CSceneFileIO& FileIO() { return m_fileIO; }
		inline Logic::CCameraManager& CameraManager() { return m_cameraManager; }
		inline Graphics::CRenderingSystem& RenderingSystem() { return m_renderingSystem; }
		inline Graphics::CComputeSystem& ComputeSystem() { return m_computeSystem; }
		inline Graphics::CPostProcessor& PostProcessor() { return m_postProcessor; }
		inline UI::CUIEventSystem& UIEventSystem() { return m_uiEventSystem; }
		inline Audio::CAudioMixer& AudioMixer() { return m_audioMixer; }
		inline Universe::CUniverseManager& UniverseManager() { return m_universeManager; }
		inline Util::CGarbage& Garbage() { return m_garbage; }

	private:
		std::unordered_map<u32, CScene*> m_sceneMap;
		std::unordered_map<u32, CView*> m_viewMap;
		std::queue<CView*> m_viewLoadQueue;
		std::queue<CView*> m_viewUnloadQueue;
		std::vector<CView*> m_viewLoadedList;

		CNodeManager m_nodeManager;
		Logic::CCameraManager m_cameraManager;
		Graphics::CRenderingSystem m_renderingSystem;
		Graphics::CComputeSystem m_computeSystem;
		Graphics::CPostProcessor m_postProcessor;
		UI::CUIEventSystem m_uiEventSystem;
		Universe::CUniverseManager m_universeManager;
		Audio::CAudioMixer m_audioMixer;
		Util::CGarbage m_garbage;
		CSceneFileIO m_fileIO;

		CScene* m_pTargetScene;

		CScene* m_pGlobalScene;
		CScene* m_pDefaultScene;
		CScene* m_pScene;
	};
};

#endif
