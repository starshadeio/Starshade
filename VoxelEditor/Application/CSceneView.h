//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CSceneView.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSCENEVIEW_H
#define CSCENEVIEW_H

#include "../Universe/CCyberGrid.h"
#include "../Universe/CCyberNode.h"
#include <Graphics/CPostSky.h>
#include <Graphics/CPostToneMapping.h>
#include <Application/CScene.h>

namespace App
{
	class CSceneView : public CScene
	{
	public:
		CSceneView();
		~CSceneView();
		CSceneView(const CSceneView&) = delete;
		CSceneView(CSceneView&&) = delete;
		CSceneView& operator = (const CSceneView&) = delete;
		CSceneView& operator = (CSceneView&&) = delete;

	protected:
		void Initialize() final;
		void PostInitialize() final;
		void Load() final;
		void Unload() final;

		void Update() final;
		void LateUpdate() final;
		void Release() final;

		virtual void OnResize(const Math::Rect& rect) final;

	private:
		Universe::CCyberGrid m_cyberGrid;
		Universe::CCyberNode m_cyberNode;
	};
};

#endif
