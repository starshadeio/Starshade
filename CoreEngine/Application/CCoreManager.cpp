//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Application/CCompManager.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CCoreManager.h"

namespace App
{
	CCoreManager::CCoreManager()
	{
	}

	CCoreManager::~CCoreManager()
	{
	}
	
	void CCoreManager::Initialize()
	{
		m_nodeRegistry.Initialize();
	}
	
	void CCoreManager::PostInitialize()
	{
		m_nodeRegistry.PostInitialize();
	}

	void CCoreManager::Update()
	{
		m_nodeRegistry.Update();
		m_transformQueue.Update();
	}

	void CCoreManager::LateUpdate()
	{
		m_nodeRegistry.LateUpdate();
	}
	
	void CCoreManager::Release()
	{
		m_nodeRegistry.Release();
	}
	
	//-----------------------------------------------------------------------------------------------
	// Adjustment methods.
	//-----------------------------------------------------------------------------------------------

	void CCoreManager::OnMove(const Math::Rect& rect)
	{
		m_nodeRegistry.OnMove(rect);
	}
	
	void CCoreManager::OnResize(const Math::Rect& rect)
	{
		m_nodeRegistry.OnResize(rect);
	}
	
	//-----------------------------------------------------------------------------------------------
	// File methods.
	//-----------------------------------------------------------------------------------------------

	void CCoreManager::SaveToFile(std::ofstream& file) const
	{
	}

	void CCoreManager::LoadFromFile(std::ifstream& file)
	{
	}
};
