//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CScene.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CScene.h"
#include "CSceneManager.h"
#include <Math/CMathFNV.h>

namespace App
{
	CScene::CScene(const wchar_t* pName, SceneType sceneType) :
		m_pName(pName),
		m_hash(Math::FNV1a_32(pName))
	{
		CSceneManager::Instance().RegisterScene(this, m_hash, sceneType);
	}

	CScene::~CScene() { }
};
