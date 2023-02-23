//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMaterialFile.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATERIALFILE_H
#define CMATERIALFILE_H

#include <Utilities/CDetectComment.h>
#include <Utilities/CCompilerUtil.h>
#include <string>

namespace Graphics
{
	class CMaterialFile
	{
	public:
		struct Data
		{
			class CMaterial* pMaterial;
		};

	public:
		CMaterialFile();
		~CMaterialFile();
		CMaterialFile(const CMaterialFile&) = delete;
		CMaterialFile(CMaterialFile&&) = delete;
		CMaterialFile& operator = (const CMaterialFile&) = delete;
		CMaterialFile& operator = (CMaterialFile&&) = delete;

		void ParseFile(const std::wstring& filename);

		// Accessors.
		inline const std::string& GetCode() const { return m_code; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetCode(std::string&& code) { m_code = std::move(code); }

	private:
		void Parse(char ch);
		void ProcessPair();

	private:
		bool m_bInContainer;

		Data m_data;
		std::string m_code;

		Util::CDetectComment<std::string, char> m_detectComment;
		Util::CompilerWord<std::string, char> m_word;
		Util::CompilerTuple<2, std::string, char> m_pair;
	};
};

#endif
