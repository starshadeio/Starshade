//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Application/CLocalizedString.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CLOCALIZEDSTRING_H
#define CLOCALIZEDSTRING_H

#include <string>
#include <vector>
#include <initializer_list>

namespace App
{
	class CLocalizedString
	{
	public:
		CLocalizedString(const std::wstring& str, const std::wstring& split);
		~CLocalizedString();
		CLocalizedString(const CLocalizedString&) = delete;
		CLocalizedString(CLocalizedString&&) = delete;
		CLocalizedString& operator = (const CLocalizedString&) = delete;
		CLocalizedString& operator = (CLocalizedString&&) = delete;

		std::wstring Format(std::initializer_list<std::wstring> valList) const;

		// Accessors.
		inline const std::wstring& Get(size_t index = 0) const { return m_strList[index]; }

	private:
		void ParseString(const std::wstring& str, const std::wstring& split);

	private:
		std::vector<std::wstring> m_strList;
	};
};

#endif
