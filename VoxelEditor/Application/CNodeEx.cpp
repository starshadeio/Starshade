//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CNodeEx.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CNodeEx.h"
#include "CNodeObjectEx.h"
#include "CEditor.h"
#include <Utilities/CFileSystem.h>

namespace App
{
	CNodeEx::CNodeEx(const wchar_t* pName, u32 viewHash) :
		CNode(pName, viewHash)
	{
	}

	CNodeEx::~CNodeEx()
	{
	}

	//-----------------------------------------------------------------------------------------------
	// File methods.
	//-----------------------------------------------------------------------------------------------
	
	void CNodeEx::New()
	{
		m_chunkNode.Reset();
		
		for(auto elem : m_objMap)
		{
			dynamic_cast<CNodeObjectEx*>(elem.second)->New();
		}
	}

	void CNodeEx::Save() const
	{
		auto path = CEditor::Instance().Project().GetProjectNodesPath() + L"\\" + m_pName;
		Util::CFileSystem::Instance().NewDirectory(path.c_str());
		m_chunkNode.Save(path);

		{ // Save node objects.
			std::ofstream file(path + L"\\package.dat", std::ios::binary);
			assert(file.is_open());

			if(file.is_open())
			{
				for(auto elem : m_objMap)
				{
					u64 hash = elem.second->GetHash();
					file.write(reinterpret_cast<const char*>(&hash), sizeof(hash));
					elem.second->Save(file);
				}

				file.close();
			}
		}
	}

	void CNodeEx::Load()
	{
		auto path = CEditor::Instance().Project().GetProjectNodesPath() + L"\\" + m_pName;
		m_chunkNode.Load(path);

		{ // Load node objects.
			std::ifstream file(path + L"\\package.dat", std::ios::binary);
			assert(file.is_open());

			if(file.is_open())
			{
				while(!file.eof())
				{
					u64 hash;
					file.read(reinterpret_cast<char*>(&hash), sizeof(hash));
					if(file)
					{
						auto elem = m_objMap.find(hash);
						if(elem != m_objMap.end())
						{
							elem->second->Load(file);
						}
					}
				}

				file.close();
			}
		}
	}

	void CNodeEx::Build() const
	{
		auto filepath = CEditor::Instance().Project().GetBuildDataPath() + L"\\" + m_pName + L".dat";
		std::ofstream file(filepath, std::ios::binary);
		assert(file.is_open());

		if(file.is_open())
		{
			m_chunkNode.Save(file);
		
			// Save node objects.
			for(auto elem : m_objMap)
			{
				u64 hash = elem.second->GetHash();
				file.write(reinterpret_cast<const char*>(&hash), sizeof(hash));
				elem.second->Save(file);
			}

			file.close();
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// File methods.
	//-----------------------------------------------------------------------------------------------
	
	CNodeObject* CNodeEx::CreateNodeObject(const wchar_t* name)
	{
		return new CNodeObjectEx(name, m_viewHash);
	}
};
