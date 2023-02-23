//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Resources/CPaletteManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPALETTEMANAGER_H
#define CPALETTEMANAGER_H

#include <Universe/CChunkData.h>
#include <Math/CMathColor.h>
#include <Globals/CGlobals.h>
#include <vector>

namespace Graphics
{
	class CTexture;
};

namespace Resources
{
	class CPaletteManager
	{
	public:
		static const u64 CMD_KEY_PALETTE_COLOR_SELECT;
		static const u64 CMD_KEY_PALETTE_RECENT_SELECT;
		static const u64 CMD_KEY_PALETTE_RECENT_INDEX_CHANGE;

	public:
		CPaletteManager();
		~CPaletteManager();
		CPaletteManager(const CPaletteManager&) = delete;
		CPaletteManager(CPaletteManager&&) = delete;
		CPaletteManager& operator = (const CPaletteManager&) = delete;
		CPaletteManager& operator = (CPaletteManager&&) = delete;
		
		void Initialize();
		void PostInitialize();
		void Release();

		// Accessors.
		inline BlockId GetPaletteIndex() const { return m_paletteIndex; }
		inline u32 GetPaletteSlotCount() const { return m_paletteSlots; }

		inline Graphics::CTexture* GetTexture() const { return m_pTexture; }

	private:
		bool ColorSelect(const void* params);
		bool RecentSelect(const void* params);
		bool RecentIndexChange(const void* params);

	private:
		u32 m_paletteSlots;
		AblockId m_paletteIndex;

		std::vector<Math::Color> m_paletteColors;
		std::vector<u32> m_recentIndices;

		Graphics::CTexture* m_pTexture;
	};
};

#endif
