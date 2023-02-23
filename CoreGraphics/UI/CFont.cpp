//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CFont.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CFont.h"
#include "../Resources/CResourceManager.h"
#include "../Factory/CFactory.h"
#include "../Graphics/CTexture.h"
#include "../Resources/CResourceManager.h"
#include <Utilities/CFileSystem.h>
#include <Utilities/CDebugError.h>
#include <Utilities/CMemoryFree.h>
#include <fstream>
#include <vector>

namespace UI
{
	CFont::CFont() : 
		m_pTexture(nullptr)
	{
	}

	CFont::~CFont()
	{
	}
	
	void CFont::Initialize()
	{
		u8* pTextureData = nullptr;

		{
			std::ifstream file(m_data.filename, std::ios::binary);
			assert(file.is_open());

			file.read(reinterpret_cast<char*>(&m_header), sizeof(SDFFontHeader));
			
			m_maxBounds = Math::VEC2_ZERO;
			m_maxGlyphHeight = Math::VECINT2_ZERO;

			std::vector<SDFGlyphData> glyphList(m_header.glyphs);
			file.read(reinterpret_cast<char*>(glyphList.data()), sizeof(SDFGlyphData) * m_header.glyphs);
			for(auto& glyph : glyphList)
			{
				m_glyphMap[static_cast<wchar_t>(glyph.id)] = glyph;

				m_maxBounds.x = std::max(m_maxBounds.x, glyph.uv[2]);
				m_maxBounds.y = std::max(m_maxBounds.y, glyph.uv[3]);
			
				s32 hang = glyph.height - glyph.hBearingY;
				m_maxGlyphHeight.y = std::max(m_maxGlyphHeight.y, hang);
				m_maxGlyphHeight.x = std::max(m_maxGlyphHeight.x, glyph.height + s32(m_header.padding) / 64 - hang);
			}

			size_t loc = file.tellg();

			u32 sz = m_header.width * m_header.height;
			pTextureData = new u8[sz];
			file.read(reinterpret_cast<char*>(pTextureData), sz);

			file.close();
		}

		{ // Texture.
			m_pTexture = CFactory::Instance().CreateTexture();

			Graphics::CTexture::Data data { };
			data.width = m_header.width;
			data.height = m_header.height;
			data.depth = 1;
			data.stride = 1;
			data.textureFormat = Graphics::GFX_FORMAT::GFX_FORMAT_R8_UNORM;
			data.textureType = Graphics::CTexture::TTP_TEXTURE2D;
			data.pTextureData = pTextureData;

			m_pTexture->SetData(data);
			m_pTexture->Initialize();
		}
	}
	
	void CFont::PostInitialize()
	{
		m_pTexture->PostInitialize();
	}

	void CFont::SaveProductionFile()
	{
		if(m_data.filename.empty()) return;

		std::wstring path;
		std::wstring filename;
		std::wstring extension;
		Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(m_data.filename.c_str(), path, filename, extension);
		
		path = Resources::CManager::Instance().GetProductionPath() + path.substr(wcslen(Resources::CManager::Instance().GetFilePath()));
		ASSERT_R(Util::CFileSystem::Instance().NewPath(path.c_str()));

		ASSERT_R(Util::CFileSystem::Instance().CopyFileTo(m_data.filename.c_str(), (path + filename + extension).c_str(), true));

		/*std::wstring path;
		std::wstring filename;
		std::wstring extension;
		Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(m_data.filename.c_str(), path, filename, extension);
		
		path = Resources::CManager::Instance().GetProductionPath() + path.substr(wcslen(Resources::CManager::Instance().GetFilePath()));
		ASSERT_R(Util::CFileSystem::Instance().NewPath((path).c_str()));

		std::ifstream fileIn(m_data.filename, std::ios::binary | std::ios::ate);
		ASSERT_R(fileIn.is_open());

		const size_t sz = fileIn.tellg();
		fileIn.seekg(0, std::ios::beg);
		
		std::vector<char> data(sz, '\0');
		fileIn.read(data.data(), sz);

		std::ofstream fileOut(path + filename + extension, std::ios::binary);
		ASSERT_R(fileOut.is_open());

		fileOut.write(data.data(), sz);

		fileOut.clear();
		fileIn.clear();*/
	}
	
	void CFont::Release()
	{
		SAFE_RELEASE_DELETE(m_pTexture);
	}
};
