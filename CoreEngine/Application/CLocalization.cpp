//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Application/CLocalization.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CLocalization.h"
#include "../Utilities/CFileSystem.h"
#include "../Utilities/CFileUtil.h"
#include "../Utilities/CDebugError.h"
#include "../Utilities/CScriptObject.h"
#include "../Math/CMathFNV.h"

namespace App
{
	CLocalization::CLocalization()
	{
	}

	CLocalization::~CLocalization()
	{
	}

	void CLocalization::RegisterFile(const std::wstring& key, const std::wstring& filepath)
	{
		const u64 hash = Math::FNV1a_64(key.c_str(), key.size());
		auto elem = m_fileMap.find(hash);
		assert(elem == m_fileMap.end());
		m_fileMap.insert({ hash, filepath });
	}

	void CLocalization::Load(u64 hash)
	{
		std::wstring dir;
		std::wstring filename;
		std::wstring extension;
		Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(m_fileMap.find(hash)->second.c_str(), dir, filename, extension);

		Util::CResScript::Data data { };
		data.filepath = dir;
		data.filename = filename + extension;
		data.addResource = std::bind(&CLocalization::AddResource, this, std::placeholders::_1, std::placeholders::_2);

		m_resScript.SetData(data);
		m_resScript.Build();
	}
	
	void CLocalization::SaveProductionFile(const wchar_t* filepath, const wchar_t* prodPath)
	{
		for(auto elem : m_fileMap)
		{
			if(elem.second.empty()) continue;

			std::wstring path;
			std::wstring filename;
			std::wstring extension;
			Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(elem.second.c_str(), path, filename, extension);
		
			path = prodPath + path.substr(wcslen(filepath));
			ASSERT_R(Util::CFileSystem::Instance().NewPath(path.c_str()));

			ASSERT_R(Util::CFileSystem::Instance().CopyFileTo(elem.second.c_str(), (path + filename + extension).c_str(), true));
			Util::WriteFileUTF8((path + filename + extension).c_str(), Util::CScriptObject::FormatScript(Util::ReadFileUTF8(elem.second.c_str())));
		}
	}

	void CLocalization::AddResource(const std::wstring& category, const Util::CompilerTuple<3, std::wstring, wchar_t>& res)
	{
		m_stringMap.emplace(std::piecewise_construct, std::forward_as_tuple(Math::FNV1a_64((category + L":" + res.GetElement(0)).c_str())), 
			std::forward_as_tuple(res.GetElement(1), res.IsComplete() ? res.GetElement(2) : L""));
	}
};
