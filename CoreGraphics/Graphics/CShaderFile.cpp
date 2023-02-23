//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CShaderFile.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CShaderFile.h"
#include "CShader.h"
#include "CShaderFileProp.h"
#include <Utilities/CDebugError.h>
#include <Utilities/CConvertUtil.h>
#include <fstream>
#include <cassert>
#include <unordered_map>
#include <functional>
#include <queue>

namespace Graphics
{
	static const std::unordered_map<std::string, std::function<bool(CShaderFile*, const std::vector<std::string>&)>> MACRO_MAP = {
		{ "#define", [](CShaderFile* pFile, const std::vector<std::string>& wordList){ return pFile->ProcessMacroDefine(wordList); } },
		{ "#undef", [](CShaderFile* pFile, const std::vector<std::string>& wordList){ return pFile->ProcessMacroUndefine(wordList); } },
		{ "#if", [](CShaderFile* pFile, const std::vector<std::string>& wordList){ return pFile->ProcessMacroIf(wordList); } },
		{ "#elif", [](CShaderFile* pFile, const std::vector<std::string>& wordList){ return pFile->ProcessMacroElif(wordList); } },
		{ "#else", [](CShaderFile* pFile, const std::vector<std::string>& wordList){ return pFile->ProcessMacroElse(wordList); } },
		{ "#ifdef", [](CShaderFile* pFile, const std::vector<std::string>& wordList){ return pFile->ProcessMacroIfdef(wordList); } },
		{ "#ifndef", [](CShaderFile* pFile, const std::vector<std::string>& wordList){ return pFile->ProcessMacroIfndef(wordList); } },
		{ "#endif", [](CShaderFile* pFile, const std::vector<std::string>& wordList){ return pFile->ProcessMacroEndif(wordList); } },
	};
	
	CShaderFile::CShaderFile() :
		m_bInContainer(false),
		m_sectionType(SECTION_TYPE_ROOT),
		m_pTargetShader(nullptr)
	{ }

	CShaderFile::~CShaderFile()
	{ }

	void CShaderFile::ParseMacros()
	{ }

	void CShaderFile::ParseShader(CShader* pShader)
	{
		m_pTargetShader = pShader;

#ifndef PRODUCTION_BUILD
		std::ifstream input(m_data.filename);
		assert(input.is_open());

		std::string contents = "";
		m_contents.Clear();
		m_includeSet.clear();

		{ // Initial processing pass of shader file. Expands any include files that might be had.
			std::stack<std::string> contentsStack;
			std::stack<size_t> contentIndexStack;
			bool bEmptyLine = true;

			while(!input.eof())
			{
				char ch;

				if(contentsStack.empty())
				{
					ch = input.get();
				}
				else
				{
					ch = contentsStack.top()[contentIndexStack.top()];
					if(++contentIndexStack.top() >= contentsStack.top().size())
					{
						contentsStack.pop();
						contentIndexStack.pop();
					}
				}

				if(m_sectionType != SECTION_TYPE_MACRO)
				{
					// Macro entrance check.
					if(bEmptyLine && ch == '#')
					{
						m_sectionType = SECTION_TYPE_MACRO;
					}
					else
					{
						if(ch == '\n')
						{
							bEmptyLine = true;
						}
						else if(ch > 0x20)
						{
							bEmptyLine = false;
						}

						contents += ch;
					}
				}
				else
				{
					auto res = ProcessInclude(ch);
					if(res.first)
					{
						contentsStack.push(res.second);
						contentIndexStack.push(0);
					}
					else
					{
						contents += res.second;
					}
				}
			};
		}

		input.close();

		// Let's first get rid of any comments in the shader file's contents.
		char lastCh = '\0';
		for(char ch : contents)
		{
			if(ch == -1) { break; }
			if(ch <= 0x20)
			{
				if(ch != '\n')
				{
					ch = ' ';
				}
				if(lastCh == ch) { continue; }
			}

			lastCh = ch;
			RemoveComments(ch);
		} if(lastCh != '\n')
		{  // Parse an implicit new line to finalize any sections that are being processed.
			RemoveComments('\n');
		}

		// Next we'll process macros.
		PopulateMacroMap();

		std::string fullFile;
		m_contents.Move(fullFile);
		ProcessMacros(fullFile);

#endif
		// We should now have only valid code in 'm_contents.'
		m_contents.ForEach([this](char ch) {
			ParseShader(ch);
		}); ParseShader('\n');

		// Trim leading and trailing whitespace from both code blocks.
		m_propertyBlock.Trim();
		m_hlslBlock.Trim();

		m_pTargetShader->SetProperies(m_propertyBlock);
		m_pTargetShader->SetHLSL(m_hlslBlock);

		m_pTargetShader = nullptr;
	}

