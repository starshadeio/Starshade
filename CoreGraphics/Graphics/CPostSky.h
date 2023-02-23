//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPostSky.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPOSTSKY_H
#define CPOSTSKY_H

#include "CPostEffect.h"

namespace Graphics
{
	class CPostSky : public CPostEffect
	{
	public:
		CPostSky();
		~CPostSky();
		CPostSky(const CPostSky&) = delete;
		CPostSky(CPostSky&&) = delete;
		CPostSky& operator = (const CPostSky&) = delete;
		CPostSky& operator = (CPostSky&&) = delete;

		void Initialize() final;
		void PostInitialize() final;
		const class CRenderTexture** ApplyEffect(const CRenderTexture** ppTargetTextureList, u32 targetTextureCount,
			const CRenderTexture** ppPostTextureList, u32 postTextureCount, u32& outRenderTextureCount) final;
		void OnResize(const Math::Rect& rect) final;
		void Release() final;

		inline void SetSkyInterpolation(float t) { m_skyInterp = t; }

	private:
		float m_skyInterp;
	};
};

#endif
