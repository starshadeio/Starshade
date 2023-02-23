//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12ShaderCompiler.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef GRAPHICS_DX12

#include "CDX12ShaderCompiler.h"
#include <Globals/CGlobals.h>
#include <Utilities/CDebugError.h>
#include <d3dcompiler.h>
#include <wrl.h>

namespace Graphics
{
	using Microsoft::WRL::ComPtr;

#ifdef _DEBUG
	static const u32 compileFlags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	static const u32 compileFlags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	CDX12ShaderCompiler::CDX12ShaderCompiler() { }
	CDX12ShaderCompiler::~CDX12ShaderCompiler() { }

	// Method for compiling a shader from a file.
	void CDX12ShaderCompiler::CompileFromFile(const wchar_t* filename, const char* entryPoint, const ShaderMacro* pMacroList,
		SHADER_TYPE type, SHADER_FEATURE_LEVEL featureLevel, ByteCode& byteCode, const wchar_t* logFilename)
	{

		ComPtr<ID3DBlob> pShader;
		ComPtr<ID3DBlob> pShaderError;

		if(FAILED(D3DCompileFromFile(filename, reinterpret_cast<const D3D_SHADER_MACRO*>(pMacroList), D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint, GenerateTargetString(type, featureLevel).c_str(), compileFlags, 0, &pShader, &pShaderError)))
		{

			ASSERT_R(pShaderError != nullptr);

			if(logFilename)
			{
				SaveErrorMessage(logFilename, pShaderError->GetBufferPointer(), pShaderError->GetBufferSize());
			}

			ASSERT_R(pShaderError == nullptr);
		}

		ASSERT_R(pShader != nullptr);

		byteCode.tSize = pShader->GetBufferSize();
		byteCode.pData = new char[byteCode.tSize];
		memcpy(byteCode.pData, pShader->GetBufferPointer(), pShader->GetBufferSize());
	}

	// Method for compiling a shader from source data.
	void CDX12ShaderCompiler::CompileFromData(const char* srcData, size_t srcDataSize, const char* entryPoint, const ShaderMacro* pMacroList,
		SHADER_TYPE type, SHADER_FEATURE_LEVEL featureLevel, ByteCode& byteCode, const wchar_t* logFilename)
	{

		ComPtr<ID3DBlob> pShader;
		ComPtr<ID3DBlob> pShaderError;

		if(FAILED(D3DCompile(srcData, srcDataSize, nullptr, reinterpret_cast<const D3D_SHADER_MACRO*>(pMacroList), D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint, GenerateTargetString(type, featureLevel).c_str(), compileFlags, 0, &pShader, &pShaderError)))
		{

			ASSERT_R(pShaderError != nullptr);

			if(logFilename)
			{
				SaveErrorMessage(logFilename, pShaderError->GetBufferPointer(), pShaderError->GetBufferSize());
			}

			ASSERT_R(pShaderError == nullptr);
		}

		ASSERT_R(pShader != nullptr);

		byteCode.tSize = pShader->GetBufferSize();
		byteCode.pData = new char[byteCode.tSize];
		memcpy(byteCode.pData, pShader->GetBufferPointer(), pShader->GetBufferSize());
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	std::string CDX12ShaderCompiler::GenerateTargetString(SHADER_TYPE shaderType, SHADER_FEATURE_LEVEL featureLevel)
	{
		std::string target;

		switch(shaderType)
		{
			case Graphics::SHADER_TYPE_VS:
				target = "vs_";
				break;
			case Graphics::SHADER_TYPE_HS:
				target = "hs_";
				break;
			case Graphics::SHADER_TYPE_DS:
				target = "ds_";
				break;
			case Graphics::SHADER_TYPE_GS:
				target = "gs_";
				break;
			case Graphics::SHADER_TYPE_PS:
				target = "ps_";
				break;
			case Graphics::SHADER_TYPE_CS:
				target = "cs_";
				break;
			default:
				break;
		}

		switch(featureLevel)
		{
			case Graphics::SHADER_FEATURE_LEVEL_1_1:
				target += "1_1";
				break;
			case Graphics::SHADER_FEATURE_LEVEL_2_0:
				target += "2_0";
				break;
			case Graphics::SHADER_FEATURE_LEVEL_3_0:
				target += "3_0";
				break;
			case Graphics::SHADER_FEATURE_LEVEL_4_0:
				target += "4_0";
				break;
			case Graphics::SHADER_FEATURE_LEVEL_4_1:
				target += "4_1";
				break;
			case Graphics::SHADER_FEATURE_LEVEL_5_0:
				target += "5_0";
				break;
			case Graphics::SHADER_FEATURE_LEVEL_5_1:
				target += "5_1";
				break;
			case Graphics::SHADER_FEATURE_LEVEL_6_0:
				target += "6_0";
				break;
			default:
				break;
		}

		return target;
	}
};

#endif
