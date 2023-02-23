//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CResourceFile.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CResourceFile.h"
#include <Math/CMathFNV.h>
#include <Utilities/CDebugError.h>
#include <fstream>
#include <cassert>
#include <unordered_map>
#include <functional>

namespace Resources {
	CResourceFile::CResourceFile() { }
	CResourceFile::~CResourceFile() { }
	
	void CResourceFile::Initialize() {
		m_resScript.SetData(m_data.resData);
		m_resScript.Build();
	}
};
