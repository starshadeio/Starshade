//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Utilities/CFileSystem.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CFILESYSTEM_H
#define CFILESYSTEM_H

#include <string>

namespace Util
{
	class CFileSystem
	{
	public:
		static CFileSystem& Instance()
		{
			static CFileSystem instance;
			return instance;
		}

	private:
		CFileSystem();
		~CFileSystem();
		CFileSystem(const CFileSystem&) = delete;
		CFileSystem(CFileSystem&&) = delete;
		CFileSystem& operator = (const CFileSystem&) = delete;
		CFileSystem& operator = (CFileSystem&&) = delete;

	public:
		bool VerifyDirectory(const wchar_t* dir);
		bool NewDirectory(const wchar_t* dir);
		bool NewPath(const wchar_t* path);
		bool DeleteDirectory(const wchar_t* dir);

		void OpenFolderWithTarget(const wchar_t* path, const wchar_t* target);

		bool OpenFile(const wchar_t* pTitle, std::wstring& path, bool bFolder = false, std::pair<std::wstring, std::wstring>* pFileTypeList = nullptr, size_t fileTypeCount = 0);
		bool SaveFile(const wchar_t* pTitle, std::wstring& path, bool bFolder = false, std::pair<std::wstring, std::wstring>* pFileTypeList = nullptr, size_t fileTypeCount = 0);
		bool CopyFileTo(const wchar_t* srcFile, const wchar_t* dstFile, bool bAllowOverwrite);

		bool RenameFile(const wchar_t* filenameOld, const wchar_t* filenameNew);
		void SplitDirectoryFilenameExtension(const wchar_t* path, std::wstring& dir, std::wstring& filename, std::wstring& extension);

		// Accessors.
		inline const std::wstring& GetDataPath() const { return m_dataPath; }

	private:
		bool FindDataPath();

	private:
		std::wstring m_dataPath;
	};
};

#endif
