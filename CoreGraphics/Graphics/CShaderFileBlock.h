//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CShaderFileBlock.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSHADERFILEBLOCK_H
#define CSHADERFILEBLOCK_H

#include <Utilities/CCompilerUtil.h>
#include <Globals/CGlobals.h>
#include <vector>
#include <string>

namespace Graphics
{
	class CShaderFileBlock
	{
	public:
		enum BLOCK_TYPE
		{
			BLOCK_TYPE_UNKNOWN,
			BLOCK_TYPE_CBV,
			BLOCK_TYPE_UAV,
			BLOCK_TYPE_TEXTURE,
			BLOCK_TYPE_SAMPLER,
		};

	public:
		CShaderFileBlock();

		bool TryInitialize(const std::string& tp);
		bool ProcessCharacter(char ch);
		void Finalize(class CShader* pShader);
		void Clear();

	private:
		void FinalizeCBV(class CShader* pShader);
		void FinalizeUAV(class CShader* pShader);
		void FinalizeTexture(class CShader* pShader);
		void FinalizeSampler(class CShader* pShader);

	private:
		void GatherOptionalIndices(const std::string& str, std::function<void(int, int)> func);
		u32 ProcessVarTypeSize(const std::string& str);
		u32 ProcessVarName(const std::string& str, std::string& res);

	private:
		BLOCK_TYPE m_blockType;

		Util::CompilerTuple<3, std::string, char> m_root;
		std::vector<Util::CompilerTuple<3, std::string, char>> m_variables;

		Util::CompilerWord<std::string, char> m_word;
		Util::CompilerTuple<3, std::string, char>* m_pTarget;
	};
};

#endif
