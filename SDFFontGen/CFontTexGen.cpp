//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Console Application: SDF Font Texture Generator
//
// File: CFontTexGen.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CFontTexGen.h"
#include <Math/CMathFloat.h>
#include <assert.h>
#include <fstream>
#include <functional>
#include <cstdio>
#include <iostream>

static const u32 DIV = 1;

CFontTexGen::CFontTexGen()
{
	m_width = 4096 / DIV;
	m_height = 4096 / DIV;
	m_dpi = 300;
	m_fontMin = 16 * 64 / DIV;
	m_fontMax = 256 * 64 / DIV;
	m_fontThreshold = 16;
	m_superSamples = 4;

	m_sdfDist = 64 / DIV;
	m_padding = m_sdfDist + 2;

	m_bFastPack = true;
	
	m_bHasKerning = false;
	m_bHasHorizontal = false;
	m_bHasVertical = false;
}

CFontTexGen::~CFontTexGen()
{
}

bool CFontTexGen::Generate()
{
	FT_Error error;
	error = FT_Init_FreeType(&m_library);
	assert(error == 0);
	
	error = FT_New_Face(m_library, m_filename.c_str(), 0, &m_face);
	assert(error == 0);
	
	error = FT_New_Face(m_library, m_fileDefault.c_str(), 0, &m_faceDefault);
	assert(error == 0);

	// Temp size for counting glyphs.
	error = FT_Set_Char_Size(m_face, 0, 16 * 64, m_dpi, m_dpi);
	assert(error == 0);
	error = FT_Set_Char_Size(m_faceDefault, 0, 16 * 64, m_dpi, m_dpi);
	assert(error == 0);
	
	m_bHasKerning = FT_HAS_KERNING(m_face);
	m_bHasHorizontal = FT_HAS_HORIZONTAL(m_face);
	m_bHasVertical = FT_HAS_VERTICAL(m_face);
	
	// Count printable ASCII character glyphs.
	u32 total = 0;

	// Find 

	for(unsigned long i = 0x1A; i <= 0x7E; i++)
	{
		u32 index = 0;
		if(i == 0x1A) {
			i = 0x1F;
			if(index == 0) index = FT_Get_Char_Index(m_face, 0x25A1);
			if(index == 0) index = FT_Get_Char_Index(m_faceDefault, 0x25A1);
			if(index == 0) continue;
		} else {
			index = FT_Get_Char_Index(m_face, i);
			if(index == 0) continue;
		}
		
		//error = FT_Load_Glyph(m_face, index, FT_LOAD_NO_BITMAP);
		//assert(error == 0);
		
		//if(m_face->glyph->bitmap.width == 0 ||
		//	m_face->glyph->bitmap.rows == 0) continue;

		total++;
	}
	
	if(total == 0) return false; // Create glyph list.
	GlyphData* glyphDataList = new GlyphData[total];
	UI::SDFGlyphData* deltaGlyphTable = new UI::SDFGlyphData[total];
	memset(deltaGlyphTable, 0, sizeof(UI::SDFGlyphData) * total);
	
	int fontLeft = m_fontMin;
	int fontRight = m_fontMax;
	m_fontSize = (fontLeft + fontRight) / 2;
	
	const u32 imagePixels = m_width * m_height;
	const u32 imagePixelsSub = m_width / m_superSamples * m_height / m_superSamples;
	
	u8* imageMask = new u8[imagePixelsSub];

FONT_SIZE_ATTEMPT:
	error = FT_Set_Char_Size(m_face, 0, m_fontSize, m_dpi, m_dpi);
	assert(error == 0);
	error = FT_Set_Char_Size(m_faceDefault, 0, m_fontSize, m_dpi, m_dpi);
	assert(error == 0);
	
	// Store printable glyph characters
	total = 0;
	for(unsigned long i = 0x1A; i <= 0x7E; i++)
	{
		u32 index = 0;
		FT_Face* pFace = &m_face;
		if(i == 0x1A)
		{
			if(index == 0) index = FT_Get_Char_Index(m_face, 0x25A1);
			if(index == 0) { pFace = &m_faceDefault; index = FT_Get_Char_Index(m_faceDefault, 0x25A1); }
			if(index == 0) { i = 0x1F; continue; }
		}
		else
		{
			index = FT_Get_Char_Index(m_face, i);
			if(index == 0) continue;
		}
		
		error = FT_Load_Glyph(*pFace, index, FT_LOAD_NO_BITMAP);
		assert(error == 0);
		
		//if(m_face->glyph->bitmap.width == 0 ||
		//	m_face->glyph->bitmap.rows == 0) continue;

		deltaGlyphTable[total].id = i;
		glyphDataList[total] = { i, total, index, (*pFace)->glyph->bitmap.width *
			(*pFace)->glyph->bitmap.rows, pFace };
		total++;

		if(i == 0x1A) i = 0x1F;
	}

	// Sort glyph data by pixel totals from largest to smallest.
	SortGlyphData(glyphDataList, 0, total - 1);
	memset(imageMask, 0, imagePixelsSub);
	
	// Find the best fonts size.
	for(u32 i = 0; i < total; i++)
	{
		error = FT_Load_Glyph(*glyphDataList[i].pFace, glyphDataList[i].index, FT_LOAD_NO_BITMAP);
		assert(error == 0);

		if(glyphDataList[i].charIndex != 0x20 &&
		 !TestGlyphForStorage((*glyphDataList[i].pFace)->glyph, imageMask, m_width, m_height)) 
		{
			if(m_fontSize == m_fontMin)
			{
				delete [] deltaGlyphTable;
				delete [] glyphDataList;
				delete [] imageMask;
				return false;
			}
			else
			{
				fontRight = m_fontSize;
				m_fontSize = (fontRight + fontLeft) / 2;
				goto FONT_SIZE_ATTEMPT;
			}
		}
	}

	if(fontLeft < fontRight - m_fontThreshold)
	{
		fontLeft = m_fontSize;
		m_fontSize = (fontRight + fontLeft) / 2;
		goto FONT_SIZE_ATTEMPT;
	}

	// Store actual pixels now the best font size is found.
	u32* image = new u32[imagePixelsSub];
	memset(image, 0, imagePixelsSub);
	memset(imageMask, 0, imagePixelsSub);
	m_threadCounter = 0;
	for(u32 i = 0; i < total; i++)
	{
		if(glyphDataList[i].charIndex != 0x20)
		{
			error = FT_Load_Glyph(*glyphDataList[i].pFace, glyphDataList[i].index, FT_LOAD_DEFAULT);
			assert(error == 0);
		
			error = FT_Render_Glyph((*glyphDataList[i].pFace)->glyph, FT_RENDER_MODE_NORMAL);
			assert(error == 0);
		}
		else
		{
			error = FT_Load_Glyph(*glyphDataList[i].pFace, glyphDataList[i].index, FT_LOAD_NO_BITMAP);
			assert(error == 0);
		}

		AttemptToStoreGlyph((*glyphDataList[i].pFace)->glyph, imageMask, image, deltaGlyphTable[glyphDataList[i].tableIndex], m_width, m_height);
	}
	
	while(m_threadCounter > 0) { std::this_thread::sleep_for(std::chrono::milliseconds(10)); }

	// Save as Bitmap.
	BITMAP_HEADER header;
	const size_t headerSz = sizeof(header);
	header.size = headerSz + imagePixelsSub * sizeof(u32);
	header.rawSize = imagePixelsSub * sizeof(u32);
	header.width = m_width / m_superSamples;
	header.height = m_height / m_superSamples;

	const size_t len = m_filename.length();
	m_filename[len - 3] = 'b';
	m_filename[len - 2] = 'm';
	m_filename[len - 1] = 'p';
	std::ofstream output(m_filename, std::ios::binary);
	
	output.write((char*)&header.id, headerSz);

	for(u32 i = 0; i < header.height; i++)
	{
		for(u32 j = 0; j < header.width; j++)
		{
			int k = (header.height - i - 1) * header.width + j;
			if(imageMask[k] == 0)
			{
				image[k] = 0xFF000000;
			}

			output << static_cast<char>((image[k]) & 0xFF);
			output << static_cast<char>((image[k] >> 8) & 0xFF);
			output << static_cast<char>((image[k] >> 16) & 0xFF);
			output << static_cast<char>((image[k] >> 24) & 0xFF);//*/
		}
	}
	
	output.close();
	
	// Save as memory delta font file.
	m_filename[len - 3] = 'f';
	m_filename[len - 2] = 'n';
	m_filename[len - 1] = 't';
	output.open(m_filename, std::ios::binary);
	
	UI::SDFFontHeader deltaFontHeader;
	deltaFontHeader.width = header.width;
	deltaFontHeader.height = header.height;
	deltaFontHeader.dpi = m_dpi;
	deltaFontHeader.fontSize = m_fontSize;
	deltaFontHeader.padding = m_padding * m_fontSize;
	deltaFontHeader.glyphs = total;

	output.write((char*)&deltaFontHeader, sizeof(UI::SDFFontHeader));
	output.write((char*)&deltaGlyphTable[0], sizeof(UI::SDFGlyphData) * total);
	
	for(u32 i = 0; i < header.height; i++)
	{
		for(u32 j = 0; j < header.width; j++)
		{
			int k = i * header.width + j;
			if(imageMask[k] == 0)
			{
				output << '\0';
			}
			else
			{
				output << static_cast<char>((image[k]) & 0xFF);
			}
		}
	}

	output.close();

	// Free memory.
	delete [] image;
	delete [] imageMask;
	delete [] deltaGlyphTable;
	delete [] glyphDataList;

	return true;
}

