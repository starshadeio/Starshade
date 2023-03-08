//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioClip.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAudioClip.h"
#include "CAudioLoader.h"
#include "../Resources/CResourceManager.h"
#include <Utilities/CFileSystem.h>
#include <Utilities/CConvertUtil.h>
#include <Utilities/CDebugError.h>
#include <fstream>

namespace Audio
{
	CAudioClip::CAudioClip() { }
	CAudioClip::~CAudioClip() { }

	void CAudioClip::Initialize()
	{
		memset(&m_info, 0, sizeof(m_info));

		Util::SplitString(m_data.meta, L':', [this](u32 index, const std::wstring& elem){
			switch(index)
			{
				case 0:
					m_info.bStream = Util::StringToBool(elem);
					break;
				default:
					break;
			}
		});

		CAudioLoader::Load(m_data.filename, m_info);
	}
	
	void CAudioClip::SaveProductionFile()
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

	void CAudioClip::Release() { }
};
