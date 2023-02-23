//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CDebugError.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDEBUGERROR_H
#define CDEBUGERROR_H

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include <cstdlib>
#endif

#include <cassert>
#include <cstdlib>

#ifdef NDEBUG
inline void ReportError(const char* expr)
{
	//static const char* format = "%s";

#ifdef PLATFORM_WINDOWS
	//static const int MAX_CHARS = 1024;
	//static char buffer[MAX_CHARS];
	//snprintf(buffer, MAX_CHARS, format, expr);
	MessageBoxA(nullptr, expr, "Error", MB_OK);
#else
	printf(expr);
#endif

	exit(0);
}
#endif

#ifndef NDEBUG
#define ASSERT(cond) { assert(cond); }
#define ASSERT_R(cond) { assert(cond); }
#else
#define ASSERT(cond) { cond; }
#define ASSERT_R(cond) { if(!(cond)) { ReportError(#cond); } }
#endif

#ifdef PLATFORM_WINDOWS

#define assertHR(hr) { assert(SUCCEEDED(hr)); }

#ifndef NDEBUG
#define ASSERT_HR(hr) { assert(SUCCEEDED(hr)); }
#define ASSERT_HR_R(hr) { assert(SUCCEEDED(hr)); }
#else
#define ASSERT_HR(hr) { hr; }
#define ASSERT_HR_R(hr) { if(FAILED(hr)) { ReportError(#hr); } }
#endif

#endif

#endif
