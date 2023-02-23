//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Resources/CAssetManager.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAssetManager.h"
#include <Graphics/CTexture.h>
#include <Math/CMathFNV.h>
#include <Utilities/CDebugError.h>
#include <Utilities/CFileSystem.h>
#include <Utilities/CFileUtil.h>
#include <cassert>
#include <algorithm>

namespace Resources
{
	CAssets::CAssets()
	{
	}

	CAssets::~CAssets()
	{
	}
	
	void CAssets::Initialize()
	{
		CManager::Instance().SetGetResourceCallback(std::bind(&CAssets::GetResource, this, std::placeholders::_1, std::placeholders::_2));

		m_palette.Initialize();
	}
	
	void CAssets::PostInitialize()
	{
		m_palette.PostInitialize();
	}
	
	void CAssets::Release()
	{
		m_palette.Release();
	}
	
	//-----------------------------------------------------------------------------------------------
	// Save methods.
	//-----------------------------------------------------------------------------------------------

	void CAssets::Save(const std::wstring& path)
	{
		std::wstring resScript;
		{ // Build asset resource file.
			auto& code = Resources::CManager::Instance().GetResourceFile().GetResScript().GetCode();
			auto indices = Resources::CManager::Instance().GetResourceFile().GetResScript().GetGroupIndices(Math::FNV1a_32(L"ENGINE"));
			resScript = code.substr(indices.first, indices.second);

			Util::CResScript res;
			Util::CResScript::Data data { };
			data.addResource = [&](const std::wstring& category, const Util::CompilerTuple<3, std::wstring, wchar_t>& res){
				std::wstring fromPath = Resources::CManager::Instance().GetFilePath() + res.GetElement(1);
				std::wstring toPath = path + L"\\" + res.GetElement(1);

				std::wstring dir;
				std::wstring filename;
				std::wstring extension;
				Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(toPath.c_str(), dir, filename, extension);
				Util::CFileSystem::Instance().NewPath(dir.c_str());

				if(category == RES_STR_SHADER)
				{
					toPath = dir + filename + L".csh";

					dir.clear();
					filename.clear();
					extension.clear();

					Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(fromPath.c_str(), dir, filename, extension);
					fromPath = dir + filename + L".csh";
				}

				Util::CFileSystem::Instance().CopyFileTo(fromPath.c_str(), toPath.c_str(), true);
			};
			
			res.SetData(data);
			res.Build(resScript);
		}

		resScript += L"\n@APP {";

		std::wstring assetPath = path + L"\\Assets\\";
		Util::CFileSystem::Instance().NewDirectory(assetPath.c_str());

		{ // Textures.
			resScript += L"\n" + std::wstring(RES_STR_TEXTURE) + L" {";

			std::wstring texturePath = assetPath + L"Textures\\";
			Util::CFileSystem::Instance().NewDirectory(texturePath.c_str());

			for(auto elem : m_textureMap)
			{
				auto textureFilePath = texturePath + elem.second->GetName() + L".dds";
				resScript += L"\n( " + m_keyMap.find(elem.first)->second + L", \"" + textureFilePath.substr(path.size() + 1) + L"\" )";
				elem.second->SaveAsTexture(textureFilePath.c_str());
			}

			resScript += L"\n}";
		}

		resScript += L"\n}\n";
		std::transform(resScript.cbegin(), resScript.cend(), resScript.begin(), [](wchar_t c){ return c == L'\\' ? L'/' : c; });

		Util::WriteFileUTF8((path + L"\\App.res").c_str(), resScript);
	}
	
	//-----------------------------------------------------------------------------------------------
	// (De)registration methods.
	//-----------------------------------------------------------------------------------------------

	void CAssets::RegisterAsset(RESOURCE_TYPE type, const std::wstring& key, void* pResource)
	{
		const u64 hash = Math::FNV1a_64(key.c_str(), key.size());
		m_keyMap.insert({ hash, key });

		switch(type)
		{
			case RESOURCE_TYPE_SHADER:
				break;
			case RESOURCE_TYPE_MATERIAL:
				break;
			case RESOURCE_TYPE_TEXTURE:
				ASSERT(m_textureMap.insert({ hash, reinterpret_cast<Graphics::CTexture*>(pResource) }).second);
				break;
			case RESOURCE_TYPE_FONT:
				break;
			case RESOURCE_TYPE_AUDIO:
				break;
			case RESOURCE_TYPE_MESH:
				break;
			case RESOURCE_TYPE_PREFAB:
				break;
			case RESOURCE_TYPE_COUNT:
				break;
			default:
				break;
		}
	}

	void CAssets::DeregisterAsset(RESOURCE_TYPE type, const std::wstring& key)
	{
		const u64 hash = Math::FNV1a_64(key.c_str(), key.size());
		m_keyMap.erase(hash);

		switch(type)
		{
			case RESOURCE_TYPE_SHADER:
				break;
			case RESOURCE_TYPE_MATERIAL:
				break;
			case RESOURCE_TYPE_TEXTURE:
				m_textureMap.erase(hash);
				break;
			case RESOURCE_TYPE_FONT:
				break;
			case RESOURCE_TYPE_AUDIO:
				break;
			case RESOURCE_TYPE_MESH:
				break;
			case RESOURCE_TYPE_PREFAB:
				break;
			case RESOURCE_TYPE_COUNT:
				break;
			default:
				break;
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void* CAssets::GetResource(RESOURCE_TYPE type, u64 key) const
	{
		switch(type)
		{
			case RESOURCE_TYPE_SHADER:
				break;
			case RESOURCE_TYPE_MATERIAL:
				break;
			case RESOURCE_TYPE_TEXTURE:
				return m_textureMap.find(key)->second;
			case RESOURCE_TYPE_FONT:
				break;
			case RESOURCE_TYPE_AUDIO:
				break;
			case RESOURCE_TYPE_MESH:
				break;
			case RESOURCE_TYPE_PREFAB:
				break;
			case RESOURCE_TYPE_COUNT:
				break;
			default:
				break;
		}

		return nullptr;
	}
};
