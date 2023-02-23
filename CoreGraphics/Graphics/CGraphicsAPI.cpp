//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CGraphicsAPI.h
//
//-------------------------------------------------------------------------------------------------

#include "CGraphicsAPI.h"
#include <Utilities/CFileSystem.h>
#include <Utilities/CTextUtil.h>
#include <unordered_map>
#include <algorithm>

namespace Graphics
{
	CGraphicsAPI::CGraphicsAPI()
	{
	}

	CGraphicsAPI::~CGraphicsAPI()
	{
	}

	bool CGraphicsAPI::Screenshot(const wchar_t* filename)
	{
		std::wstring dir;
		std::wstring name;
		std::wstring ext;
		Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(filename, dir, name, ext);

		if(!Util::CFileSystem::Instance().VerifyDirectory(dir.c_str()))
		{
			Util::CFileSystem::Instance().NewPath(dir.c_str());
		}

		static const std::unordered_map<std::wstring, IMAGE_FILETYPE> IMAGE_FILETYPE_MAP = {
			{ L".dds", IMAGE_FILETYPE_DDS },
			{ L".bmp", IMAGE_FILETYPE_BMP },
			{ L".ico", IMAGE_FILETYPE_ICO },
			{ L".gif", IMAGE_FILETYPE_GIF },
			{ L".jpeg", IMAGE_FILETYPE_JPEG },
			{ L".jpg", IMAGE_FILETYPE_JPEG },
			{ L".png", IMAGE_FILETYPE_PNG },
			{ L".tiff", IMAGE_FILETYPE_TIFF },
			{ L".wmp", IMAGE_FILETYPE_WMP },
		};

		std::transform(ext.begin(), ext.end(), ext.begin(), [](wchar_t c) { return std::tolower(c); });

		return Screenshot((dir + name + ext).c_str(), IMAGE_FILETYPE_MAP.find(ext)->second);
	}
};
