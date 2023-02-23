//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CShaderFile.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSHADERFILE_H
#define CSHADERFILE_H

#include "CShaderFileBlock.h"
#include <Utilities/CDetectComment.h>
#include <Utilities/CCompilerUtil.h>
#include <Globals/CGlobals.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <vector>

namespace Graphics
{
	class CShaderFile
	{
	public:
		struct Data
		{
			std::wstring filename;
		};

	private:
		enum SECTION_TYPE
		{
			SECTION_TYPE_ROOT,
			SECTION_TYPE_PROPERTY,
			SECTION_TYPE_BLOCK,
			SECTION_TYPE_MACRO,
		};

		struct MACRO_CONDITION
		{
			bool bInverse;
			u64 bit;
		};

	public:
		CShaderFile();
		~CShaderFile();
		CShaderFile(const CShaderFile&) = delete;
		CShaderFile(CShaderFile&&) = delete;
		CShaderFile& operator = (const CShaderFile&) = delete;
		CShaderFile& operator = (CShaderFile&&) = delete;

		void ParseMacros();
		void ParseShader(class CShader* pShader);

		// Accessors.
		inline const Data& GetData() const { return m_data; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetContents(std::string&& str) { m_contents.Clear(); m_contents.AddString(std::move(str)); }

	private:
		void RemoveComments(char ch);
		void PopulateMacroMap();
		void ProcessMacros(const std::string& contents);
		bool ProcessMacroExpression(const std::string& contents);
		void ParseShader(char ch);

		bool ProcessRoot(char ch);
		bool ProcessProperty(char ch);
		bool ProcessBlock(char ch);
		std::pair<bool, std::string> ProcessInclude(char ch);
		
		// Accessors.
		inline bool Included() const { return m_macroStack.empty() || m_macroStack.top(); }

	public:
		std::string IncludeFile(const std::string& filename);
		
		bool ProcessMacroDefine(const std::vector<std::string>& wordList);
		bool ProcessMacroUndefine(const std::vector<std::string>& wordList);
		bool EvaluateMacroExpression(const std::vector<std::string>& wordList);
		bool ProcessMacroIf(const std::vector<std::string>& wordList);
		bool ProcessMacroElif(const std::vector<std::string>& wordList);
		bool ProcessMacroElse(const std::vector<std::string>& wordList);
		bool ProcessMacroIfdef(const std::vector<std::string>& wordList);
		bool ProcessMacroIfndef(const std::vector<std::string>& wordList);
		bool ProcessMacroEndif(const std::vector<std::string>& wordList);

	private:
		bool m_bInContainer;

		SECTION_TYPE m_sectionType;

		Util::CDetectComment<std::string, char> m_detectComment;
		Util::CompilerWord<std::string, char> m_word;
		Util::CompilerWord<std::string, char> m_contents;
		Util::CompilerWord<std::string, char> m_propertyBlock;
		Util::CompilerWord<std::string, char> m_hlslBlock;

		CShaderFileBlock m_blockProcessor;
		Util::CompilerTuple<2, std::string, char> m_property;
		Util::CompilerTuple<2, std::string, char> m_macro;
		std::stack<bool> m_macroStack;

		Data m_data;

		std::unordered_set<std::string> m_includeSet;
		std::unordered_map<std::string, std::string> m_macroMap;

		class CShader* m_pTargetShader;
	};
};

#endif
