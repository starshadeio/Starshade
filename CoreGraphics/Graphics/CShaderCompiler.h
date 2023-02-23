//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CShaderCompiler.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSHADERCOMPILER_H
#define CSHADERCOMPILER_H

#include "CGraphicsData.h"

namespace Graphics
{
	class CShaderCompiler
	{
	protected:
		CShaderCompiler();
		CShaderCompiler(const CShaderCompiler&) = delete;
		CShaderCompiler(CShaderCompiler&&) = delete;
		CShaderCompiler& operator = (const CShaderCompiler&) = delete;
		CShaderCompiler& operator = (CShaderCompiler&&) = delete;

	public:
		virtual ~CShaderCompiler();

		virtual void CompileFromFile(const wchar_t* filename, const char* entryPoint, const ShaderMacro* pMacroList,
			SHADER_TYPE type, SHADER_FEATURE_LEVEL featureLevel, ByteCode& byteCode, const wchar_t* logFilename) = 0;

		virtual void CompileFromData(const char* srcData, size_t srcDataSize, const char* entryPoint, const ShaderMacro* pMacroList,
			SHADER_TYPE type, SHADER_FEATURE_LEVEL featureLevel, ByteCode& byteCode, const wchar_t* logFilename) = 0;

	protected:
		void SaveErrorMessage(const wchar_t* logFilename, void* pData, size_t sz);
	};
};

#endif
