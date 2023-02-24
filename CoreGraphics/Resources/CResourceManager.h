//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Resources/CResourceManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CRESOURCEMANAGER_H
#define CRESOURCEMANAGER_H

#include "CResourceFile.h"
#include <Utilities/CCompilerUtil.h>
#include <Utilities/CArchive.h>
#include <string>
#include <unordered_map>
#include <functional>

namespace Audio
{
	class CAudioClip;
};

namespace Graphics
{
	class CShaderTrie;
	class CMaterial;
	class CTexture;
	class CMesh;
};

namespace UI
{
	class CFont;
};

namespace Resources
{
	static const wchar_t* META_DATA_EXTENSION = L".meta";

	static const wchar_t* RES_STR_LOCALIZATION = L"LOCALIZATION";
	static const wchar_t* RES_STR_SCRIPT = L"SCRIPTS";
	static const wchar_t* RES_STR_DATA = L"DATA";
	static const wchar_t* RES_STR_SHADER = L"SHADERS";
	static const wchar_t* RES_STR_MATERIAL = L"MATERIALS";
	static const wchar_t* RES_STR_TEXTURE = L"TEXTURES";
	static const wchar_t* RES_STR_FONT = L"FONTS";
	static const wchar_t* RES_STR_AUDIO = L"AUDIO";
	static const wchar_t* RES_STR_MESH = L"MESHES";
	static const wchar_t* RES_STR_PREFAB = L"PREFABS";

	enum RESOURCE_TYPE
	{
		RESOURCE_TYPE_LOCALIZATION,
		RESOURCE_TYPE_SCRIPT,
		RESOURCE_TYPE_DATA,
		RESOURCE_TYPE_SHADER,
		RESOURCE_TYPE_MATERIAL,
		RESOURCE_TYPE_TEXTURE,
		RESOURCE_TYPE_FONT,
		RESOURCE_TYPE_AUDIO,
		RESOURCE_TYPE_MESH,
		RESOURCE_TYPE_PREFAB,
		RESOURCE_TYPE_COUNT,
	};

	class CManager
	{
	public:
		struct Data
		{
			const wchar_t* filepath;
			const wchar_t* prodPath;
			const wchar_t* filename;
		};

	private:
		CManager();
		~CManager();
		CManager(const CManager&) = delete;
		CManager(CManager&&) = delete;
		CManager& operator = (const CManager&) = delete;
		CManager& operator = (CManager&&) = delete;

	public:
		static CManager& Instance()
		{
			static CManager instance;
			return instance;
		}

		void Initialize();
		void PostInitialize();
		void Release();

		void BuildProductionResources();

		void AddResourceEx(RESOURCE_TYPE type, u64 key, void* pResource);

		void* GetResource(RESOURCE_TYPE type, const std::string& key) const;
		void* GetResource(RESOURCE_TYPE type, u64 key) const;

		// Accessors.
		const wchar_t* GetFilePath() const { return m_data.filepath; }
		const wchar_t* GetProductionPath() const { return m_data.prodPath; }

		inline const CResourceFile& GetResourceFile() { return m_resFile; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetGetResourceCallback(std::function<void*(RESOURCE_TYPE, u64)> callback) { m_resourceCallback = callback; }

	private:
		void AddResource(const std::wstring& category, const Util::CompilerTuple<3, std::wstring, wchar_t>& res);

		void RegisterScript(const std::wstring& key, const std::wstring& filepath);
		void RegisterData(const std::wstring& key, const std::wstring& filepath);
		void SaveProductionScripts();
		void SaveProductionData();

	private:
		Data m_data;
		CResourceFile m_resFile;

		std::function<void*(RESOURCE_TYPE, u64)> m_resourceCallback;
		
		std::unordered_map<u64, std::wstring> m_scriptMap;
		std::unordered_map<u64, std::wstring> m_dataMap;
		std::unordered_map<u64, Graphics::CTexture*> m_textureMap;
		std::unordered_map<u64, Graphics::CTexture*> m_textureMapEx;
		std::unordered_map<u64, UI::CFont*> m_fontMap;
		std::unordered_map<u64, Graphics::CShaderTrie*> m_shaderTrieMap;
		std::unordered_map<u64, Graphics::CMaterial*> m_materialMap;
		std::unordered_map<u64, Audio::CAudioClip*> m_audioMap;
		std::unordered_map<u64, Graphics::CMesh*> m_meshMap;
	};
};

#endif
