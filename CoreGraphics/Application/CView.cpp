//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CView.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CView.h"
#include "CSceneManager.h"
#include <Math/CMathFNV.h>

namespace App
{
	CView::CView(const wchar_t* pName) :
		m_pName(pName),
		m_hash(Math::FNV1a_32(pName)),
		m_postProcessor(pName)
	{
		CSceneManager::Instance().RegisterView(this, m_hash);
	}

	CView::~CView() { }
	
	void CView::Initialize()
	{
		m_postProcessor.Initialize();
	}

	void CView::PostInitialize()
	{
		m_postProcessor.PostInitialize();
	}

	bool CView::Load()
	{
		if(m_bLoaded) { return false; }
		m_bLoaded = true;
		return true;
	}

	bool CView::Unload()
	{
		if(!m_bLoaded) { return false; }
		m_bLoaded = false;
		return true;
	}

	void CView::Release()
	{
		m_postProcessor.Release();
	}

	void CView::OnResize(const Math::Rect& rect)
	{
		m_postProcessor.OnResize(rect);
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CView::QueueRender()
	{
		CSceneManager::Instance().RenderingSystem().QueueRender(&m_postProcessor, m_hash);
	}
};
