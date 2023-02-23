//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CViewOverlay.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CViewOverlay.h"
#include "CViewScene.h"
#include "CAppData.h"
#include "CAppState.h"
#include <Application/CSceneManager.h>
#include <Application/CCommandManager.h>
#include <Graphics/CGraphicsAPI.h>
#include <Application/CAppVersion.h>
#include <Application/CPlatform.h>
#include <Application/CPanel.h>
#include <Resources/CResourceManager.h>
#include <Factory/CFactory.h>
#include <Utilities/CTimer.h>

namespace App
{
	CViewOverlay::CViewOverlay() : 
		CView(VIEW_NAME_OVERLAY)
	{
	}

	CViewOverlay::~CViewOverlay()
	{
	}

	void CViewOverlay::Initialize()
	{
		{ // Setup post processor.
			Graphics::CRenderTexture::Data data { };

			memset(&data, 0, sizeof(data));
			data.colorBufferCount = 1;
			data.bUseDepthStencil = true;
			data.width = static_cast<u32>(CFactory::Instance().GetGraphicsAPI()->GetWidth());
			data.height = static_cast<u32>(CFactory::Instance().GetGraphicsAPI()->GetHeight());
			data.downScale = 0;
			data.colorFormat[0] = Graphics::GFX_FORMAT_R32G32B32A32_FLOAT;
			data.depthFormat = Graphics::GFX_FORMAT_D32_FLOAT;

			data.clearColor[0] = Math::COLOR_BLACK;
			data.clearColor[0].a = 0.0f;

			data.clearDepthStencil.depth = 1.0f;
			data.clearDepthStencil.stencil = 0;

			data.colorTexKey[0] = L"VIEW_OVERLAY_COLOR_0";
			data.depthTexKey = L"VIEW_OVERLAY_DEPTH";

			SetPostProcessorData(data);
		}

		CView::Initialize();

		{ // Post effects.
			RegisterPostEffect(&m_postOverlay);

			{ // Setup post overlay.
				Graphics::CPostOverlay::Data data { };
				data.pViewScene = m_data.pViewScene;
				m_postOverlay.SetData(data);
			}

			m_postOverlay.Initialize();
		}
	}

	void CViewOverlay::PostInitialize()
	{
		CView::PostInitialize();
		
		m_postOverlay.PostInitialize();
	}

	bool CViewOverlay::Load()
	{
		if(!CView::Load()) return false;
		return true;
	}

	bool CViewOverlay::Unload()
	{
		if(!CView::Unload()) return false;
		return true;
	}

	void CViewOverlay::LateUpdate()
	{
		QueueRender();
	}

	void CViewOverlay::Release()
	{
		m_postOverlay.Release();

		CView::Release();
	}

	void CViewOverlay::OnResize(const Math::Rect& rect)
	{
		CView::OnResize(rect);

		m_postOverlay.OnResize(rect);
	}
};
