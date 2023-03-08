//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CResScript.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CResScript.h"
#include "CDebugError.h"
#include "CFileUtil.h"
#include "../Math/CMathFNV.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <locale>
#include <codecvt>

namespace Util
{
	const std::unordered_map<std::wstring, std::function<void(CResScript*)>> CResScript::MACRO_MAP = {
		{L"include", [](CResScript* pResFile){ pResFile->MacroInclude(); }}
	};

	CResScript::CResScript() : m_pGroup(nullptr)
	{
	}

	CResScript::~CResScript()
	{
	}
	
	bool CResScript::Build()
	{
		return Build(m_data.filename.c_str());
	}

	bool CResScript::Build(const wchar_t* filename)
	{
		const u32 hash = Math::FNV1a_32(filename);
		if(m_includeSet.find(hash) != m_includeSet.end()) { return false; }
		m_includeSet.insert(hash);

#ifndef PRODUCTION_BUILD
		bool bAwaitingPrintable = true;
		
		std::wstring input = ReadFileUTF8((m_data.filepath + filename).c_str());
		m_code.clear();
		m_codeIndex = 0;

		for(size_t i = 0; i < input.size(); ++i)
		{
			wchar_t ch = input[i];

			if(!m_bInQuote && ch > 0xFF)
			{
				continue;
			}

			Util::COMMENT_TYPE commentType;
			if(!m_bInQuote && (commentType = m_detectComment.InComment(ch, m_word)))
			{
				if((commentType & Util::COMMENT_TYPE::COMMENT_TYPE_OPEN) && !bAwaitingPrintable)
				{
					m_code.pop_back();
					bAwaitingPrintable = true;
				}

				// At any point we're in a macro and a new line is found, the macro needs to be completed.
				if(ch == '\n') {
					if(m_sectionType == SECTION_TYPE_MACRO) {
						ProcessMacro(ch);
					}
				}
			}
			else
			{
				if(ch == '\n')
				{
					if(!bAwaitingPrintable)
					{
						m_code.push_back(ch);
					}

					bAwaitingPrintable = true;
				}
				else if(!bAwaitingPrintable || ch > 0x20)
				{
					m_code.push_back(ch);
					bAwaitingPrintable = ch <= 0x20;
				}

				Parse(ch);
				m_codeIndex = m_code.size();
			}

		} Parse('\n'); // Parse an implicit new line to finalize any sections that are being processed.
#else
		Build(ReadFileUTF8((m_data.filepath + filename).c_str()));
#endif

		return true;
	}

	void CResScript::Build(const std::wstring& code)
	{
		m_code = code;
		for(auto ch : m_code)
		{
			Parse(ch);
			++m_codeIndex;
		} Parse('\n'); // Parse an implicit new line to finalize any sections that are being processed.
	}
	