	void CShaderFile::PopulateMacroMap()
	{
		m_macroMap.clear();
		for(const std::string& exp : m_pTargetShader->GetMacroList())
		{
			if(exp.empty()) { continue; }

			// macro(value)
			std::string macro;
			std::string value;
			size_t index = 0;
			for(; index < exp.size() && exp[index] != '('; ++index)
			{
				macro.push_back(exp[index]);
			}

			for(index = index + 1; index < exp.size() && exp[index] != ')'; ++index)
			{
				value.push_back(exp[index]);
			}

			m_macroMap.insert({ macro, value });
		}
	}

	// Method for going through a shader file and evaluating, removing or replacing macros.
	void CShaderFile::ProcessMacros(const std::string& contents)
	{
		std::string word = "";
		bool bNewLine = true;
		bool bInMacroExp = false;
		bool bLastSpace = false;
		char lastMacroCh = '\0';
		for(size_t i = 0; i <= contents.size(); ++i)
		{
			if(bInMacroExp)
			{
				if(i == contents.size() || (contents[i] == '\n' && (!bInMacroExp || lastMacroCh != '\\')))
				{
					// Process macro contents.
					ProcessMacroExpression(word);
					
					word.clear();
					bInMacroExp = false;
					lastMacroCh = '\0';
					bNewLine = true;
				}
				else
				{
					// Gather macro line.
					word += contents[i];
					lastMacroCh = contents[i];
				}
			}
			else if(i < contents.size())
			{
				if(bNewLine && contents[i] == '#')
				{
					word = contents[i];

					// Start of macro.
					bInMacroExp = true;
				}
				else if(Included())
				{
					if(contents[i] <= 0x20 || !(isalpha(contents[i]) || contents[i] == '_' || (!word.empty() && isdigit(contents[i]))))
					{
						// End of word.
						if(!word.empty())
						{
							// Test of word is a macro define. If it is replace word with its value.
							auto elem = m_macroMap.find(word);
							if(elem != m_macroMap.end())
							{
								word = elem->second;
							}

							m_contents.AddString(word);
							word.clear();
						}

						if(!bNewLine || contents[i] > 0x20)
						{
							m_contents.AddChar(contents[i]);
						}
					}
					else
					{
						word += contents[i];
					}
				}
				
				// Update new line flag.
				if(contents[i] > 0x20)
				{
					bNewLine = false;
				}
				else if(contents[i] == '\n')
				{
					bNewLine = true;
				}
			}
		}
	}
	
	bool CShaderFile::ProcessMacroExpression(const std::string& contents)
	{
		std::vector<std::string> wordList;
		wordList.push_back("");
		for(size_t i = 0; i <= contents.size(); ++i)
		{
			if(i == contents.size() || contents[i] <= 0x20)
			{
				if(wordList.back().size())
				{
					wordList.push_back("");
				}
			}
			else
			{
				wordList.back() += contents[i];
			}
		}

		wordList.pop_back();

		// Use first word to call proper processor.
		auto elem = MACRO_MAP.find(wordList[0]);
		if(elem != MACRO_MAP.end())
		{
			return elem->second(this, wordList);
		}

		return false;
	}

	void CShaderFile::RemoveComments(char ch)
	{
		if(m_detectComment.InComment(ch, m_contents) == 0)
		{
			m_contents.AddChar(ch);
		}
	}

	void CShaderFile::ParseShader(char ch)
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
			default:
				break;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Section processors.
	//-----------------------------------------------------------------------------------------------

	bool CShaderFile::ProcessRoot(char ch)
	{
		if(ch == '$')
		{
			m_sectionType = SECTION_TYPE_PROPERTY;
		}
		else
		{
			if(ch > 0x20)
			{
				m_word.AddChar(ch);
			}
			else
			{
				std::string str;
				m_word.Move(str);
				if(m_blockProcessor.TryInitialize(str))
				{
					// CBuffer check.
					m_sectionType = SECTION_TYPE_BLOCK;
				}
			}

			m_hlslBlock.AddChar(ch);
		}

		return m_sectionType == SECTION_TYPE_ROOT;
	}

