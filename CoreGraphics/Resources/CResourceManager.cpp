//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Resources/CResourceManager.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CResourceManager.h"
#include "../Application/CPlatform.h"
#include "../Factory/CFactory.h"
#include "../Audio/CAudioClip.h"
#include "../Graphics/CShaderTrie.h"
#include "../Graphics/CMaterial.h"
#include "../Graphics/CTexture.h"
#include "../Graphics/CMesh.h"
#include "../UI/CFont.h"
#include <Application/CLocalization.h>
#include <Utilities/CFileSystem.h>
#include <Math/CMathFNV.h>
#include <Utilities/CDebugError.h>
#include <Utilities/CMemoryFree.h>
#include <Utilities/CFileUtil.h>
#include <Utilities/CScriptObject.h>
#include <codecvt>
#include <locale>

namespace Resources
{
	static const std::unordered_map<std::wstring, RESOURCE_TYPE> RES_FILE_MAP {
		{ RES_STR_LOCALIZATION, RESOURCE_TYPE_LOCALIZATION },
		{ RES_STR_SCRIPT, RESOURCE_TYPE_SCRIPT },
		{ RES_STR_SHADER, RESOURCE_TYPE_SHADER },
		{ RES_STR_MATERIAL, RESOURCE_TYPE_MATERIAL },
		{ RES_STR_TEXTURE, RESOURCE_TYPE_TEXTURE },
		{ RES_STR_FONT, RESOURCE_TYPE_FONT },
		{ RES_STR_AUDIO, RESOURCE_TYPE_AUDIO },
		{ RES_STR_MESH, RESOURCE_TYPE_MESH },
		{ RES_STR_PREFAB, RESOURCE_TYPE_PREFAB },
	};

	CManager::CManager() { }
	CManager::~CManager() { }

	void CManager::Initialize()
	{
		CResourceFile::Data resData { };
		resData.resData.filename = m_data.filename;
#ifndef PRODUCTION_BUILD
		resData.resData.filepath = m_data.filepath;
#else
		resData.resData.filepath = m_data.prodPath;
#endif

		resData.resData.addResource = std::bind(&CManager::AddResource, this, std::placeholders::_1, std::placeholders::_2);

		m_resFile.SetData(resData);
		m_resFile.Initialize();
		
		for(auto texture : m_textureMap)
		{
			texture.second->Initialize();
		}
		
		for(auto font : m_fontMap)
		{
			font.second->Initialize();
		}
		
		for(auto shaderTrie : m_shaderTrieMap)
		{
			shaderTrie.second->Setup();
		}

		for(auto material : m_materialMap)
		{
			material.second->ParseFile();
		}

		for(auto material : m_materialMap)
		{
			material.second->Initialize();
		}

		for(auto clip : m_audioMap)
		{
			clip.second->Initialize();
		}

		for(auto mesh : m_meshMap)
		{
			mesh.second->Initialize();
		}
	}

	void CManager::PostInitialize()
	{
		for(auto texture : m_textureMap)
		{
			texture.second->PostInitialize();
		}
		
		for(auto font : m_fontMap)
		{
			font.second->PostInitialize();
		}

		for(auto material : m_materialMap)
		{
			material.second->PostInitialize();
		}
	}

