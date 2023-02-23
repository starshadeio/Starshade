//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CViewScene.cpp
//
//-------------------------------------------------------------------------------------------------

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
	CViewScene::CViewScene() : 
		CView(VIEW_NAME_MAIN)
	{
	}

	CViewScene::~CViewScene()
	{
	}

	void CViewScene::Initialize()
	{
		{ // Setup post processor.
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

			data.colorTexKey[0] = L"VIEW_MAIN_COLOR_0";
			data.colorTexKey[1] = L"VIEW_MAIN_COLOR_1";
			data.colorTexKey[2] = L"VIEW_MAIN_COLOR_2";
			data.colorTexKey[3] = L"VIEW_MAIN_COLOR_3";
			data.depthTexKey = L"VIEW_MAIN_DEPTH";

			SetPostProcessorData(data);
		}

		CView::Initialize();

		{ // Post effects.
			RegisterPostEffect(&m_postSky);
			RegisterPostEffect(&m_postEdge);
			RegisterPostEffect(&m_postSSAO);
			RegisterPostEffect(&m_postLighting);
			RegisterPostEffect(&m_postToneMapping);
			RegisterPostEffect(&m_postFXAA);

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
	}

	void CViewScene::PostInitialize()
	{
		CView::PostInitialize();
		
		m_postSky.PostInitialize();
		m_postEdge.PostInitialize();
		m_postSSAO.PostInitialize();
		m_postLighting.PostInitialize();
		m_postToneMapping.PostInitialize();
		m_postFXAA.PostInitialize();
	}

	bool CViewScene::Load()
	{
		if(!CView::Load()) return false;
		return true;
	}

	bool CViewScene::Unload()
	{
		if(!CView::Unload()) return false;
		return true;
	}

	void CViewScene::LateUpdate()
	{
		QueueRender();
	}

	void CViewScene::Release()
	{
		m_postFXAA.Release();
		m_postToneMapping.Release();
		m_postLighting.Release();
		m_postSSAO.Release();
		m_postEdge.Release();
		m_postSky.Release();

		CView::Release();
	}

	void CViewScene::OnResize(const Math::Rect& rect)
	{
		CView::OnResize(rect);

		m_postSky.OnResize(rect);
		m_postEdge.OnResize(rect);
		m_postSSAO.OnResize(rect);
		m_postLighting.OnResize(rect);
		m_postToneMapping.OnResize(rect);
		m_postFXAA.OnResize(rect);
	}
};
