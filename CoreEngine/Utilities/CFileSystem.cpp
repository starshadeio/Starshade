//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Utilities/CFileSystem.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CFileSystem.h"
#include "CConvertUtil.h"
#ifdef PLATFORM_WINDOWS
#include "CWinFileUtil.h"
#include <Windows.h>
#include <shlwapi.h>
#include <ShlObj.h>
#pragma comment(lib,"shlwapi.lib")
#endif

namespace Util
{
	CFileSystem::CFileSystem()
	{
		FindDataPath();
	}

	CFileSystem::~CFileSystem()
	{
	}
	
	//------------------------------------------------------------------------------------------------
	// Directory methods.
	//------------------------------------------------------------------------------------------------

	bool CFileSystem::VerifyDirectory(const wchar_t* dir)
	{
#ifdef PLATFORM_WINDOWS
		DWORD attrib = GetFileAttributesW(dir);
		if(attrib != INVALID_FILE_ATTRIBUTES && attrib == FILE_ATTRIBUTE_DIRECTORY)
		{
			return true;
		}
#endif

		return false;
	}

	bool CFileSystem::NewDirectory(const wchar_t* path)
	{
#ifdef PLATFORM_WINDOWS
		if(CreateDirectoryW(path, nullptr))
		{
			return true;
		}
#endif

		return false;
	}

	bool CFileSystem::NewPath(const wchar_t* path)
	{
		bool bResult = true;
		std::wstring fullPath = path;
		std::wstring pathBuilder = L"";

		Util::SplitString(fullPath, L'/', [&](u32 index, const std::wstring& dir){
			if(!bResult || dir.empty()) return;

			pathBuilder += dir + L"/";
			if(!Util::CFileSystem::Instance().VerifyDirectory(pathBuilder.c_str()))
			{
				if(!Util::CFileSystem::Instance().NewDirectory(pathBuilder.c_str()))
				{
					bResult = false;
					return;
				}
			}
		});

		return bResult;
	}
	
	//------------------------------------------------------------------------------------------------
	// Directory methods.
	//------------------------------------------------------------------------------------------------

	void CFileSystem::OpenFolderWithTarget(const wchar_t* path, const wchar_t* target)
	{
		OpenFolder(path, target);
	}

	bool CFileSystem::DeleteDirectory(const wchar_t* dir)
	{
		std::wstring str = dir;
		str.resize(str.size() + 2, L'\0');
		
		SHFILEOPSTRUCTW file_op = {
			NULL,
			FO_DELETE,
			str.c_str(),
			NULL,
			FOF_NOCONFIRMATION |
			FOF_NOERRORUI |
			FOF_SILENT,
			false,
			0,
			L""
		};

		return SHFileOperation(&file_op) == 0;
	}

	bool CFileSystem::RenameFile(const wchar_t* filenameOld, const wchar_t* filenameNew)
	{
#ifdef PLATFORM_WINDOWS
		if(MoveFileW(filenameOld, filenameNew))
		{
			return true;
		}
#endif

		return false;
	}
	
	void CFileSystem::SplitDirectoryFilenameExtension(const wchar_t* path, std::wstring& dir, std::wstring& filename, std::wstring& extension)
	{
		size_t pathLen = wcslen(path);

		size_t extLen = 1;
		while(extLen < pathLen && path[pathLen - extLen] != '.')
		{
			++extLen;
		}

		size_t dirLen = pathLen - extLen;
		while(dirLen > 0 && path[dirLen - 1] != '/' && path[dirLen - 1] != '\\')
		{
			--dirLen;
		}

		dir.reserve(dirLen);
		filename.reserve(pathLen - dirLen - extLen);
		extension.reserve(extLen);

		size_t index = 0;
		for(; index < dirLen; ++index)
		{
			dir.push_back(path[index]);
		}

		for(; index < pathLen - extLen; ++index)
		{
			filename.push_back(path[index]);
		}

		for(; index < pathLen; ++index)
		{
			extension.push_back(path[index]);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//------------------------------------------------------------------------------------------------

	bool CFileSystem::OpenFile(const wchar_t* pTitle, std::wstring& path, bool bFolder, std::pair<std::wstring, std::wstring>* pFileTypeList, size_t fileTypeCount)
	{
		return FileDialog(pTitle, true, path, bFolder, pFileTypeList, fileTypeCount);
	}

	bool CFileSystem::SaveFile(const wchar_t* pTitle, std::wstring& path, bool bFolder, std::pair<std::wstring, std::wstring>* pFileTypeList, size_t fileTypeCount)
	{
		return FileDialog(pTitle, false, path, bFolder, pFileTypeList, fileTypeCount);
	}
	
	bool CFileSystem::CopyFileTo(const wchar_t* srcFile, const wchar_t* dstFile, bool bAllowOverwrite)
	{
#ifdef PLATFORM_WINDOWS
		return CopyFileW(srcFile, dstFile, !bAllowOverwrite);
#endif
	}

	//------------------------------------------------------------------------------------------------
	// Internals.
	//------------------------------------------------------------------------------------------------

	bool CFileSystem::FindDataPath()
	{
#ifdef PLATFORM_WINDOWS
		wchar_t buffer[MAX_PATH];
		if(SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, buffer)))
		{
			m_dataPath = buffer;
			return true;
		}
#endif

		return false;
	}
};
