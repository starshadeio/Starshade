//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CShaderFileProp.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSHADERFILEPROP_H
#define CSHADERFILEPROP_H

#include "CGraphicsData.h"
#include <Utilities/CCompilerUtil.h>
#include <string>

namespace Graphics
{
	class CShaderFileProp
	{
	public:
		static bool ProcessProperty(const Util::CompilerTuple<2, std::string, char>& prop, class CShader* pShader);
	};
};

#endif