//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPostToneMapping.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPOSTTONEMAPPING_H
#define CPOSTTONEMAPPING_H

#include "CPostEffect.h"

namespace Graphics
{
	class CPostToneMapping : public CPostEffect
	{
	public:
		CPostToneMapping();
		~CPostToneMapping();
		CPostToneMapping(const CPostToneMapping&) = delete;
		CPostToneMapping(CPostToneMapping&&) = delete;
		CPostToneMapping& operator = (const CPostToneMapping&) = delete;
		CPostToneMapping& operator = (CPostToneMapping&&) = delete;

		void Initialize() final;
		void PostInitialize() final;
		const class CRenderTexture** ApplyEffect(const CRenderTexture** ppTargetTextureList, u32 targetTextureCount,
			const CRenderTexture** ppPostTextureList, u32 postTextureCount, u32& outRenderTextureCount) final;
		void OnResize(const Math::Rect& rect) final;
		void Release() final;

	private:
	};
};

#endif
