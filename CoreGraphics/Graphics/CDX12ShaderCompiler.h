//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12ShaderCompiler.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDX12SHADERCOMPILER_H
#define CDX12SHADERCOMPILER_H

#include "CShaderCompiler.h"
#include <string>

#pragma comment(lib, "d3dcompiler.lib")

namespace Graphics
{
	class CDX12ShaderCompiler : public CShaderCompiler
	{
	public:
		CDX12ShaderCompiler();
		~CDX12ShaderCompiler();
		CDX12ShaderCompiler(const CDX12ShaderCompiler&) = delete;
		CDX12ShaderCompiler(CDX12ShaderCompiler&&) = delete;
		CDX12ShaderCompiler& operator = (const CDX12ShaderCompiler&) = delete;
		CDX12ShaderCompiler& operator = (CDX12ShaderCompiler&&) = delete;

		void CompileFromFile(const wchar_t* filename, const char* entryPoint, const ShaderMacro* pMacroList,
			SHADER_TYPE type, SHADER_FEATURE_LEVEL featureLevel, ByteCode& byteCode, const wchar_t* logFilename) final;

		void CompileFromData(const char* srcData, size_t srcDataSize, const char* entryPoint, const ShaderMacro* pMacroList,
			SHADER_TYPE type, SHADER_FEATURE_LEVEL featureLevel, ByteCode& byteCode, const wchar_t* logFilename) final;

	private:
		std::string GenerateTargetString(SHADER_TYPE shaderType, SHADER_FEATURE_LEVEL featureLevel);
	};
};

#endif
