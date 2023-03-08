//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CShaderTrie.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CShaderTrie.h"
#include "CShader.h"
#include "../Resources/CResourceManager.h"
#include "../Factory/CFactory.h"
#include <Utilities/CFileSystem.h>
#include <Utilities/CMemoryFree.h>
#include <Utilities/CDebugError.h>
#include <Utilities/CFileUtil.h>
#include <fstream>

namespace Graphics
{
	CShaderTrie::CShaderTrie() :
		m_pBase(nullptr) {
	}

	CShaderTrie::~CShaderTrie() { }

	void CShaderTrie::Setup()
	{
		CShaderFile::Data data { };
		data.filename = m_data.filename;
		m_shaderFile.SetData(data);
#ifdef PRODUCTION_BUILD
		LoadCSH();
#else
		m_shaderFile.ParseMacros();
#endif
	}
	
	void CShaderTrie::LoadCSH()
	{
		std::wstring path;
		std::wstring filename;
		std::wstring extension;
		Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(m_data.filename.c_str(), path, filename, extension);
		
		std::ifstream file(path + filename + L".csh", std::ios::binary | std::ios::in | std::ios::ate);
		ASSERT_R(file.is_open());
		size_t fileSz = file.tellg();
		file.seekg(0L, std::ios::beg);

		while(static_cast<size_t>(file.tellg()) < fileSz)
		{
			Node* pNode = nullptr;
			size_t macroCount;
			file.read(reinterpret_cast<char*>(&macroCount), sizeof(macroCount));
			
			for(int i = 0; i < macroCount; ++i)
			{
				std::string macro;
				size_t macroIndex = 0;

				size_t macroSz;
				file.read(reinterpret_cast<char*>(&macroSz), sizeof(macroSz));
				if(macroSz > 0)
				{
					macro.resize(macroSz);
					file.read(macro.data(), sizeof(char) * macroSz);
				}

				if(i == macroCount - 1)
				{
					std::string contents;

					size_t propSize;
					file.read(reinterpret_cast<char*>(&propSize), sizeof(propSize));
					contents.resize(contents.size() + propSize + 5);
					contents[0] = '$';
					contents[1] = '\n';
					file.read(contents.data() + 2, sizeof(char) * propSize);
					contents[contents.size() - 3] = '\n';
					contents[contents.size() - 2] = '$';
					contents[contents.size() - 1] = '\n';

					size_t hlslSize;
					file.read(reinterpret_cast<char*>(&hlslSize), sizeof(hlslSize));
					contents.resize(contents.size() + hlslSize);
					file.read(contents.data() + propSize + 5, sizeof(char) * hlslSize);

					m_shaderFile.SetContents(std::move(contents));
				}

				pNode = Insert(macro, i == macroCount - 1, pNode);
			}
		}

		file.close();
	}
	
	void CShaderTrie::BuildCSH()
	{
		if(m_pBase == nullptr) return;
		Node* pNode = m_pBase;

		std::wstring path;
		std::wstring filename;
		std::wstring extension;
		Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(m_data.filename.c_str(), path, filename, extension);

		path = Resources::CManager::Instance().GetProductionPath() + path;//.substr(wcslen(Resources::CManager::Instance().GetFilePath()));
		ASSERT_R(Util::CFileSystem::Instance().NewPath((path).c_str()));

		std::ofstream file(path + filename + L".csh", std::ios::binary);
		ASSERT_R(file.is_open());

		BuildCSHAppend(file, pNode);

		file.close();
	}
	
	void CShaderTrie::BuildCSHAppend(std::ofstream& file, Node* pNode)
	{
		if(pNode == nullptr) return;
		
		if(pNode->pShader)
		{
			size_t sz = pNode->macroList.size();
			file.write(reinterpret_cast<const char*>(&sz), sizeof(sz));

			for(const auto& elem : pNode->macroList)
			{
				sz = elem.size();
				file.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
				if(sz > 0)
				{
					file.write(elem.data(), sizeof(char) * sz);
				}
			}
		
			sz = pNode->pShader->GetProperies().size();
			file.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
			file.write(pNode->pShader->GetProperies().data(), sizeof(char) * sz);
		
			sz = pNode->pShader->GetHLSL().size();
			file.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
			file.write(pNode->pShader->GetHLSL().data(), sizeof(char) * sz);
		}

		for(auto elem : pNode->children)
		{
			BuildCSHAppend(file, elem.second);
		}
	}

	void CShaderTrie::Release()
	{
		Release(m_pBase);
	}

	void CShaderTrie::Release(Node* pNode)
	{
		if(pNode == nullptr) { return; }

		SAFE_RELEASE_DELETE(pNode->pShader);

		for(auto child : pNode->children)
		{
			Release(child.second);
		}

		SAFE_DELETE(pNode);
	}

	// Method for inserting a shader into the trie step by step.
	CShaderTrie::Node* CShaderTrie::Insert(const std::string& macro, bool bTerminal, Node* pNode)
	{
		auto createShader = [this](Node* pNode) {
			pNode->pShader = CFactory::Instance().CreateShader();

			CShader::Data data { };
			data.pNode = pNode;
			pNode->pShader->SetData(data);

			// Build the shader.
			pNode->pShader->Initialize(&m_shaderFile);
		};

		if(pNode == nullptr)
		{
			if(m_pBase == nullptr)
			{
				m_pBase = new Node();
				if(bTerminal)
				{
					m_pBase->macroList.push_back(macro);
				}
			}

			pNode = m_pBase;
			if(macro.empty() && bTerminal)
			{
				// No appending is needed seeing as we're inserting a base shader.
				if(pNode->pShader == nullptr) 
				{
					createShader(pNode);
				}

				return m_pBase;
			}
		}

		// Append node.
		auto elem = pNode->children.find(macro);
		if(elem == pNode->children.end())
		{
			Node* pNextNode = new Node();
			pNextNode->macroList = pNode->macroList;
			pNextNode->macroList.push_back(macro);
			pNode->children.insert({ macro, pNextNode });
			elem = pNode->children.find(macro);
		}

		if(bTerminal && elem->second->pShader == nullptr)
		{
			createShader(elem->second);
		}

		return elem->second;
	}
};
