//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CViewScene.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CVIEWSCENE_H
#define CVIEWSCENE_H

#include <Graphics/CPostSky.h>
#include <Graphics/CPostEdge.h>
#include <Graphics/CPostSSAO.h>
#include <Graphics/CPostLighting.h>
#include <Graphics/CPostToneMapping.h>
#include <Graphics/CPostFXAA.h>
#include <Application/CView.h>

namespace App
{
	class CViewScene : public CView
	{
	public:
		CViewScene();
		~CViewScene();
		CViewScene(const CViewScene&) = delete;
		CViewScene(CViewScene&&) = delete;
		CViewScene& operator = (const CViewScene&) = delete;
		CViewScene& operator = (CViewScene&&) = delete;

	protected:
		void Initialize() final;
		void PostInitialize() final;
		bool Load() final;
		bool Unload() final;

		void LateUpdate() final;
		void Release() final;
		
	public:
		void OnResize(const Math::Rect& rect) final;

	private:
		Graphics::CPostSky m_postSky;
		Graphics::CPostEdge m_postEdge;
		Graphics::CPostSSAO m_postSSAO;
		Graphics::CPostLighting m_postLighting;
		Graphics::CPostToneMapping m_postToneMapping;
		Graphics::CPostFXAA m_postFXAA;
	};
};

#endif