// Test if the x, y slot is open.
bool CFontTexGen::TestGlyph(u32 x, u32 y, FT_GlyphSlot& glyph, u8* imageMask, u32 width, u32 height)
{
	const int stepSize = (m_bFastPack ? m_superSamples : 1);
	const u32 widthSub = width / m_superSamples;
	const u32 heightSub = height / m_superSamples;

	for(u32 i = 0; i < glyph->bitmap.rows + m_padding * 2; i += stepSize)
	{
		for(u32 j = 0; j < glyph->bitmap.width + m_padding * 2; j += stepSize)
		{
			if(x + j >= width || y + i >= height)
				return false;

			u32 index = (heightSub - (y + i) / m_superSamples - 1) * widthSub + (x + j) / m_superSamples;
			if(imageMask[index] != 0)
				return false;
		}
	}
	
	return true;
}

// Method for storing a glyph into an image mask and image.
void CFontTexGen::StoreGlyph(u32 x, u32 y, FT_Bitmap* glyphBmp, 
 u8* imageMask, u32* image, u32 width, u32 height, bool maskOnly)
{
	const int stepSize = (m_bFastPack ? m_superSamples : 1);
	const u32 widthSub = width / m_superSamples;
	const u32 heightSub = height / m_superSamples;

	if(maskOnly)
	{ // Only write to image mask.
		for(u32 i = 0; i < glyphBmp->rows + m_padding * 2; i += stepSize)
		{
			for(u32 j = 0; j < glyphBmp->width + m_padding * 2; j += stepSize)
			{
				u32 k = (heightSub - (y + i) / m_superSamples - 1) * widthSub + (x + j) / m_superSamples;
				imageMask[k] = 0xFF;
			}
		}
	}
	else
	{
		m_threadCounter++;

		// Write to mask and to image.
		for(u32 i = stepSize / 2; i < glyphBmp->rows + m_padding * 2; i += stepSize)
		{
			for(u32 j = stepSize / 2; j < glyphBmp->width + m_padding * 2; j += stepSize)
			{
				u32 k = (heightSub - (y + i) / m_superSamples - 1) * widthSub + (x + j) / m_superSamples;
				// Calculate value of glyph's indexed pixel. Set as zero for out of bounds.
				u8 val = 0;
				if(i >= m_padding && i < glyphBmp->rows + m_padding && 
				 j >= m_padding && j < glyphBmp->width + m_padding)
				{
					u32 glyphIndex = (glyphBmp->rows - (i - m_padding) - 1) * glyphBmp->width + (j - m_padding);
					val = glyphBmp->buffer[glyphIndex];
				}

				float minDist = FLT_MAX;
				float sdf = 0.0f;

				if(val > 127)
				{
					// Generate SDF for inner pixel.
					for(int m = -m_sdfDist; m <= m_sdfDist; m++)
					{
						for(int n = -m_sdfDist; n <= m_sdfDist; n++)
						{
							if((m + int(i)) < int(m_padding) || (n + int(j)) < int(m_padding) ||
							 (m + int(i)) >= int(glyphBmp->rows + m_padding) || 
							 (n + int(j)) >= int(glyphBmp->width + m_padding))
							{
								float dist = (powf(static_cast<float>(m), 2) + powf(static_cast<float>(n), 2));// / m_superSamples;
								if(dist <= minDist)
									minDist = dist;
								continue;
							}

							// Check if we're on an edge.
							bool edge = false;
							for(int r = -1; r <= 1; r++)
							{
								for(int s = -1; s <= 1; s++)
								{
									if(r + (m + int(i)) < int(m_padding) || s + (n + int(j)) < int(m_padding) ||
									 r + (m + int(i)) >= int(glyphBmp->rows + m_padding) || 
									 s + (n + int(j)) >= int(glyphBmp->width + m_padding))
									{
										edge = true;
										break;
									}

									u32 edgeIndex = (glyphBmp->rows - (((m + int(i)) + r) - m_padding) - 1) * 
										glyphBmp->width + (((n + int(j)) + s) - m_padding);
									if(glyphBmp->buffer[edgeIndex] <= 127)
									{
										edge = true;
										break;
									}
								} if(edge) break;
							}
									
							u32 glyphIndex = (glyphBmp->rows - ((m + int(i)) - m_padding) - 1) * 
								glyphBmp->width + ((n + int(j)) - m_padding);
							if(edge || glyphBmp->buffer[glyphIndex] <= 127)
							{
								float dist = (powf(static_cast<float>(m), 2) + powf(static_cast<float>(n), 2));// / m_superSamples;
								if(dist <= minDist)
									minDist = dist;
							}
						}
					}

					sdf = Math::Saturate(sqrtf(minDist / (m_sdfDist * m_sdfDist)));
				}
				else
				{
					// Generate SDF for outer pixel.
					for(int m = -m_sdfDist; m <= m_sdfDist; m++)
					{
						for(int n = -m_sdfDist; n <= m_sdfDist; n++)
						{
							if((m + int(i)) < int(m_padding) || (n + int(j)) < int(m_padding) ||
							 (m + int(i)) >= int(glyphBmp->rows + m_padding) || 
							 (n + int(j)) >= int(glyphBmp->width + m_padding))
							{
								continue;
							}

							u32 glyphIndex = (glyphBmp->rows - ((m + int(i)) - m_padding) - 1) * 
								glyphBmp->width + ((n + int(j)) - m_padding);
							if(glyphBmp->buffer[glyphIndex] > 127)
							{
								float dist = (powf(static_cast<float>(m), 2) + powf(static_cast<float>(n), 2));// / m_superSamples;
								if(dist <= minDist)
								{
									minDist = dist;
								}
							}
						}
					}
					
					sdf = -Math::Saturate(sqrtf(minDist / (m_sdfDist * m_sdfDist)));
				}
				
				sdf = Math::Clamp((sdf + 1.0f) * 127.5f, 0.0f, 255.0f);
				val = static_cast<u8>(sdf);
				u32 pixel = 0;
				pixel  = static_cast<u32>(val);
				pixel |= static_cast<u32>(val) << 8;
				pixel |= static_cast<u32>(val) << 16;
				pixel |= 0xFF000000;

				image[k] = std::max(image[k], pixel);
			}
		}

		delete [] glyphBmp->buffer;
		delete glyphBmp;
		m_threadCounter--;
	}
}

