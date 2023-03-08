//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CUniverseManager.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUniverseManager.h"

namespace Universe
{
	CUniverseManager::CUniverseManager()
	{
	}

	CUniverseManager::~CUniverseManager()
	{
	}

	void CUniverseManager::Initialize()
	{
		m_chunkManager.Initialize();
	}

	void CUniverseManager::Update()
	{
		m_chunkManager.Update();
	}

	void CUniverseManager::LateUpdate()
	{
		m_chunkManager.LateUpdate();
	}

	void CUniverseManager::Release()
	{
		m_chunkManager.Release();
	}
};