	void CManager::Release()
	{
		for(auto mesh : m_meshMap)
		{
			SAFE_RELEASE_DELETE(mesh.second);
		}
		
		for(auto clip : m_audioMap)
		{
			SAFE_RELEASE_DELETE(clip.second);
		}

		for(auto material : m_materialMap)
		{
			SAFE_RELEASE_DELETE(material.second);
		}

		for(auto shaderTrie : m_shaderTrieMap)
		{
			SAFE_RELEASE_DELETE(shaderTrie.second);
		}

		for(auto font : m_fontMap)
		{
			SAFE_RELEASE_DELETE(font.second);
		}

		for(auto texture : m_textureMap)
		{
			SAFE_RELEASE_DELETE(texture.second);
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CManager::BuildProductionResources()
	{
#ifndef PRODUCTION_BUILD
		// Delete
		if(Util::CFileSystem::Instance().VerifyDirectory(m_data.prodPath))
		{
			ASSERT_R(Util::CFileSystem::Instance().DeleteDirectory(m_data.prodPath));
			ASSERT_R(Util::CFileSystem::Instance().NewDirectory(m_data.prodPath));
		}
		else
		{
			ASSERT_R(Util::CFileSystem::Instance().NewPath(m_data.prodPath));
		}

		{ // Even though WUI scripts are separate from the resource manager, this will still handle moving them over.
			CFactory::Instance().GetPlatform()->SaveProductionScripts();
		}

		{ // Copy resource file over to production.
			Util::WriteFileUTF8((std::wstring(m_data.prodPath) + m_data.filename).c_str(), m_resFile.GetResScript().GetCode());
		}

		App::CLocalization::Instance().SaveProductionFile(m_data.filepath, m_data.prodPath);
		SaveProductionScripts();

		for(auto texture : m_textureMap)
		{
			texture.second->SaveProductionFile();
		}
		
		for(auto font : m_fontMap)
		{
			font.second->SaveProductionFile();
		}

		for(auto shaderTrie : m_shaderTrieMap)
		{
			shaderTrie.second->BuildCSH();
		}

		for(auto material : m_materialMap)
		{
			material.second->SaveProductionFile();
		}

		for(auto clip : m_audioMap)
		{
			clip.second->SaveProductionFile();
		}

		for(auto mesh : m_meshMap)
		{
			mesh.second->SaveProductionFile();
		}
#endif
	}

	//-----------------------------------------------------------------------------------------------
	// Accessors.
	//-----------------------------------------------------------------------------------------------

	void* CManager::GetResource(RESOURCE_TYPE type, const std::string& key) const
	{
		return GetResource(type, Math::FNV1a_64(key.c_str(), key.size()));
	}

	void* CManager::GetResource(RESOURCE_TYPE type, u64 key) const
	{
		switch(type)
		{
			case RESOURCE_TYPE_SCRIPT: {
				auto elem = m_scriptMap.find(key);
				if(elem != m_scriptMap.end())
				{
					return const_cast<void*>(reinterpret_cast<const void*>(&elem->second));
				}
			} break;
			case RESOURCE_TYPE_SHADER: {
				auto elem = m_shaderTrieMap.find(key);
				if(elem != m_shaderTrieMap.end())
				{
					return elem->second;
				}
			} break;
			case RESOURCE_TYPE_MATERIAL: {
				auto elem = m_materialMap.find(key);
				if(elem != m_materialMap.end())
				{
					return elem->second;
				}
			} break;
			case RESOURCE_TYPE_TEXTURE: {
				auto elem = m_textureMap.find(key);
				if(elem != m_textureMap.end())
				{
					return elem->second;
				}
				else
				{
					auto elemEx = m_textureMapEx.find(key);
					if(elemEx != m_textureMapEx.end())
					{
						return elemEx->second;
					}
				}
			} break;
			case RESOURCE_TYPE_FONT: {
				auto elem = m_fontMap.find(key);
				if(elem != m_fontMap.end())
				{
					return elem->second;
				}
			} break;
			case RESOURCE_TYPE_AUDIO: {
				auto elem = m_audioMap.find(key);
				if(elem != m_audioMap.end())
				{
					return elem->second;
				}
			} break;
			case RESOURCE_TYPE_MESH: {
				auto elem = m_meshMap.find(key);
				if(elem != m_meshMap.end())
				{
					return elem->second;
				}
			} break;
			case RESOURCE_TYPE_PREFAB: {
			} break;
		}

		if(m_resourceCallback != nullptr)
		{
			return m_resourceCallback(type, key);
		}
		else
		{
			return nullptr;
		}
	}

	void CManager::AddResourceEx(RESOURCE_TYPE type, u64 key, void* pResource)
	{
		switch(type)
		{
			case RESOURCE_TYPE_TEXTURE:
				ASSERT(m_textureMapEx.insert({ key, reinterpret_cast<Graphics::CTexture*>(pResource) }).second);
				break;
			default:
				assert(false);
				break;
		}
	}

	void CManager::AddResource(const std::wstring& category, const Util::CompilerTuple<3, std::wstring, wchar_t>& res)
	{
		auto elem = RES_FILE_MAP.find(category);
		ASSERT_R(elem != RES_FILE_MAP.end());

		switch(elem->second)
		{
			case RESOURCE_TYPE_LOCALIZATION:
			{
				App::CLocalization::Instance().RegisterFile(res.GetElement(0), m_data.filepath + res.GetElement(1));
			} break;
			case RESOURCE_TYPE_SCRIPT:
			{
				RegisterScript(res.GetElement(0), m_data.filepath + res.GetElement(1));
			} break;
			case RESOURCE_TYPE_SHADER:
			{
				Graphics::CShaderTrie* pShaderTrie = new Graphics::CShaderTrie();
				Graphics::CShaderTrie::Data data { };
				data.filename = m_data.filepath + res.GetElement(1);
				pShaderTrie->SetData(data);
				ASSERT(m_shaderTrieMap.insert({ Math::FNV1a_64(res.GetElement(0).c_str(), res.GetElement(0).size()), pShaderTrie }).second);

			} break;
			case RESOURCE_TYPE_MATERIAL:
			{
				Graphics::CMaterial* pMaterial = new Graphics::CMaterial();
				Graphics::CMaterial::Data data { };
				data.filename = m_data.filepath + res.GetElement(1);
				pMaterial->SetData(data);
				ASSERT(m_materialMap.insert({ Math::FNV1a_64(res.GetElement(0).c_str(), res.GetElement(0).size()), pMaterial }).second);

			}	break;
			case RESOURCE_TYPE_TEXTURE:
			{
				Graphics::CTexture* pTexture = CFactory::Instance().CreateTexture();
				Graphics::CTexture::Data data { };
				data.filename = m_data.filepath + res.GetElement(1);
				pTexture->SetData(data);
				ASSERT(m_textureMap.insert({ Math::FNV1a_64(res.GetElement(0).c_str(), res.GetElement(0).size()), pTexture }).second);

			} break;
			case RESOURCE_TYPE_FONT:
			{
				UI::CFont* pTexture = new UI::CFont();
				UI::CFont::Data data { };
				data.filename = m_data.filepath + res.GetElement(1);
				pTexture->SetData(data);
				ASSERT(m_fontMap.insert({ Math::FNV1a_64(res.GetElement(0).c_str(), res.GetElement(0).size()), pTexture }).second);

			} break;
			case RESOURCE_TYPE_AUDIO:
			{
				Audio::CAudioClip* pAudioClip = new Audio::CAudioClip();
				Audio::CAudioClip::Data data { };
				data.filename = m_data.filepath + res.GetElement(1);
				data.meta = res.GetElement(2);
				pAudioClip->SetData(data);
				ASSERT(m_audioMap.insert({ Math::FNV1a_64(res.GetElement(0).c_str(), res.GetElement(0).size()), pAudioClip }).second);

			}	break;
			case RESOURCE_TYPE_MESH:
			{
				Graphics::CMesh* pMesh = new Graphics::CMesh();
				Graphics::CMesh::Data data { };
				data.filename = m_data.filepath + res.GetElement(1);
				pMesh->SetData(data);
				ASSERT(m_meshMap.insert({ Math::FNV1a_64(res.GetElement(0).c_str(), res.GetElement(0).size()), pMesh }).second);
			} break;
			case RESOURCE_TYPE_PREFAB:
				break;
			default:
				break;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Script.
	//-----------------------------------------------------------------------------------------------
	
	void CManager::RegisterScript(const std::wstring& key, const std::wstring& filepath)
	{
		const u64 hash = Math::FNV1a_64(key.c_str(), key.size());
		auto elem = m_scriptMap.find(hash);
		assert(elem == m_scriptMap.end());
		m_scriptMap.insert({ hash, filepath });
	}

	void CManager::SaveProductionScripts()
	{
		for(auto elem : m_scriptMap)
		{
			if(elem.second.empty()) continue;

			std::wstring path;
			std::wstring filename;
			std::wstring extension;
			Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(elem.second.c_str(), path, filename, extension);
		
			path = m_data.prodPath + path.substr(wcslen(m_data.filepath));
			ASSERT_R(Util::CFileSystem::Instance().NewPath(path.c_str()));

			Util::WriteFileUTF8((path + filename + extension).c_str(), Util::CScriptObject::FormatScript(Util::ReadFileUTF8(elem.second.c_str())));
		}
	}
};
