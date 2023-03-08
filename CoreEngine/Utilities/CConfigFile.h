//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CConfigFile.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCONFIGFILE_H
#define CCONFIGFILE_H

#include "../Globals/CGlobals.h"
#include "CDetectComment.h"
#include <string>
#include <functional>

namespace Util
{
	class CConfigFile
	{
	public:
		struct Data
		{
			std::wstring filename;
			std::function<void(const std::wstring&, const std::vector<std::vector<std::wstring>>&)> processConfig;
		};

	private:
		enum SECTION_TYPE
		{
			SECTION_TYPE_ROOT,
			SECTION_TYPE_PROPERTY,
			SECTION_TYPE_BLOCK,
			SECTION_TYPE_SUBBLOCK,
		};

	public:
		CConfigFile();
		~CConfigFile();
		CConfigFile(const CConfigFile&) = delete;
		CConfigFile(CConfigFile&&) = delete;
		CConfigFile& operator = (const CConfigFile&) = delete;
		CConfigFile& operator = (CConfigFile&&) = delete;

		bool Parse();

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void Parse(wchar_t ch);

		void ProcessRoot(wchar_t ch);
		void ProcessProperty(wchar_t ch);
		void ProcessBlock(wchar_t ch);
		void ProcessSubBlock(wchar_t ch);

	private:
		bool m_bInQuote;
		bool m_bSingleSubBlock;
		SECTION_TYPE m_sectionType;

		Data m_data;
		
		std::wstring m_code;

		std::wstring m_property;
		std::wstring m_elem;
		std::vector<std::wstring> m_elemList;
		std::vector<std::vector<std::wstring>> m_propertyElemList;

		CDetectComment<std::wstring, wchar_t> m_detectComment;
		CompilerWord<std::wstring, wchar_t> m_word;
	};
};

#endif
