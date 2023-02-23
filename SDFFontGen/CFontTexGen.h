//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Console Application: SDF Font Texture Generator
//
// File: CFontTexGen.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CFONTTEXGEN_H
#define CFONTTEXGEN_H

#include <Globals/CGlobals.h>
#include <UI/CFontData.h>
#include <ft2build.h>
#include <thread>
#include <string>
#include FT_FREETYPE_H

class CFontTexGen
{
private:
	struct GlyphData
	{
		u32 charIndex;
		u32 tableIndex;
		u32 index;
		u32 pixels;
		FT_Face* pFace;
	};

#pragma pack(push, 1)
	struct BITMAP_HEADER
	{
		u16 id = 0x4D42;
		u32 size = 0;
		u16 r00 = 0;
		u16 r01 = 0;
		u32 offset = 0x46;
		u32 headerSz = 0x38;
		u32 width = 0;
		u32 height = 0;
		u16 colorPlanes = 1;
		u16 bits = 32;
		u32 compression = 3;
		u32 rawSize = 0; // Can stay zero for no compression.
		u32 horizontalRes = 0x1BAF;
		u32 verticalRes = 0x1BAF;
		u32 colorPalette = 0;
		u32 importantColors = 0;
		u32 rMask = 0x000000FF;
		u32 gMask = 0x0000FF00;
		u32 bMask = 0x00FF0000;
		u32 aMask = 0xFF000000;
	};
#pragma pack(pop)

public:
	CFontTexGen();
	~CFontTexGen();
	CFontTexGen(const CFontTexGen&) = delete;
	CFontTexGen(CFontTexGen&&) = delete;
	CFontTexGen& operator = (const CFontTexGen&) = delete;
	CFontTexGen& operator = (CFontTexGen&&) = delete;

	bool Generate();

	inline void SetDimensions(u32 width, u32 height) { m_width = width; m_height = height; }
	inline void SetFontSizeBounds(u32 mn, u32 mx) { m_fontMin = mn; m_fontMax = mx; }
	inline void SetPadding(u32 padding) { m_padding = padding; }
	inline void SetFilename(const char* filename) { m_filename = filename; }
	inline void SetDefaultFilename(const char* filename) { m_fileDefault = filename; }

private:
	bool TestGlyph(u32 x, u32 y, FT_GlyphSlot& glyph, u8* imageMask, u32 width, u32 height);
	void StoreGlyph(u32 x, u32 y, FT_Bitmap* glyphBmp, u8* imageMask, u32* image, u32 width, u32 height, bool maskOnly);
	bool TestGlyphForStorage(FT_GlyphSlot& glyph, u8* imageMask, u32 width, u32 height);
	bool AttemptToStoreGlyph(FT_GlyphSlot& glyph, u8* imageMask, u32* image, UI::SDFGlyphData& deltaGlyphData, u32 width, u32 height);
	void SortGlyphData(GlyphData* data, int left, int right);
	void ProcessGlyph(FT_GlyphSlot& glyph);

private:
	u32 m_width;
	u32 m_height;
	u32 m_dpi;
	u32 m_fontSize;
	u32 m_fontMin;
	u32 m_fontMax;
	int m_fontThreshold;
	int m_superSamples;
	u32 m_padding;
	int m_sdfDist;
	bool m_bFastPack;

	bool m_bHasKerning;
	bool m_bHasHorizontal;
	bool m_bHasVertical;

	Au8 m_threadCounter;

	std::string m_filename;
	std::string m_fileDefault;
	
	FT_Library m_library;
	FT_Face m_face;
	FT_Face m_faceDefault;
};

#endif
