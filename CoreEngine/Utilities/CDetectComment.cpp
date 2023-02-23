//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CDetectComment.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CDetectComment.h"
#include "CTextUtil.h"

namespace Util
{
	const Util::CDSTrie<COMMENT_TYPE, std::string, char> CDetectComment<std::string, char>::m_commandTrie = {
		{ "//", COMMENT_TYPE_SINGLE_OPEN },
		{ "\n", COMMENT_TYPE_SINGLE_CLOSE },
		{ "/*", COMMENT_TYPE_MULTI_OPEN },
		{ "*/", COMMENT_TYPE_MULTI_CLOSE },
	};

	const Util::CDSTrie<COMMENT_TYPE, std::wstring, wchar_t> CDetectComment<std::wstring, wchar_t>::m_commandTrie = {
		{ L"//", COMMENT_TYPE_SINGLE_OPEN },
		{ L"\n", COMMENT_TYPE_SINGLE_CLOSE },
		{ L"/*", COMMENT_TYPE_MULTI_OPEN },
		{ L"*/", COMMENT_TYPE_MULTI_CLOSE },
	};

	const Util::CDSTrie<COMMENT_TYPE, std::u32string, char32_t> CDetectComment<std::u32string, char32_t>::m_commandTrie = {
		{ U"//", COMMENT_TYPE_SINGLE_OPEN },
		{ U"\n", COMMENT_TYPE_SINGLE_CLOSE },
		{ U"/*", COMMENT_TYPE_MULTI_OPEN },
		{ U"*/", COMMENT_TYPE_MULTI_CLOSE },
	};
};
