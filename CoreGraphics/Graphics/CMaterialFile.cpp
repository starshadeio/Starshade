//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMaterialFile.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMaterialFile.h"
#include "CMaterial.h"
#include "CShaderTrie.h"
#include "CShader.h"
#include "../Resources/CResourceManager.h"
#include <Math/CMathFNV.h>
#include <Utilities/CDebugError.h>
#include <fstream>
#include <cassert>
#include <algorithm>

namespace Graphics
{
	CMaterialFile::CMaterialFile() : m_bInContainer(false) { }
	CMaterialFile::~CMaterialFile() { }

	void CMaterialFile::ParseFile(const std::wstring& filename)
	{
#ifndef PRODUCTION_BUILD
		std::ifstream input(filename);
		assert(input.is_open());

		m_code.clear();
		bool bAwaitingPrintable = true;

		char ch;
		while((ch = input.get()) != -1)
		{
			Util::COMMENT_TYPE commentType;
			if(commentType = m_detectComment.InComment(ch, m_word)) 
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


		} Parse('\n'); // Parse an implicit new line to finalize any sections that are being processed.

		input.close();
#else
		for(char ch : m_code)
		{
			Parse(ch);
		} Parse('\n'); // Parse an implicit new line to finalize any sections that are being processed.
#endif
	}

	void CMaterialFile::Parse(char ch)
	{
		if(ch == '(')
		{
			assert(!m_bInContainer);
			m_bInContainer = true;
		}
		else if(ch == ')')
		{
			assert(m_bInContainer);
			m_bInContainer = false;
		}
		else
		{
			if(ch > 0x20 || m_bInContainer)
			{
				m_word.AddChar(ch);
			}
			else if(!m_word.Empty())
			{
				if(ch == '\n')
				{
					// Try to finalize pair.
					m_pair.SetTarget(m_word);
					ASSERT(m_pair.IsComplete());
					ProcessPair();
					m_pair.Clear();
				}
				else
				{
					// Try to move to the next section of pair.
					m_pair.SetTarget(m_word);
					ASSERT(m_pair.Continue());
				}
			}
		}
	}

	void CMaterialFile::ProcessPair()
	{
		if(m_pair.GetElement(0) == "shader")
		{
			// The first part of the pair value is the base shader key, followed by zero or more macros for it.
			int index = 0;
			for(; index < m_pair.GetElement(1).size(); ++index)
			{
				if(m_pair.GetElement(1)[index] == '.')
				{
					break;
				}
			}

			CShaderTrie* shaderTrie = reinterpret_cast<CShaderTrie*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_SHADER, m_pair.GetElement(1).substr(0, index)));
			CShaderTrie::Node* trieNode = nullptr;
			assert(shaderTrie);

			std::vector<std::string> macroList;
			macroList.push_back("");
			for(; index <= m_pair.GetElement(1).size(); ++index)
			{
				if(index == m_pair.GetElement(1).size() || m_pair.GetElement(1)[index] == '.')
				{
					macroList.push_back("");
				}
				else
				{
					macroList.back() += m_pair.GetElement(1)[index];
				}
			}

			macroList.pop_back();
			std::sort(macroList.begin(), macroList.end());
			for(size_t i = 0; i < macroList.size(); ++i)
			{
				trieNode = shaderTrie->Insert(macroList[i], i == macroList.size() - 1, trieNode);
			}

			if(trieNode)
			{
				m_data.pMaterial->BuildBuffers(trieNode->pShader);
			}
		}
		else
		{
			std::string bufferPair[2];
			for(size_t i = 0, j = 0, index = 0; i <= m_pair.GetElement(0).size() && index < 2; ++i)
			{
				if(i == m_pair.GetElement(0).size() || m_pair.GetElement(0)[i] == '.')
				{
					bufferPair[index++] = m_pair.GetElement(0).substr(j, i);
					j = i + 1;
				}
			}

			m_data.pMaterial->SetValue(Math::FNV1a_32(bufferPair[0].c_str(), bufferPair[0].size()),
				Math::FNV1a_32(bufferPair[1].c_str(), bufferPair[1].size()), m_pair.GetElement(1));
		}
	}
};
