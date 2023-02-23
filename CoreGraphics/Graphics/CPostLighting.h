//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPostLighting.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPOSTLIGHTING_H
#define CPOSTLIGHTING_H

#include "CPostEffect.h"

namespace Graphics
{
	class CPostLighting : public CPostEffect
	{
	public:
		struct Data
		{
			class CPostSky* pPostSky;
			class CPostEdge* pPostEdge;
			class CPostSSAO* pPostSSAO;
		};

	public:
		CPostLighting();
		~CPostLighting();
		CPostLighting(const CPostLighting&) = delete;
		CPostLighting(CPostLighting&&) = delete;
		CPostLighting& operator = (const CPostLighting&) = delete;
		CPostLighting& operator = (CPostLighting&&) = delete;

		void Initialize() final;
		void PostInitialize() final;
		const class CRenderTexture** ApplyEffect(const CRenderTexture** ppTargetTextureList, u32 targetTextureCount,
			const CRenderTexture** ppPostTextureList, u32 postTextureCount, u32& outRenderTextureCount) final;
		void OnResize(const Math::Rect& rect) final;
		void Release() final;

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		Data m_data;
	};
};

#endif
