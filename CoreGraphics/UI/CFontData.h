//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CFontData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CFONTDATA_H
#define CFONTDATA_H

#include <Globals/CGlobals.h>

namespace UI
{
#pragma pack(push, 1)
	struct SDFFontHeader {
		u32 width;
		u32 height;
		u32 dpi;
		u32 fontSize;
		u32 padding;
		u32 glyphs;
	};

	struct SDFGlyphData {
		u32 id = ~0;
		float uv[4];
		s32 width;
		s32 height;
		s32 hAdvance;
		s32 hBearingX;
		s32 hBearingY;
		s32 vAdvance;
		s32 vBearingX;
		s32 vBearingY;
	};
#pragma pack(pop)

	constexpr bool IsValidGlyph(const SDFGlyphData& glyph) { return glyph.id != ~0; }
};

#endif
