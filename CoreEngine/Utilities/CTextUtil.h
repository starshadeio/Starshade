//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CTextUtil.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CTEXTUTIL_H
#define CTEXTUTIL_H

#include <string>
#include <codecvt>

namespace Util
{
	inline std::wstring s2ws(const std::string& str)
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(str);
	}

	inline std::string ws2s(const std::wstring& wstr)
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(wstr);
	}
};

#endif
