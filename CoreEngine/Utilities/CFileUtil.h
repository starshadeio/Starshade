//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CFileUtil.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CFILEUTIL_H
#define CFILEUTIL_H

//#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
//#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include <string>
#include <sstream>
#include <fstream>
#include <codecvt>
#include <locale>

namespace Util {
	inline std::wstring ReadFileUTF8(const wchar_t* filename)
	{
		static const std::locale utf8_locale = std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>());

    std::basic_ifstream<wchar_t> fin(filename);
		if(!fin.is_open()) { return L""; }

		fin.imbue(utf8_locale);

    std::wstring str { };

		while(fin.peek() == 0xFEFF) { fin.get(); }

    std::getline(fin, str, L'\0');
		fin.close();
    return str;
	}

	inline bool WriteFileUTF8(const wchar_t* filename, const std::wstring& contents)
	{
		static const std::locale utf8_locale = std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>());

    std::basic_ofstream<wchar_t> fin(filename);
		if(!fin.is_open()) { return false; }

		fin.imbue(utf8_locale);

		fin << contents;
		fin.close();

		return true;
	}
};

#endif
