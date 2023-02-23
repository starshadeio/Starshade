//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Application/CLocalization.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CLOCALIZATION_H
#define CLOCALIZATION_H

#include "CLocalizedString.h"
#include "../Utilities/CResScript.h"
#include "../Globals/CGlobals.h"
#include <string>
#include <unordered_map>

namespace App
{
	class CLocalization
	{
	public:
		static CLocalization& Instance()
		{
			static CLocalization instance;
			return instance;
		}

	private:
		CLocalization();
		~CLocalization();
		CLocalization(const CLocalization&) = delete;
		CLocalization(CLocalization&&) = delete;
		CLocalization& operator = (const CLocalization&) = delete;
		CLocalization& operator = (CLocalization&&) = delete;

	public:
		void RegisterFile(const std::wstring& key, const std::wstring& filepath);
		void Load(u64 hash);
		void SaveProductionFile(const wchar_t* filepath, const wchar_t* prodPath);

		// Accessors.
		inline const CLocalizedString& Get(u64 key) const { return m_stringMap.find(key)->second; }

	private:
		void AddResource(const std::wstring& category, const Util::CompilerTuple<3, std::wstring, wchar_t>& res);

	private:
		Util::CResScript m_resScript;

		std::unordered_map<u64, CLocalizedString> m_stringMap;
		std::unordered_map<u64, std::wstring> m_fileMap;
	};
};

#endif
