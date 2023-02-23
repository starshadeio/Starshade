//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CShaderCompiler.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CShaderCompiler.h"
#include <Utilities/CDebugError.h>
#include <fstream>

namespace Graphics
{
	CShaderCompiler::CShaderCompiler() { }
	CShaderCompiler::~CShaderCompiler() { }

	// Method for saving an error message stored in pData.
	void CShaderCompiler::SaveErrorMessage(const wchar_t* logFilename, void* pData, size_t sz)
	{
		std::ofstream shaderErrorFile(logFilename);

		ASSERT_R(shaderErrorFile.is_open());

		shaderErrorFile.write((char*)pData, sz);
		shaderErrorFile.close();
	}
};