	void CResScript::Parse(wchar_t ch) {
		switch(m_sectionType) {
			case SECTION_TYPE_ROOT: {
				ProcessRoot(ch);
			} break;
			case SECTION_TYPE_CATEGORY: {
				ProcessCategory(ch);
			} break;
			case SECTION_TYPE_RESOURCE: {
				ProcessResource(ch);
			} break;
			case SECTION_TYPE_MACRO: {
				ProcessMacro(ch);
			} break;
			default:
				break;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Section processors.
	//-----------------------------------------------------------------------------------------------
	
	// Method for processing the contents of root.
	bool CResScript::ProcessRoot(wchar_t ch) {
		if(ch == '{') { // Attempting to enter into the previously stated category.
			assert(!m_word.Empty());
			m_category.push_back(L"");
			m_word.Move(m_category.back());
			m_sectionType = SECTION_TYPE_CATEGORY;
			

			if(m_category.back()[0] == L'@')
			{
				m_pGroup = &m_groupMap.insert({ Math::FNV1a_32(m_category.back().substr(1).c_str()), { m_codeIndex - m_category.back().size() - 1, 0U } }).first->second;
			}
		}
		
		else if(ch > 0x20) {
			if(ch == '#') { // Attempting to enter into a macro.
				assert(m_word.Empty());
				m_sectionType = SECTION_TYPE_MACRO;
			}
			
			else { // If the category has whitespace between characters, it is simply ignored.
				m_word.AddChar(ch);
			}
		}

		return m_sectionType == SECTION_TYPE_ROOT;
	}

	// Method for processing the contents of a category.
	bool CResScript::ProcessCategory(wchar_t ch)
	{
		if(ch == '{')
		{ // Sub-category.
			assert(!m_word.Empty());
			m_category.push_back(L"");
			m_word.Move(m_category.back());
		}
		else if(ch == '}')
		{ // Exiting the category.
			m_category.pop_back();
			if(m_category.empty())
			{
				if(m_pGroup)
				{
					m_pGroup->second = m_codeIndex - m_pGroup->first + 1;
					m_pGroup = nullptr;
				}

				m_sectionType = SECTION_TYPE_ROOT;
			}
		}
		else if(ch == '(')
		{ // Entering into a resource block.
			m_sectionType = SECTION_TYPE_RESOURCE;
		}
		else if(ch > 0x20)
		{
			m_word.AddChar(ch);
		}

		return m_sectionType == SECTION_TYPE_CATEGORY;
	}

	// Method for processing a resource block.
	bool CResScript::ProcessResource(wchar_t ch)
	{
		if(!m_bInQuote && ch == ')')
		{
			m_resource.SetTarget(m_word);
			assert(m_resource.GetTargetIndex() >= 1);

			// Process resource.
			m_data.addResource(m_category.back(), m_resource);

			m_resource.Clear();
			m_sectionType = SECTION_TYPE_CATEGORY;
		}
		else
		{
			if(m_bInQuote && ch == '\"' && m_word.Peek() == '\\')
			{
				m_word.Pop();
				m_word.AddChar(ch);
			}
			else
			{
				if(!m_bInQuote && ch == ',')
				{
					m_resource.SetTarget(m_word);
					ASSERT(m_resource.Continue());
				}
				else if(ch == '\"')
				{
					m_bInQuote = !m_bInQuote;
				}
				else if(ch > 0x20 || m_bInQuote)
				{
					m_word.AddChar(ch);
				}
			}
		}
		
		return m_sectionType == SECTION_TYPE_RESOURCE;
	}

	// Method for processing a macro sequence.
	bool CResScript::ProcessMacro(wchar_t ch) {
		if(ch == '\n') {
			assert(!m_bInQuote);

			// Process contents of macro based on its keyword.
			m_macro.SetTarget(m_word);

			auto elem = MACRO_MAP.find(m_macro.GetElement(0));
			assert(elem != MACRO_MAP.end());
			elem->second(this);
		}

		else if(ch > 0x20) {
			if(ch == '\"') {
				m_bInQuote = !m_bInQuote;
			}

			m_word.AddChar(ch);
		}

		else {
			if(m_macro.GetElement(0).length() == 0) {
				m_macro.SetTarget(m_word);
			}

			if(m_macro.Continue()) {
				assert(m_macro.GetElement(0).length() > 0);
			}
		}
		
		return m_sectionType == SECTION_TYPE_MACRO;
	}

	//-----------------------------------------------------------------------------------------------
	// Macro methods.
	//-----------------------------------------------------------------------------------------------

	void CResScript::MacroInclude()
	{
		std::wstring include;
		bool bInQuote = false;

		for(int i = 0; i < m_macro.GetElement(1).length(); ++i)
		{
			if(m_macro.GetElement(1)[i] == '\"') 
			{
				bInQuote = !bInQuote;
			}
			else if(bInQuote)
			{
				include += m_macro.GetElement(1)[i];
			}
		}

		m_macro.Clear();
		m_sectionType = SECTION_TYPE_ROOT;
		
		std::wstring wInclude(include.begin(), include.end());
		Build(wInclude.c_str());
	}
};
