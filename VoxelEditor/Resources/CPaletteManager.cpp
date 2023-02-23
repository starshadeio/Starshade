//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Resources/CPaletteManager.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPaletteManager.h"
#include "CAssetManager.h"
#include <Application/CCommandManager.h>
#include <Factory/CFactory.h>
#include <Graphics/CTexture.h>
#include <Math/CMathFNV.h>
#include <Utilities/CMemoryFree.h>
#include <vector>

namespace Resources
{
	const u64 CPaletteManager::CMD_KEY_PALETTE_COLOR_SELECT = Math::FNV1a_64(L"palette_color_select");
	const u64 CPaletteManager::CMD_KEY_PALETTE_RECENT_SELECT = Math::FNV1a_64(L"palette_recent_select");
	const u64 CPaletteManager::CMD_KEY_PALETTE_RECENT_INDEX_CHANGE = Math::FNV1a_64(L"palette_recent_index_change");

	CPaletteManager::CPaletteManager() : 
		m_paletteSlots(256),
		m_paletteIndex(0),
		m_pTexture(nullptr)
	{
		m_paletteColors.resize(256);
		size_t index = 0;
		
		// 00-07: black to white.
		for(int i = 15; i >= 0; --i)
		{
			m_paletteColors[index++] = Math::Color::ColorFromHSV(Math::Color(0.0f, 0.0f, (1.0f  / 15.0f) * i));
		}

		const float HUE_LIST[10] = {
			00.0f / 36.0f, // Red
			12.5f / 360.0f, // Orange
			30.0f / 360.0f, // Orange
			06.0f / 36.0f, // Yellow
			09.0f / 36.0f, // Lime
			12.0f / 36.0f, // Green
			19.0f / 36.0f, // Cyan
			23.0f / 36.0f, // Blue
			27.0f / 36.0f, // Purple
			31.0f / 36.0f, // Pink
		};

		for(int i = 0; i < 10; ++i)
		{
			for(int j = 1; j <= 3; ++j)
			{
				for(int k = 8; k >= 1; --k)
				{
					m_paletteColors[index++] = Math::Color::ColorFromHSV(Math::Color(HUE_LIST[i], Math::g_1Over3 * j, (1.0f  / 8.0f) * k));
				}
			}
		}
		
		m_recentIndices.resize(32);
		for(auto& elem : m_recentIndices)
		{
			elem = 0;
		}
	}

	CPaletteManager::~CPaletteManager()
	{
	}
	
	void CPaletteManager::Initialize()
	{
		{ // Setup texture.
			m_pTexture = CFactory::Instance().CreateTexture();

			Graphics::CTexture::Data data { };

			data.width = 128;
			data.height = 512;
			data.depth = 1;
			data.stride = 4;
			data.textureFormat = Graphics::GFX_FORMAT_R8G8B8A8_UNORM_SRGB;
			data.textureType = Graphics::CTexture::TTP_TEXTURE2D;
			data.name = L"palette_0";
			
			auto GenerateTextureData = [this](u32 width, u32 height, u32 pixelSize, std::vector<u8>& texture) {
				const u32 rowPitch = width * pixelSize;
				const u32 cellPitch = rowPitch >> 4;
				const u32 cellHeight = height;
				const u32 textureSize = rowPitch * height;

				texture.resize(textureSize);
				u8* pData = &texture[0];

				for(u32 n = 0; n < textureSize; n += pixelSize)
				{
					const u32 x = n % rowPitch;
					const u32 y = n / rowPitch;
					const u32 i = x / (pixelSize * 16);
					const u32 j = y / 16;
					const u32 index = j * 8 + i;

					if(i < m_paletteSlots)
					{
						Math::Color c = m_paletteColors[index];
						pData[n] = static_cast<u8>(roundf(c.r * 0xFF));
						pData[n + 1] = static_cast<u8>(roundf(c.g * 0xFF));
						pData[n + 2] = static_cast<u8>(roundf(c.b * 0xFF));
						pData[n + 3] = 0xFF;
					}
					else
					{
						pData[n] = 0x00;
						pData[n + 1] = 0x00;
						pData[n + 2] = 0x00;
						pData[n + 3] = 0x00;
					}
				}
			};

			std::vector<u8> texture;
			GenerateTextureData(data.width, data.height, data.stride, texture);
			data.pTextureData = texture.data();

			m_pTexture->SetData(data);
			m_pTexture->Initialize();

			CAssets::Instance().RegisterAsset(RESOURCE_TYPE_TEXTURE, L"TEXTURE_ASSET_PALETTE_0", m_pTexture);
		}

		{ // Setup commands.
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_PALETTE_COLOR_SELECT, std::bind(&CPaletteManager::ColorSelect, this, std::placeholders::_1));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_PALETTE_RECENT_SELECT, std::bind(&CPaletteManager::RecentSelect, this, std::placeholders::_1));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_PALETTE_RECENT_INDEX_CHANGE, std::bind(&CPaletteManager::RecentIndexChange, this, std::placeholders::_1));
		}
	}
	
	void CPaletteManager::PostInitialize()
	{
		m_pTexture->PostInitialize();
	}
	
	void CPaletteManager::Release()
	{
		SAFE_RELEASE_DELETE(m_pTexture);
	}

	//-----------------------------------------------------------------------------------------------
	// Commands
	//-----------------------------------------------------------------------------------------------

	bool CPaletteManager::ColorSelect(const void* params)
	{
		m_paletteIndex = static_cast<BlockId>(*reinterpret_cast<const u32*>(params));

		// Update recent index.
		int index = static_cast<int>(m_recentIndices.size());
		for(int i = index - 1; i >= 0; --i)
		{
			if(m_recentIndices[i] == m_paletteIndex)
			{
				index = i;
				break;
			}
		}

		std::pair<u32, u32> indexVal;

		while(index > 0)
		{
			if(index < static_cast<int>(m_recentIndices.size()))
			{
				indexVal = std::pair<u32, u32>(index, m_recentIndices[index - 1]);
				App::CCommandManager::Instance().Execute(CMD_KEY_PALETTE_RECENT_INDEX_CHANGE, &indexVal);
			}

			--index;
		}

		indexVal = std::pair<u32, u32>(index, m_paletteIndex);
		App::CCommandManager::Instance().Execute(CMD_KEY_PALETTE_RECENT_INDEX_CHANGE, &indexVal);

		return true;
	}

	bool CPaletteManager::RecentSelect(const void* params)
	{
		u32 index = m_recentIndices[m_recentIndices.size() - 1 - *reinterpret_cast<const u32*>(params)];
		return App::CCommandManager::Instance().Execute(CMD_KEY_PALETTE_COLOR_SELECT, &index);
	}

	bool CPaletteManager::RecentIndexChange(const void* params)
	{
		std::pair<u32, u32> indexVal = *reinterpret_cast<const std::pair<u32, u32>*>(params);
		m_recentIndices[indexVal.first] = indexVal.second;

		return true;
	}
};
