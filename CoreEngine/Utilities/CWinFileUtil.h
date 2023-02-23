//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Utilities/CWinFileUtil.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CWINFILEUTIL_H
#define CWINFILEUTIL_H

#include "CDebugError.h"

#include <Windows.h>
#define STRICT_TYPED_ITEMIDS
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC
#include <new>
#include <string>
#include <vector>

namespace Util
{
	/* File Dialog Event Handler *****************************************************************************************************/

	class CDialogEventHandler : public IFileDialogEvents,
		public IFileDialogControlEvents
	{
	public:
		// IUnknown methods
		IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
		{
			static const QITAB qit[] = {
				QITABENT(CDialogEventHandler, IFileDialogEvents),
				QITABENT(CDialogEventHandler, IFileDialogControlEvents),
				{ 0 },
				#pragma warning(suppress:4838)
			};

			return QISearch(this, qit, riid, ppv);
		}

		IFACEMETHODIMP_(ULONG) AddRef()
		{
			return InterlockedIncrement(&_cRef);
		}

		IFACEMETHODIMP_(ULONG) Release()
		{
			long cRef = InterlockedDecrement(&_cRef);
			if (!cRef)
			delete this;
			return cRef;
		}

		// IFileDialogEvents methods
		IFACEMETHODIMP OnFileOk(IFileDialog *) { return S_OK; };
		IFACEMETHODIMP OnFolderChange(IFileDialog *) { return S_OK; };
		IFACEMETHODIMP OnFolderChanging(IFileDialog *, IShellItem *) { return S_OK; };
		IFACEMETHODIMP OnHelp(IFileDialog *) { return S_OK; };
		IFACEMETHODIMP OnSelectionChange(IFileDialog *) { return S_OK; };
		IFACEMETHODIMP OnShareViolation(IFileDialog *, IShellItem *, FDE_SHAREVIOLATION_RESPONSE *) { return S_OK; };
		IFACEMETHODIMP OnTypeChange(IFileDialog *pfd) { return S_OK; };
		IFACEMETHODIMP OnOverwrite(IFileDialog *, IShellItem *, FDE_OVERWRITE_RESPONSE *) { return S_OK; };

		// IFileDialogControlEvents methods
		IFACEMETHODIMP OnItemSelected(IFileDialogCustomize *pfdc, DWORD dwIDCtl, DWORD dwIDItem) { return S_OK; };
		IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize *, DWORD) { return S_OK; };
		IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize *, DWORD, BOOL) { return S_OK; };
		IFACEMETHODIMP OnControlActivating(IFileDialogCustomize *, DWORD) { return S_OK; };

		CDialogEventHandler() : _cRef(1) { };

	private:
		~CDialogEventHandler() { };
		long _cRef;
	};

	// Instance creation helper
	HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void **ppv)
	{
		*ppv = NULL;
		CDialogEventHandler *pDialogEventHandler = new (std::nothrow) CDialogEventHandler();
		HRESULT hr = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;

		if(SUCCEEDED(hr))
		{
			hr = pDialogEventHandler->QueryInterface(riid, ppv);
			pDialogEventHandler->Release();
		}

		return hr;
	}

	//-----------------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------

	inline bool FileDialog(const wchar_t* pTitle, bool bOpen, std::wstring& path, bool bFolder = false, std::pair<std::wstring, std::wstring>* pFileTypeList = nullptr, size_t fileTypeCount = 0)
	{
		assert(bOpen || !bFolder);

		IFileDialog* pfd = nullptr;
		ASSERT_HR_R(CoCreateInstance(bOpen ? CLSID_FileOpenDialog : CLSID_FileSaveDialog, 
			nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd)));

		IFileDialogEvents* pfde = nullptr;
		ASSERT_HR_R(CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde)));

		ASSERT_HR_R(pfd->SetTitle(pTitle));

		DWORD dwCookie;
		ASSERT_HR_R(pfd->Advise(pfde, &dwCookie));

		DWORD dwFlags;
		ASSERT_HR_R(pfd->GetOptions(&dwFlags));
		ASSERT_HR_R(pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM | (bFolder ? FOS_PICKFOLDERS : 0)));

		if(!bFolder)
		{
			std::wstring defaultExt;
			std::vector<COMDLG_FILTERSPEC> fileTypeList;
			if(pFileTypeList)
			{
				for(size_t i = 0; i < fileTypeCount; ++i)
				{
					fileTypeList.push_back({ pFileTypeList[i].first.c_str(), pFileTypeList[i].second.c_str() });
				}

				size_t offset = pFileTypeList[0].second.size();
				while(offset > 0 && pFileTypeList[0].second[offset - 1] != L'.') { --offset; }
				defaultExt = pFileTypeList[0].second.substr(offset);
			}

			fileTypeList.push_back({ L"All Documents (*.*)", L"*.*" });
			
			ASSERT_HR_R(pfd->SetFileTypes(static_cast<UINT>(fileTypeList.size()), fileTypeList.data()));
			ASSERT_HR_R(pfd->SetFileTypeIndex(0));
			ASSERT_HR_R(pfd->SetDefaultExtension(defaultExt.c_str()));
		}

		{
			WCHAR appPath[256];
			GetModuleFileNameW(nullptr, appPath, 256);

			size_t sz = wcslen(appPath) - 1;
			while(sz > 0 && appPath[sz - 1] != '\\')
			{
				--sz;
			}

			appPath[sz] = L'\0';

			IShellItem* folder;
			ASSERT_HR_R(SHCreateItemFromParsingName(appPath, nullptr, IID_PPV_ARGS(&folder)));

			ASSERT_HR_R(pfd->SetDefaultFolder(folder));
			folder->Release();
		}

		bool bSuccess = SUCCEEDED(pfd->Show(nullptr));
		if(bSuccess)
		{
			IShellItem* psiResult;
			ASSERT_HR_R(pfd->GetResult(&psiResult));

			PWSTR pszFilePath = nullptr;
			ASSERT_HR_R(psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath));
			path = pszFilePath;

			CoTaskMemFree(pszFilePath);

			psiResult->Release();
		}

		pfd->Unadvise(dwCookie);
		pfde->Release();
		pfd->Release();

		return bSuccess;
	}

	inline void OpenFolder(const wchar_t* path, const wchar_t* target)
	{
		SFGAOF attributes;
		PIDLIST_ABSOLUTE folder;
		PIDLIST_ABSOLUTE file;
		ASSERT_HR_R(SHParseDisplayName(path, nullptr, &folder, 0, &attributes));

		wchar_t fullPath[MAX_PATH];

		if(target)
		{
			PathCombine(fullPath, path, target);
			ASSERT_HR_R(SHParseDisplayName(fullPath, nullptr, &file, 0, &attributes));

			PCUITEMID_CHILD child { reinterpret_cast<ITEMID_CHILD*>(&file->mkid) };

			SHOpenFolderAndSelectItems(folder, 1, &child, 0);

			CoTaskMemFree(file);
		}
		else
		{
			SHOpenFolderAndSelectItems(folder, 0, nullptr, 0);
		}

		CoTaskMemFree(folder);
	}
};

#endif
