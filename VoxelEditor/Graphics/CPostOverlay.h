//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Graphics/CPostOverlay.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPOSTOVERLAY_H
#define CPOSTOVERLAY_H

#include <Graphics/CPostEffect.h>

namespace App
{
	class CView;
}

namespace Graphics
{
	class CPostOverlay : public CPostEffect
	{
	public:
		struct Data
		{
			App::CView* pViewScene;
		};

	public:
		CPostOverlay();
		~CPostOverlay();
		CPostOverlay(const CPostOverlay&) = delete;
		CPostOverlay(CPostOverlay&&) = delete;
		CPostOverlay& operator = (const CPostOverlay&) = delete;
		CPostOverlay& operator = (CPostOverlay&&) = delete;

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
