//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CViewOverlay.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CVIEWOVERLAY_H
#define CVIEWOVERLAY_H

#include "../Graphics/CPostOverlay.h"
#include <Graphics/CPostFXAA.h>
#include <Application/CView.h>

namespace App
{
	class CViewOverlay : public CView
	{
	public:
		struct Data
		{
			class CViewScene* pViewScene;
		};

	public:
		CViewOverlay();
		~CViewOverlay();
		CViewOverlay(const CViewOverlay&) = delete;
		CViewOverlay(CViewOverlay&&) = delete;
		CViewOverlay& operator = (const CViewOverlay&) = delete;
		CViewOverlay& operator = (CViewOverlay&&) = delete;

	protected:
		void Initialize() final;
		void PostInitialize() final;
		bool Load() final;
		bool Unload() final;

		void LateUpdate() final;
		void Release() final;
		
	public:
		void OnResize(const Math::Rect& rect) final;

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		Data m_data;

		Graphics::CPostOverlay m_postOverlay;
	};
};

#endif
