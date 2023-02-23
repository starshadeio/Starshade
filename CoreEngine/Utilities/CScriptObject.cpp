//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CScriptObject.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CScriptObject.h"
#include "CDebugError.h"
#include "CFileUtil.h"
#include "CTextUtil.h"
#include <fstream>
#include <unordered_set>

namespace Util
{
	static const std::unordered_set<wchar_t> WORD_BREAK_SET = {
		L'~',
		L'!',
		L'@',
		L'#',
		L'%',
		L'^',
		L'&',
		L'*',
		L'(',
		L')',
		L'-',
		L'=',
		L'+',
		L'[',
		L'{',
		L']',
		L'}',
		L'|',
		L':',
		L';',
		L'\'',
		L'\"',
		L',',
		L'<',
		//L'.',
		L'>',
		L'/',
		L'?',
	};

	static const CScriptObject::StateMap BASE_MAP = {
		{ L"PropBlock", [](CScriptObject* pObj) { return CreatePropBlock(pObj); } },
	};

	static const CScriptObject::StateMap PROP_BLOCK_SYMBOL_MAP = {
		{ L"{", [](CScriptObject* pObj) { return NextSubProp(pObj); } },
		{ L":", [](CScriptObject* pObj) { return NextVal(pObj); } },
		{ L";", [](CScriptObject* pObj) { return NextProp(pObj); } },
	};

	static const CScriptObject::StateMap PROP_BLOCK_BREAK_MAP = {
		{ L"}", [](CScriptObject* pObj) { return ClosePropBlock(pObj); } },
	};

	static const CScriptObject::State STATE_BASE(&BASE_MAP, nullptr);
	static const CScriptObject::State STATE_PROP_NAME(&PROP_BLOCK_BREAK_MAP, [](CScriptObject* pObj, const std::wstring& word) { return SetVal(pObj, word); });
	static const CScriptObject::State STATE_PROP_SYMBOLS(&PROP_BLOCK_SYMBOL_MAP, nullptr);

	//-----------------------------------------------------------------------------------------------
	// CScriptObject
	//-----------------------------------------------------------------------------------------------

	CScriptObject::CScriptObject() : 
		m_pPropBlock(nullptr),
		m_pState(&STATE_BASE)
	{
	}

	CScriptObject::~CScriptObject() { }

	// Remove comments and excess whitespace.
	std::wstring CScriptObject::FormatScript(const std::wstring& script)
	{
		CompilerWord<std::wstring, wchar_t> word;
		CDetectComment<std::wstring, wchar_t> detectComment;

		bool bInQuote = false;
		wchar_t lastCh = L'\0';
		bool bAwaitingPrintable = true;
		
		std::wstring output;
		
		for(size_t i = 0; i <= script.size(); ++i)
		{ // Remove comments.
			wchar_t ch = i < script.size() ? script[i] : -1;

			COMMENT_TYPE commentType = COMMENT_TYPE_NONE;
			if(i < script.size() && (bInQuote || (commentType = detectComment.InComment(ch, word)) == COMMENT_TYPE_NONE))
			{
				if(bInQuote && lastCh != L'\\' && ch == L'\"')
				{
					bInQuote = false;
					bAwaitingPrintable = false;
					word.AddChar(ch);
				}
				else
				{
					if(!bAwaitingPrintable || ch > 0x20)
					{
						if(ch == L'\n')
						{
							bAwaitingPrintable = true;
						}
						else
						{
							bAwaitingPrintable = !bInQuote && (ch <= 0x20);
						}

						word.AddChar(ch);
					}

					if(!bInQuote && ch == L'\"') { bInQuote = true; }
				}
			}
			else
			{
				std::wstring str;
				word.Move(str);
				if(str.empty())
				{
					bAwaitingPrintable = true;
				}
				else
				{
					output += str;
				}
			}

			lastCh = ch;
		}

		return output;
	}

	void CScriptObject::Build()
	{
#ifndef PRODUCTION_BUILD
		m_code = FormatScript(ReadFileUTF8((m_data.filename).c_str()));
#else
		m_code = ReadFileUTF8((m_data.filename).c_str());
#endif

		// Parse code.
		bool bInQuote = false;
		wchar_t lastCh = L'\0';
		for(wchar_t ch : m_code)
		{
			if(!bInQuote && ch == L'\"')
			{
				bInQuote = true;
			}
			else if(bInQuote && lastCh != L'\\' && ch == L'\"')
			{
				bInQuote = false;

			}
			else if(!bInQuote && (ch <= 0x20 || WORD_BREAK_SET.find(ch) != WORD_BREAK_SET.end()))
			{
				// End of word.
				std::wstring str = L"";
				if(!m_word.Empty())
				{
					m_word.Move(str);
					ASSERT_R(m_pState->ProccessWord(this, str));
				}
				
				if(ch > 0x20)
				{ // Process symbol.
					str = ch;
					ASSERT_R(m_pState->ProccessWord(this, str));
				}

			}
			else if(ch > 0x20 || bInQuote)
			{
				m_word.AddChar(ch);
			}

			lastCh = ch;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// PropBlock methods.
	//-----------------------------------------------------------------------------------------------

	bool CreatePropBlock(CScriptObject* pObj)
	{
		pObj->m_propBlockList.push_back(CScriptObject::PropBlock());
		pObj->m_pPropBlock = &pObj->m_propBlockList.back();
		pObj->SetAwaitingValue(false);

		NextVal(pObj);

		return true;
	}

	bool SetVal(CScriptObject* pObj, const std::wstring& word)
	{
		if(pObj->IsAwaitingValue())
		{
			pObj->m_pPropBlock->children.push_back(CScriptObject::PropBlock());
			pObj->m_pPropBlock->children.back().pParent = pObj->m_pPropBlock;
			pObj->m_pPropBlock = &pObj->m_pPropBlock->children.back();

			pObj->m_pPropBlock->propList.push_back(L"");

			pObj->SetAwaitingValue(false);
		}
		else
		{
			pObj->m_pPropBlock->propList.push_back(L"");
		}

		pObj->m_pPropBlock->propList.back() = std::wstring(word.begin(), word.end());

		pObj->m_pState = &STATE_PROP_SYMBOLS;

		return true;
	}

	bool NextVal(CScriptObject* pObj)
	{
		pObj->m_pState = &STATE_PROP_NAME;

		return true;
	}

	bool NextProp(CScriptObject* pObj)
	{
		pObj->m_pPropBlock = pObj->m_pPropBlock->pParent;
		NextSubProp(pObj);

		return true;
	}

	bool NextSubProp(CScriptObject* pObj)
	{
		pObj->SetAwaitingValue(true);
		NextVal(pObj);

		return true;
	}

	bool ClosePropBlock(CScriptObject* pObj)
	{
		pObj->m_pPropBlock = pObj->m_pPropBlock->pParent;

		if(pObj->m_pPropBlock == nullptr)
		{
			pObj->m_pState = &STATE_BASE;
		}
		else
		{
			NextSubProp(pObj);
		}

		return true;
	}
};