// Method for testing if there's room for a glyph in a image.
bool CFontTexGen::TestGlyphForStorage(FT_GlyphSlot& glyph, u8* imageMask, u32 width, u32 height)
{
	const int stepSize = (m_bFastPack ? m_superSamples : 1);
	u32 x = 0;
	u32 y = 0;

	while(x < width && y < height)
	{
		if(TestGlyph(x, y, glyph, imageMask, width, height))
		{
			StoreGlyph(x, y, &glyph->bitmap, imageMask, nullptr, width, height, true);
			return true;
		}

		x += stepSize;
		if(x >= width)
		{
			x = 0;
			y += stepSize;
		}
	}

	return false;
}

// Method for attempting to store the glyph.
bool CFontTexGen::AttemptToStoreGlyph(FT_GlyphSlot& glyph, u8* imageMask, u32* image, UI::SDFGlyphData& deltaGlyphData, u32 width, u32 height)
{
	const int stepSize = (m_bFastPack ? m_superSamples : 1);
	u32 x = 0;
	u32 y = 0;

	while(x < width && y < height)
	{
		if(glyph->bitmap.buffer == nullptr || TestGlyph(x, y, glyph, imageMask, width, height))
		{
			if(glyph->bitmap.buffer != nullptr)
				StoreGlyph(x, y, &glyph->bitmap, imageMask, nullptr, width, height, true);

			// UV coords
			if(glyph->bitmap.buffer == nullptr)
			{
				memset(deltaGlyphData.uv, 0, sizeof(float) * 4);
			}
			else
			{
				deltaGlyphData.uv[0] = static_cast<float>(x) / width;
				deltaGlyphData.uv[1] = static_cast<float>(y) / height;
				deltaGlyphData.uv[2] = static_cast<float>(glyph->bitmap.width + m_padding * 2) / width;
				deltaGlyphData.uv[3] = static_cast<float>(glyph->bitmap.rows + m_padding * 2) / height;
			}

			// Glyph bound's dimensions.
			deltaGlyphData.width = glyph->metrics.width;
			deltaGlyphData.height = glyph->metrics.height;

			// Horizontal metrics.
			if(m_bHasHorizontal)
			{
				deltaGlyphData.hAdvance = glyph->metrics.horiAdvance;
				deltaGlyphData.hBearingX = glyph->metrics.horiBearingX;
				deltaGlyphData.hBearingY = glyph->metrics.horiBearingY;
			}
			else
			{
				deltaGlyphData.hAdvance = glyph->metrics.width;
				deltaGlyphData.hBearingX = 0;
				deltaGlyphData.hBearingY = 0;
			}
			
			// Vertical metrics.
			if(m_bHasVertical)
			{
				deltaGlyphData.vAdvance= glyph->metrics.vertAdvance;
				deltaGlyphData.vBearingX = glyph->metrics.vertBearingX;
				deltaGlyphData.vBearingY = glyph->metrics.vertBearingY;
			}
			else
			{
				deltaGlyphData.vAdvance = glyph->metrics.height;
				deltaGlyphData.vBearingX = 0;
				deltaGlyphData.vBearingY = 0;
			}

			if(glyph->bitmap.buffer != nullptr)
			{
				while(m_threadCounter >= 8) { std::this_thread::sleep_for(std::chrono::milliseconds(10)); }

				FT_Bitmap* glyphBmp = new FT_Bitmap();
				glyphBmp->width = glyph->bitmap.width;
				glyphBmp->rows = glyph->bitmap.rows;
				glyphBmp->pitch = glyph->bitmap.pitch;
				glyphBmp->buffer = new u8[glyphBmp->rows * glyphBmp->pitch];
				memcpy(glyphBmp->buffer, glyph->bitmap.buffer, glyphBmp->rows * glyphBmp->pitch);

				std::thread thread(std::bind(&CFontTexGen::StoreGlyph, this, x, y, glyphBmp, imageMask, image, width, height, false));
				thread.detach();
				//StoreGlyph(x, y, glyph, imageMask, image, width, height, false);
			}

			return true;
		}

		x += stepSize;
		if(x >= width)
		{
			x = 0;
			y += stepSize;
		}
	}

	return false;
}

