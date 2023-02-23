//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRenderTexture.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CRENDERTEXTURE_H
#define CRENDERTEXTURE_H

#include "CGraphicsData.h"
#include <Math/CMathRect.h>
#include <string>

namespace Graphics
{
	class CRenderTexture
	{
	public:
		struct Data
		{
			u8 colorBufferCount;
			bool bUseDepthStencil;

			u32 width;
			u32 height;
			u32 downScale;

			GFX_FORMAT colorFormat[8];
			GFX_FORMAT depthFormat;

			Math::Color clearColor[8];
			DepthStencil clearDepthStencil;

			std::wstring colorTexKey[8];
			std::wstring depthTexKey;
		};

	public:
		CRenderTexture();
		virtual ~CRenderTexture();
		CRenderTexture(const CRenderTexture&) = delete;
		CRenderTexture(const CRenderTexture&&) = delete;
		CRenderTexture& operator = (const CRenderTexture&) = delete;
		CRenderTexture& operator = (const CRenderTexture&&) = delete;

		void Initialize();
		void PostInitialize();
		void Resize();
		void BeginRender(CLEAR_MODE clearMode, u32 numRects, const Math::RectLTRB* pRects);
		void EndRender();
		void Bind(u32& index, bool bCompute);
		void Release();

		// Accessors.
		inline const Math::Color& GetClearColor(u32 index) const { return m_data.clearColor[index]; }
		inline const DepthStencil& GetClearDepthStencil() const { return m_data.clearDepthStencil; }
		inline class CTexture* GetRTVTexture(u32 index) const { return m_pRTVTexture[index]; }
		inline class CTexture* GetDSVTexture() const { return m_pDSVTexture; }

		inline u64 GetRTVTextureHash(u32 index) const { return m_rtvTextureHash[index]; }
		inline u64 GetDSVTextureHash() const { return m_dsvTextureHash; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		float m_aspectRatio;
		Viewport m_viewport;
		Math::RectLTRB m_scissorRect;

		Data m_data;

		u64 m_rtvTextureHash[8];
		u64 m_dsvTextureHash;

		class CTexture* m_pRTVTexture[8];
		class CTexture* m_pDSVTexture;
		class CGraphicsAPI* m_GraphicsAPI;
	};
};

#endif
