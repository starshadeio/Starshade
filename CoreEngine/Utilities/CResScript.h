//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CResScript.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CRESSCRIPT_H
#define CRESSCRIPT_H

#include "../Globals/CGlobals.h"
#include "CDetectComment.h"
#include "CCompilerUtil.h"
#include <unordered_set>
#include <string>
#include <functional>
#include <vector>

namespace Util
{
	class CResScript
	{
	private:
		static const std::unordered_map<std::wstring, std::function<void(CResScript*)>> MACRO_MAP;

	private:
		enum SECTION_TYPE
		{
			SECTION_TYPE_ROOT,
			SECTION_TYPE_CATEGORY,
			SECTION_TYPE_RESOURCE,
			SECTION_TYPE_MACRO,
		};

	public:
		struct Data
		{
			std::wstring filename;
			std::wstring filepath;
			std::function<void(const std::wstring&, const Util::CompilerTuple<3, std::wstring, wchar_t>&)> addResource;
		};

	public:
		CResScript();
		~CResScript();
		CResScript(const CResScript&) = delete;
		CResScript(CResScript&&) = delete;
		CResScript& operator = (const CResScript&) = delete;
		CResScript& operator = (CResScript&&) = delete;
		
		bool Build();
		void Build(const std::wstring& code);

		// Accessors.
		inline const std::wstring& GetFilename() const { return m_data.filename; }
		inline const std::wstring& GetFilepath() const { return m_data.filepath; }
		inline const std::wstring& GetCode() const { return m_code; }
		inline std::pair<size_t, size_t> GetGroupIndices(u32 hash) const { return m_groupMap.find(hash)->second; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		bool Build(const wchar_t* filename);
		void Parse(wchar_t ch);

		bool ProcessRoot(wchar_t ch);
		bool ProcessCategory(wchar_t ch);
		bool ProcessResource(wchar_t ch);
		bool ProcessMacro(wchar_t ch);

		void MacroInclude();

	private:
		bool m_bInQuote;
		SECTION_TYPE m_sectionType;

		Data m_data;
		std::wstring m_code;
		size_t m_codeIndex;

		CDetectComment<std::wstring, wchar_t> m_detectComment;

		CompilerWord<std::wstring, wchar_t> m_word;
		std::vector<std::wstring> m_category;
		CompilerTuple<3, std::wstring, wchar_t> m_resource;
		CompilerTuple<2, std::wstring, wchar_t> m_macro;
		std::unordered_set<u32> m_includeSet;

		std::unordered_map<u32, std::pair<size_t, size_t>> m_groupMap;
		std::pair<size_t, size_t>* m_pGroup;
	};
};

#endif
