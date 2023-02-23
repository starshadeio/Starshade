//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CSceneFileIO.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSceneFileIO.h"
#include <Utilities/CFileSystem.h>
#include <Utilities/CDebugError.h>
#include <Utilities/CConvertUtil.h>
#include "CScene.h"
#include <cassert>

namespace App
{
	CSceneFileIO::CSceneFileIO()
	{
	}

	CSceneFileIO::~CSceneFileIO()
	{
	}

	bool CSceneFileIO::Save(bool bPrompt)
	{
		if(bPrompt || m_path.empty())
		{
			if(!Util::CFileSystem::Instance().SaveFile(L"Save File As", m_path))
			{
				return false;
			}
		}

		std::ofstream file(m_path.c_str(), std::ios::out | std::ios::binary);
		if(!file.is_open())
		{
			return false;
		}

		for(auto pScene : m_sceneList)
		{
			pScene->SaveToFile(file);
		}
		
		file.close();

		return true;
	}

	bool CSceneFileIO::Load()
	{
		if(!Util::CFileSystem::Instance().OpenFile(L"Open File", m_path))
		{
			return false;
		}

		std::ifstream file(m_path.c_str(), std::ios::in | std::ios::binary);
		if(!file.is_open())
		{
			return Save();
		}

		for(auto pScene : m_sceneList)
		{
			pScene->LoadFromFile(file);
		}

		file.close();

		return true;
	}
};