	bool CShaderFile::ProcessProperty(char ch)
	{
		if(ch == '$')
		{
			m_sectionType = SECTION_TYPE_ROOT;
		}
		else
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
			else if(m_bInContainer || ch > 0x20)
			{
				m_word.AddChar(ch);
			}
			else
			{
				if(!m_word.Empty())
				{
					m_property.SetTarget(m_word);
					if(m_property.IsComplete())
					{
						// Collected property! Noew process it.
						CShaderFileProp::ProcessProperty(m_property, m_pTargetShader);
						m_property.Clear();
					}
					else
					{
						ASSERT(m_property.Continue());
					}
				}
			}

			m_propertyBlock.AddChar(ch);
		}

		return m_sectionType == SECTION_TYPE_PROPERTY;
	}

	bool CShaderFile::ProcessBlock(char ch)
	{
		if(!m_blockProcessor.ProcessCharacter(ch))
		{
			m_blockProcessor.Finalize(m_pTargetShader);
			m_blockProcessor.Clear();
			m_sectionType = SECTION_TYPE_ROOT;
		}

		m_hlslBlock.AddChar(ch);

		return m_sectionType == SECTION_TYPE_BLOCK;
	}

	std::pair<bool, std::string> CShaderFile::ProcessInclude(char ch)
	{
		static std::string line = "#";
		line.push_back(ch);

		std::pair<bool, std::string> retVal = { false, "" };

		if(ch == '\n')
		{
			// Process contents of macro based on its keyword.
			m_macro.SetTarget(m_word);

			if(m_macro.GetElement(0) == "include")
			{
				retVal = { true, IncludeFile(m_macro.GetElement(1)) };
			}
			else
			{
				retVal = { false, line };
			}

			line.resize(1, '#');
			m_macro.Clear();
			m_sectionType = SECTION_TYPE_ROOT;
		}

		else if(ch > 0x20)
		{
			m_word.AddChar(ch);
		}

		else
		{
			if(m_macro.GetElement(0).length() == 0)
			{
				m_macro.SetTarget(m_word);
			}

			if(m_macro.Continue())
			{
				assert(m_macro.GetElement(0).length() > 0);
			}
		}

		return retVal;
	}

	//-----------------------------------------------------------------------------------------------
	// Macro methods.
	//-----------------------------------------------------------------------------------------------

	// Include file.
	std::string CShaderFile::IncludeFile(const std::string& filename)
	{
		if(!m_includeSet.insert(filename).second) { return ""; }

		std::wstring path = m_data.filename;
		while(!path.empty() && path.back() != '/')
		{
			path.pop_back();
		}

		for(size_t i = 1; i < filename.size() - 1; ++i)
		{
			path += filename[i];
		}

		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if(!file.is_open()) { return ""; }

		size_t len = file.tellg();
		std::string contents(len, '\0');
		file.seekg(0, std::ios::beg);
		file.read(contents.data(), len);
		file.close();

		return contents;
	}
	
	bool CShaderFile::ProcessMacroDefine(const std::vector<std::string>& wordList)
	{
		if(!Included()) { return false; }
		
		std::string macro = "";
		std::string contents = "";
		bool bMethod = false;

		for(size_t i = 0; i < wordList[1].size(); ++i)
		{
			if(!bMethod && wordList[1][i] != '(')
			{
				macro += wordList[1][i];
			}
			else
			{
				contents += wordList[1][i];
				bMethod = true;
			}
		}

		if(bMethod)
		{
			contents += ' ';
		}

		for(size_t i = 2; i < wordList.size(); ++i)
		{
			contents += wordList[i];
			contents.push_back(' ');
		}

		if(!contents.empty())
		{
			contents.pop_back();
		}
		
		if(bMethod)
		{
			m_contents.AddString(wordList[0] + " " + macro + contents + '\n');
		}
		else
		{
			m_macroMap.insert({ macro, contents });
		}

		return true;
	}
	
	bool CShaderFile::ProcessMacroUndefine(const std::vector<std::string>& wordList)
	{
		if(!Included()) { return false; }
		m_macroMap.erase(wordList[1]);

		return true;
	}

	bool CShaderFile::EvaluateMacroExpression(const std::vector<std::string>& wordList)
	{
		enum class TYPE
		{
			PAREN_OPEN,
			PAREN_CLOSE,

			DEFINE,
			MACRO,
			NUMBER,

			LOGIC,
			CONDITIONAL,
		};
		static const std::unordered_map<std::string, TYPE> TYPE_MAP =
		{
			{ "(", TYPE::PAREN_OPEN },
			{ ")", TYPE::PAREN_CLOSE },

			{ "defined", TYPE::DEFINE },

			{ "!", TYPE::LOGIC},
			{ "||", TYPE::LOGIC},
			{ "&&", TYPE::LOGIC},

			{ "==", TYPE::CONDITIONAL},
			{ "!=", TYPE::CONDITIONAL},
			{ "<", TYPE::CONDITIONAL},
			{ "<=", TYPE::CONDITIONAL},
			{ ">", TYPE::CONDITIONAL},
			{ ">=", TYPE::CONDITIONAL},
		};
		

		auto IsWord = [](char ch, bool bFirst){
			return isalpha(ch) || ch == '_' || (!bFirst && isdigit(ch));
		};

		// Build full expression string.
		std::string exp = "";
		for(size_t i = 1; i < wordList.size(); ++i)
		{
			exp += wordList[i];
			exp.push_back(' ');
		}

		if(!exp.empty())
		{
			exp.pop_back();
		}

		std::queue<std::pair<TYPE, std::string>> expQueue;

		std::string word = "";
		for(size_t i = 0; i <= exp.size(); ++i)
		{
			if(i == exp.size() || exp[i] <= 0x20 || (!word.empty() && ((isdigit(word[0]) && !isdigit(exp[i])) || (!isdigit(word[0]) && (word[0] == '!' || (IsWord(word.front(), true) != IsWord(exp[i], false)))))))
			{
				auto elem = TYPE_MAP.find(word);
				if(elem != TYPE_MAP.end())
				{
					expQueue.push({ elem->second, word });
				}
				else
				{
					expQueue.push({ isdigit(word[0]) ? TYPE::NUMBER : TYPE::MACRO, word });
				}

				word.clear();
			}

			if(i < exp.size() && exp[i] > 0x20)
			{
				word += exp[i];
			}
		}
		
		struct Macro
		{
			bool bDefine;
			TYPE type;
			std::string str;

			double GetValue(CShaderFile* pFile) const
			{
				if(type == TYPE::MACRO)
				{
					return atof(pFile->m_macroMap.find(str)->second.c_str());
				}
				else if(type == TYPE::NUMBER)
				{
					return atof(str.c_str());
				}

				return 0.0f;
			}
		};

		enum class LOGIC
		{
			NOT,
			OR,
			AND,
		};
		
		static const std::unordered_map<std::string, LOGIC> LOGIC_MAP =
		{
			{ "!", LOGIC::NOT},
			{ "||", LOGIC::OR},
			{ "&&", LOGIC::AND},
		};
		

		enum class COND
		{
			EQUAL,
			NEQUAL,
			LESS,
			LEQUAL,
			GREATER,
			GEQUAL,
		};
		
		static const std::unordered_map<std::string, COND> COND_MAP =
		{
			{ "==", COND::EQUAL},
			{ "!=", COND::NEQUAL},
			{ "<", COND::LESS},
			{ "<=", COND::LEQUAL},
			{ ">", COND::GREATER},
			{ ">=", COND::GEQUAL},
		};
		

		std::stack<Macro> macroStack;
		std::stack<COND> condStack;
		std::stack<LOGIC> logicStack;
		std::stack<bool> booleanStack;
		logicStack.push(LOGIC::OR);
		booleanStack.push(false);

		auto EvalutateConditional = [&](const Macro& a, const Macro& b, COND cond){
			const double i = a.GetValue(this);
			const double j = b.GetValue(this);
			switch(cond)
			{
				case COND::EQUAL:
					return i == j;
				case COND::NEQUAL:
					return i != j;
				case COND::LESS:
					return i < j;
				case COND::LEQUAL:
					return i <= j;
				case COND::GREATER:
					return i > j;
				case COND::GEQUAL:
					return i >= j;
				default:
					return false;
			}
		};

		auto GatherMacro = [&](){
			Macro macro { };

			if(expQueue.front().first == TYPE::DEFINE)
			{
				expQueue.pop();
				assert(expQueue.front().first == TYPE::PAREN_OPEN);
				expQueue.pop();
				macro.bDefine = true;

				assert(expQueue.front().first == TYPE::MACRO);
				macro.type = expQueue.front().first;
				macro.str = expQueue.front().second;
				expQueue.pop();

				assert(expQueue.front().first == TYPE::PAREN_CLOSE);
				expQueue.pop();
			}
			else
			{
				assert(expQueue.front().first == TYPE::MACRO || expQueue.front().first == TYPE::NUMBER);
				macro.type = expQueue.front().first;
				macro.str = expQueue.front().second;
				expQueue.pop();
			}

			return macro;
		};

		auto ApplyEvaluation = [&](bool bEval){
			// Check if evaluation should be flipped.
			LOGIC logic = logicStack.top();
			logicStack.pop();
			if(logic == LOGIC::NOT)
			{
				bEval = !bEval;
				logic = logicStack.top();
				logicStack.pop();
			}
				
			// Apply evaluation.
			assert(logic != LOGIC::NOT);
			if(logic == LOGIC::AND)
			{
				booleanStack.top() &= bEval;
			}
			else if(logic == LOGIC::OR)
			{
				booleanStack.top() |= bEval;
			}
		};

		while(!expQueue.empty()) {
			if(expQueue.front().first == TYPE::PAREN_OPEN)
			{
				expQueue.pop();
				booleanStack.push(false);
				logicStack.push(LOGIC::OR);
			}
			else if(expQueue.front().first == TYPE::PAREN_CLOSE)
			{
				expQueue.pop();
				bool bEval = booleanStack.top();
				booleanStack.pop();
				ApplyEvaluation(bEval);
			}
			else if(expQueue.front().first == TYPE::LOGIC)
			{
				auto elem = LOGIC_MAP.find(expQueue.front().second);
				assert(elem != LOGIC_MAP.end());
				expQueue.pop();
				if(elem->second == LOGIC::AND && !booleanStack.top())
				{
					int layer = 1;
					while(layer && !expQueue.empty())
					{
						if(expQueue.front().first == TYPE::PAREN_OPEN)
						{
							++layer;
						}
						else if(expQueue.front().first == TYPE::PAREN_CLOSE)
						{
							--layer;
						}
						
						expQueue.pop();
					}

					// Apply evaluation.
					bool bEval = booleanStack.top();
					booleanStack.pop();
					if(booleanStack.empty())
					{
						booleanStack.push(bEval);
					}
					else
					{
						ApplyEvaluation(bEval);
					}
				}
				else
				{
					logicStack.push(elem->second);
				}
			}
			else
			{
				macroStack.push(GatherMacro());
				if(!expQueue.empty() && expQueue.front().first == TYPE::CONDITIONAL)
				{
					condStack.push(COND_MAP.find(expQueue.front().second)->second);
					expQueue.pop();
					macroStack.push(GatherMacro());
				}

				bool bEval = false;
				if(condStack.empty())
				{
					// Single macro expression.
					Macro a = macroStack.top();
					macroStack.pop();

					if(a.bDefine)
					{
						bEval = m_macroMap.find(a.str) != m_macroMap.end();
					}
					else
					{
						bEval = (a.GetValue(this) != 0);
					}
				}
				else
				{
					// Full conditional expression.
					Macro b = macroStack.top();
					macroStack.pop();
					Macro a = macroStack.top();
					macroStack.pop();
					bEval = EvalutateConditional(a, b, condStack.top());
					condStack.pop();
				}

				ApplyEvaluation(bEval);
			}
		}

		return booleanStack.top();
	}

	bool CShaderFile::ProcessMacroIf(const std::vector<std::string>& wordList)
	{
		if(!Included())
		{
			m_macroStack.push(false);
			return false;
		}
		
		// Evaluate expression.
		m_macroStack.push(EvaluateMacroExpression(wordList));

		return true;
	}

	bool CShaderFile::ProcessMacroElif(const std::vector<std::string>& wordList)
	{
		const bool bLast = m_macroStack.top();
		m_macroStack.pop();

		if(!Included() || bLast)
		{
			m_macroStack.push(false);
			return false;
		}

		// Evaluate expression.
		m_macroStack.push(EvaluateMacroExpression(wordList));

		return true;
	}

	bool CShaderFile::ProcessMacroElse(const std::vector<std::string>& wordList)
	{
		const bool bLast = m_macroStack.top();
		m_macroStack.pop();

		if(!Included() || bLast)
		{
			m_macroStack.push(false);
			return false;
		}

		m_macroStack.push(true);
		return true;
	}

	bool CShaderFile::ProcessMacroIfdef(const std::vector<std::string>& wordList)
	{
		if(!Included())
		{
			m_macroStack.push(false);
			return false;
		}

		m_macroStack.push(m_macroMap.find(wordList[1]) != m_macroMap.end());

		return true;
	}

	bool CShaderFile::ProcessMacroIfndef(const std::vector<std::string>& wordList)
	{
		if(!Included())
		{
			m_macroStack.push(false);
			return false;
		}

		m_macroStack.push(m_macroMap.find(wordList[1]) == m_macroMap.end());

		return true;
	}

	bool CShaderFile::ProcessMacroEndif(const std::vector<std::string>& wordList)
	{
		m_macroStack.pop();
		return true;
	}
};
