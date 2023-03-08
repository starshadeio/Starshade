//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CNodeVersion.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CNodeVersion.h"
#include <Application/CAppVersion.h>
#include <fstream>

namespace App
{
	CNodeVersion::CNodeVersion() : 
		m_version(0)
	{
	}

	CNodeVersion::~CNodeVersion()
	{
	}

	void CNodeVersion::Save(const std::wstring& path)
	{
		std::ofstream file(path + L"\\version", std::ios::binary);
		if(file.is_open())
		{
			m_version = App::CVersion::Instance().GetVersion();
			file.write(reinterpret_cast<char*>(&m_version), sizeof(m_version));
			file.close();
		}
	}

	void CNodeVersion::Load(const std::wstring& path)
	{
		std::ifstream file(path + L"\\version", std::ios::binary);
		if(file.is_open())
		{
			file.read(reinterpret_cast<char*>(&m_version), sizeof(m_version));
			file.close();
		}
		else
		{
			m_version = 0;
		}
	}
};
