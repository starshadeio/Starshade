//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPostSSAO.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPOSTSSAO_H
#define CPOSTSSAO_H

#include "CPostEffect.h"

namespace Graphics
{
	class CPostSSAO : public CPostEffect
	{
	public:
		CPostSSAO();
		~CPostSSAO();
		CPostSSAO(const CPostSSAO&) = delete;
		CPostSSAO(CPostSSAO&&) = delete;
		CPostSSAO& operator = (const CPostSSAO&) = delete;
		CPostSSAO& operator = (CPostSSAO&&) = delete;

		void Initialize() final;
		void PostInitialize() final;
		const class CRenderTexture** ApplyEffect(const CRenderTexture** ppTargetTextureList, u32 targetTextureCount,
			const CRenderTexture** ppPostTextureList, u32 postTextureCount, u32& outRenderTextureCount) final;
		void OnResize(const Math::Rect& rect) final;
		void Release() final;

	private:
		class CMaterial* m_pHBlurMat;
		class CMaterial* m_pVBlurMat;
	};
};

#endif
