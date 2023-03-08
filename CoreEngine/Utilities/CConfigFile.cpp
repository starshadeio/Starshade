//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CConfigFile.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CConfigFile.h"
#include "CFileUtil.h"

namespace Util
{
	CConfigFile::CConfigFile()
	{
	}

	CConfigFile::~CConfigFile()
	{
	}

	bool CConfigFile::Parse()
	{
		m_bInQuote = false;
		m_sectionType = SECTION_TYPE_ROOT;

		bool bAwaitingPrintable = true;

		std::wstring input = ReadFileUTF8((m_data.filename).c_str());
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
			}
		} Parse('\n');

		return true;
	}
	
	void CConfigFile::Parse(wchar_t ch)
	{
		switch(m_sectionType)
		{
			case SECTION_TYPE_ROOT:
				ProcessRoot(ch);
				break;
			case SECTION_TYPE_PROPERTY:
				ProcessProperty(ch);
				break;
			case SECTION_TYPE_BLOCK:
				ProcessBlock(ch);
				break;
			case SECTION_TYPE_SUBBLOCK:
				ProcessSubBlock(ch);
				break;
			default:
				break;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Processing methods.
	//-----------------------------------------------------------------------------------------------

	void CConfigFile::ProcessRoot(wchar_t ch)
	{
		if(ch > 0x20)
		{
			m_sectionType = SECTION_TYPE_PROPERTY;
			Parse(ch);
		}
	}

	void CConfigFile::ProcessProperty(wchar_t ch)
	{
		if(ch != ':')
		{
			if(ch > 0x20)
			{
				m_property += ch;
			}
		}
		else
		{
			m_sectionType = SECTION_TYPE_BLOCK;
		}
	}

	void CConfigFile::ProcessBlock(wchar_t ch)
	{
		if(ch == '{')
		{
			m_bSingleSubBlock = false;
			m_sectionType = SECTION_TYPE_SUBBLOCK;
		}
		else if(ch > 0x20)
		{
			m_bSingleSubBlock = true;
			m_sectionType = SECTION_TYPE_SUBBLOCK;
		}
		else if(ch == '\n')
		{ // Process config property.
			m_data.processConfig(m_property, m_propertyElemList);
			m_property.clear();
			m_propertyElemList.clear();
			m_sectionType = SECTION_TYPE_ROOT;
		}
	}

	void CConfigFile::ProcessSubBlock(wchar_t ch)
	{
		bool bSubBlockComplete = false;
		if(m_bSingleSubBlock)
		{
			if(ch > 0x20)
			{
				m_elem += ch;
			}
			else
			{
				bSubBlockComplete = true;
			}
		}
		else
		{
			if(ch == '}')
			{
				bSubBlockComplete = true;
			}
			else if(ch == ',')
			{
				if(!m_elem.empty())
				{
					m_elemList.push_back(std::move(m_elem));
				}
			}
			else if(ch > 0x20)
			{
				m_elem += ch;
			}
		}

		if(bSubBlockComplete)
		{
			if(!m_elem.empty())
			{
				m_elemList.push_back(std::move(m_elem));
			}

			if(!m_elemList.empty())
			{
				m_propertyElemList.push_back(std::move(m_elemList));
			}

			m_sectionType = SECTION_TYPE_BLOCK;
			if(ch == '\n')
			{
				Parse(ch);
			}
		}
	}
};
