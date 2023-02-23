//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Resources/CAssetManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CASSETMANAGER_H
#define CASSETMANAGER_H

#include "CPaletteManager.h"
#include <Resources/CResourceManager.h>
#include <unordered_map>
#include <string>

namespace Resources
{
	class CAssets
	{
	public:
		static CAssets& Instance()
		{
			static CAssets instance;
			return instance;
		}

	private:
		CAssets();
		~CAssets();
		CAssets(const CAssets&) = delete;
		CAssets(CAssets&&) = delete;
		CAssets& operator = (const CAssets&) = delete;
		CAssets& operator = (CAssets&&) = delete;

	public:
		void Initialize();
		void PostInitialize();
		void Release();

		void RegisterAsset(RESOURCE_TYPE type, const std::wstring& key, void* pResource);
		void DeregisterAsset(RESOURCE_TYPE type, const std::wstring& key);

		void Save(const std::wstring& path);

		// Accessors.
		inline CPaletteManager& GetPalette() { return m_palette; }
		
	private:
		void* GetResource(RESOURCE_TYPE type, u64 hash) const;

	private:
		std::unordered_map<u64, Graphics::CTexture*> m_textureMap;
		std::unordered_map<u64, std::wstring> m_keyMap;

		CPaletteManager m_palette;
	};
};

#endif
