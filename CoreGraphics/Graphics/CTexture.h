//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CTexture.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CTEXTURE_H
#define CTEXTURE_H

#include "CGraphicsData.h"
#include <Globals/CGlobals.h>
#include <string>

namespace Graphics
{
	class CTexture
	{
	public:
		enum TEXTURE_TYPE
		{
			TTP_TEXTURE1D,
			TTP_TEXTURE2D,
			TTP_TEXTURE3D,
			TTP_TEXTURECUBE,
			TTP_RTV,
			TTP_DSV,
		};

		struct Data
		{
			u32 width;
			u32 height;
			u32 depth;
			u32 stride;
			u32 index;

			TEXTURE_TYPE textureType;
			GFX_FORMAT textureFormat;
			bool bCube = false;

			std::wstring filename;
			std::wstring name;

			u8* pTextureData;
			class CRenderTexture* pRenderTexture;
		};

	protected:
		CTexture();
		CTexture(const CTexture&) = delete;
		CTexture(CTexture&&) = delete;
		CTexture& operator = (const CTexture&) = delete;
		CTexture& operator = (CTexture&&) = delete;

	public:
		virtual ~CTexture();

		virtual void Initialize() = 0;
		virtual void PostInitialize() = 0;
		void SaveProductionFile();

		virtual void Bind(u32& index, bool bCompute) = 0;
		virtual void Release() = 0;

		virtual void* GetCPUHandle(DESC_HEAP_TYPE tp) = 0;

		virtual void TransitionToShader() = 0;
		virtual void TransitionFromShader() = 0;

		virtual void SaveAsTexture(const wchar_t* filename) = 0;

		// Accessors.
		inline u32 GetWidth() const { return m_data.width; }
		inline u32 GetHeight() const { return m_data.height; }

		inline const std::wstring& GetName() const { return m_data.name; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	protected:
		Data m_data;
	};
};

#endif
