//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CTexture.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CTexture.h"
#include <Utilities/CFileSystem.h>
#include "../Resources/CResourceManager.h"
#include <Utilities/CDebugError.h>
#include <fstream>

namespace Graphics
{
	CTexture::CTexture()
	{
	}

	CTexture::~CTexture()
	{
	}

	void CTexture::SaveProductionFile()
	{
		if(m_data.filename.empty()) return;

		std::wstring path;
		std::wstring filename;
		std::wstring extension;
		Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(m_data.filename.c_str(), path, filename, extension);
		
		path = Resources::CManager::Instance().GetProductionPath() + path;//.substr(wcslen(Resources::CManager::Instance().GetFilePath()));
		ASSERT_R(Util::CFileSystem::Instance().NewPath(path.c_str()));

		ASSERT_R(Util::CFileSystem::Instance().CopyFileTo(m_data.filename.c_str(), (path + filename + extension).c_str(), true));
	}
};
