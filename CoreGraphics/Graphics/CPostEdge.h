//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPostEdge.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPOSTEDGE_H
#define CPOSTEDGE_H

#include "CPostEffect.h"

namespace Graphics
{
	class CPostEdge : public CPostEffect
	{
	public:
		CPostEdge();
		~CPostEdge();
		CPostEdge(const CPostEdge&) = delete;
		CPostEdge(CPostEdge&&) = delete;
		CPostEdge& operator = (const CPostEdge&) = delete;
		CPostEdge& operator = (CPostEdge&&) = delete;

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
