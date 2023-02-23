//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRenderingSystem.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CRenderingSystem.h"
#include "CRenderer_.h"
#include "CRenderer.h"
#include "CMaterial.h"
#include "CShader.h"
#include "CRootSignature.h"
#include "CPostProcessor.h"
#include "../UI/CUICanvas.h"
#include "../Application/CSceneManager.h"
#include "../Utilities/CDebug.h"
#include <Objects/CVObject.h>

namespace Graphics
{
	CRenderingSystem::CRenderingSystem() { }
	CRenderingSystem::~CRenderingSystem() { }

	void CRenderingSystem::PostInitialize()
	{
		for(auto elem : m_rendererList_)
		{
			for(CRenderer_* pRenderer : elem.second)
			{
				pRenderer->PostInitialize();
			};
		}
	}

	void CRenderingSystem::QueueRender(CPostProcessor* pPostProcessor, u32 viewHash)
	{
		m_renderQueue.push({ pPostProcessor, viewHash });
	}

	//-----------------------------------------------------------------------------------------------
	// Internal rendering methods.
	//-----------------------------------------------------------------------------------------------

	void CRenderingSystem::RenderAll()
	{
		while(!m_renderQueue.empty())
		{
			const RenderData& data = m_renderQueue.front();

			data.pPostProcessor->BeginRenderTo(Graphics::CLEAR_ALL, 0, nullptr);
			RenderWorld(data.viewHash);
			RenderUI(data.viewHash);

			if(data.viewHash == 0)
			{
				Util::CDebug::Instance().Drawer().Render();
			}

			auto overlayList = m_overlayList.find(data.viewHash);
			if(overlayList != m_overlayList.end())
			{
				for(auto& func : overlayList->second)
				{
					func();
				}
			}

			data.pPostProcessor->EndRenderTo();
			
			m_renderQueue.pop();
		}
	}

	void CRenderingSystem::RenderWorld(u32 viewHash)
	{
		CRootSignature* rootSig = nullptr;

		for(CRenderer_* pRenderer : m_rendererList_[viewHash])
		{
			if(!pRenderer->IsActive()) { continue; }

			if(rootSig != pRenderer->GetMaterial()->GetShader()->GetRootSignature())
			{
				rootSig = pRenderer->GetMaterial()->GetShader()->GetRootSignature();
				rootSig->Bind();
			}

			pRenderer->Render();
		};

		for(CRenderer* pRenderer : m_rendererList)
		{
			pRenderer->Render(viewHash);
		}
		
		const Math::SIMDVector camPos = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition();
		SortBackToFront(camPos, m_depthlessList[viewHash]);

		for(CRenderer_* pRenderer : m_depthlessList[viewHash])
		{
			if(!pRenderer->IsActive()) { continue; }

			if(rootSig != pRenderer->GetMaterial()->GetShader()->GetRootSignature())
			{
				rootSig = pRenderer->GetMaterial()->GetShader()->GetRootSignature();
				rootSig->Bind();
			}

			pRenderer->Render();
		};
	}

	void CRenderingSystem::RenderUI(u32 viewHash)
	{
		// UI.
		for(UI::CUICanvas* pCanvas : m_canvasList[viewHash])
		{
			if(!pCanvas->IsActive()) { continue; }
			pCanvas->Render();
		};
	}

	//-----------------------------------------------------------------------------------------------
	// (De)registration methods.
	//-----------------------------------------------------------------------------------------------

	// NOTE: As a more advanced culling system is put in place, this system will need to be adjusted for it.
	
	// pRenderer (CNodeComponent).
	void CRenderingSystem::Register(CRenderer* pRenderer)
	{
		Register<CRenderer>(pRenderer, m_rendererList, [](CRenderer* a, CRenderer* b){
			return a->GetDepth() > b->GetDepth();
		});
	}

	void CRenderingSystem::Deregister(CRenderer* pRenderer)
	{
		Deregister<CRenderer>(pRenderer, m_rendererList);
	}

	// CRenderer_.
	void CRenderingSystem::Register(CRenderer_* pRenderer)
	{
		if(!pRenderer->GetMaterial()->GetShader()->GetDepthWrite())
		{
			Register<CRenderer_>(pRenderer, m_depthlessList[pRenderer->GetObject()->GetViewHash()], nullptr);
		}
		else
		{
			Register<CRenderer_>(pRenderer, m_rendererList_[pRenderer->GetObject()->GetViewHash()], [](CRenderer_* a, CRenderer_* b){
				return reinterpret_cast<uintptr_t>(a->GetMaterial()->GetShader()->GetRootSignature()) >
					reinterpret_cast<uintptr_t>(b->GetMaterial()->GetShader()->GetRootSignature());
			});
		}
	}

	void CRenderingSystem::Deregister(CRenderer_* pRenderer)
	{
		if(!pRenderer->GetMaterial()->GetShader()->GetDepthWrite())
		{
			Deregister<CRenderer_>(pRenderer, m_depthlessList[pRenderer->GetObject()->GetViewHash()]);
		}
		else
		{
			Deregister<CRenderer_>(pRenderer, m_rendererList_[pRenderer->GetObject()->GetViewHash()]);
		}
	}
	
	// CUICanvas.
	void CRenderingSystem::Register(UI::CUICanvas* pCanvas)
	{
		Register<UI::CUICanvas>(pCanvas, m_canvasList[pCanvas->GetObject()->GetViewHash()], [](UI::CUICanvas* a, UI::CUICanvas* b){
			return a->GetDepth() > b->GetDepth();
		});
	}

	void CRenderingSystem::Deregister(UI::CUICanvas* pCanvas)
	{
		Deregister<UI::CUICanvas>(pCanvas, m_canvasList[pCanvas->GetObject()->GetViewHash()]);
	}
};
