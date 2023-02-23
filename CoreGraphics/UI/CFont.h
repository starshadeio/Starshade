//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CFont.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CFONT_H
#define CFONT_H

#include "CFontData.h"
#include <Math/CMathVector2.h>
#include <Math/CMathVectorInt2.h>
#include <string>
#include <unordered_map>

namespace Graphics
{
	class CTexture;
};

namespace UI
{
	class CFont
	{
	public:
		struct Data
		{
			std::wstring filename;
		};

	public:
		CFont();
		~CFont();
		CFont(const CFont&) = delete;
		CFont(CFont&&) = delete;
		CFont& operator = (const CFont&) = delete;
		CFont& operator = (CFont&&) = delete;
		
		void Initialize();
		void PostInitialize();
		void SaveProductionFile();
		void Release();

		// Accessors.
		inline const Math::Vector2& GetMaxBounds() const { return m_maxBounds; }
		inline const Math::VectorInt2& GetMaxGlyphHeight() const { return m_maxGlyphHeight; }

		inline const SDFFontHeader& GetHeader() const { return m_header; }
		inline const SDFGlyphData& GetGlyph(wchar_t ch) const { return m_glyphMap.find(ch)->second; }
		inline Graphics::CTexture* GetTexture() const { return m_pTexture; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		Data m_data;

		Math::Vector2 m_maxBounds;
		Math::VectorInt2 m_maxGlyphHeight;

		SDFFontHeader m_header;
		std::unordered_map<wchar_t, SDFGlyphData> m_glyphMap;

		Graphics::CTexture* m_pTexture;
	};
};

#endif
