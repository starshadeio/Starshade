//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CSceneNode.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSCENENODE_H
#define CSCENENODE_H

#include "CViewScene.h"
#include "CViewOverlay.h"
#include "../Physics/CTestCube.h"
#include <Graphics/CPostSky.h>
#include <Graphics/CPostToneMapping.h>
#include <Application/CScene.h>

namespace App
{
	class CSceneNode : public CScene
	{
	public:
		CSceneNode();
		~CSceneNode();
		CSceneNode(const CSceneNode&) = delete;
		CSceneNode(CSceneNode&&) = delete;
		CSceneNode& operator = (const CSceneNode&) = delete;
		CSceneNode& operator = (CSceneNode&&) = delete;

	protected:
		void Initialize() final;
		void PostInitialize() final;
		void Load() final;
		void Unload() final;

		void Update() final;
		void LateUpdate() final;
		void Release() final;

		void OnResize(const Math::Rect& rect) final;

	private:
		CViewScene m_viewScene;
		CViewOverlay m_viewOverlay;
	};
};

#endif
