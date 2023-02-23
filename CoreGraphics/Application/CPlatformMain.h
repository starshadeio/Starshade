//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CPlatformMain.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPLATFORMMAIN_H
#define CPLATFORMMAIN_H

#include "../Factory/CFactory.h"

//-------------------------------------------------------------------------------------------------
// DEFINES
//-------------------------------------------------------------------------------------------------

#ifndef WUI_FILEPATH
#define WUI_FILEPATH L"../Resources/UI/WUI/Default.wui"
#endif

#ifndef APP_INSTANCE
#define APP_INSTANCE nullptr
#endif

//-------------------------------------------------------------------------------------------------
// Target Platform: Windows
//-------------------------------------------------------------------------------------------------

#ifdef PLATFORM_WINDOWS

#include "CWinPlatform.h"
#include <Windows.h>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	App::CWinPlatform* pWinPlatform = dynamic_cast<App::CWinPlatform*>(CFactory::Instance().GetPlatform());
	App::CWinPlatform::WinData winData { };
	winData.hInstance = hInstance;
	winData.nCmdShow = nCmdShow;
	winData.wuiFilename = WUI_FILEPATH;

	return pWinPlatform->Run(APP_INSTANCE, &winData);
}

#endif

//-------------------------------------------------------------------------------------------------
// Target Platform: Mac
//-------------------------------------------------------------------------------------------------

#ifdef PLATFORM_MAC
#endif

//-------------------------------------------------------------------------------------------------
// Target Platform: Linux
//-------------------------------------------------------------------------------------------------

#ifdef PLATFORM_LINUX
#endif

#endif