// Method for sorting glyph data from largest to smallest.
void CFontTexGen::SortGlyphData(GlyphData* data, int left, int right)
{
	if(left >= right) return;
	int l = left;
	int r = right;

	// Set pivot to center of left and right.
	u32 pivot = data[(left + right) / 2].pixels;

	// Find first left <= pivot pixels.
	// Find first right >= pivot pixels.
	// Swap the two values .
	while(l <= r)
	{
		while(data[l].pixels > pivot) l++;
		while(data[r].pixels < pivot) r--;
		if(l <= r)
		{
			if(l != r)
			{ // Swap if indices aren't equal.
				GlyphData tmp = data[l];
				data[l] = data[r];
				data[r] = tmp;
			} l++; r--;
		}
	}

	// Recurse from both sides.
	SortGlyphData(data, left, r);
	SortGlyphData(data, l, right);
}

void CFontTexGen::ProcessGlyph(FT_GlyphSlot& glyph)
{
	FT_Bitmap bitmap = glyph->bitmap;
	FT_Glyph_Metrics metrics = glyph->metrics;

	const u32 imageSize = bitmap.width * bitmap.rows;
	
	BITMAP_HEADER header;
	const size_t headerSz = sizeof(header);
	header.size = headerSz + imageSize;
	header.rawSize = imageSize;
	header.width = bitmap.width;
	header.height = bitmap.rows;

	std::ofstream output("test.bmp", std::ios::binary);
	//FILE* file;
	//fopen_s(&file, "test.bmp", "w");

	//fwrite(&header, headerSz, 1, file);
	output.write((char*)&header.id, headerSz);
	for(u32 i = 0; i < bitmap.rows; i++)
	{
		for(u32 j = 0; j < bitmap.width; j++)
		{
			int k = (bitmap.rows - i - 1) * bitmap.pitch + j;
			output << bitmap.buffer[k];
			output << bitmap.buffer[k];
			output << bitmap.buffer[k];
			output << (char)0xFF;
		}
	}
	
	//fclose(file);

	//output.write((char*)bitmap.buffer, imageSize);
	output.close();

	return;
}
