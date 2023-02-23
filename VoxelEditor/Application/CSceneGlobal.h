//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CSceneGlobal.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSCENEGLOBAL_H
#define CSCENEGLOBAL_H

#include "../Actors/CUser.h"
#include <Application/CScene.h>

namespace App
{
	class CSceneGlobal : public CScene
	{
	public:
		CSceneGlobal();
		~CSceneGlobal();
		CSceneGlobal(const CSceneGlobal&) = delete;
		CSceneGlobal(CSceneGlobal&&) = delete;
		CSceneGlobal& operator = (const CSceneGlobal&) = delete;
		CSceneGlobal& operator = (CSceneGlobal&&) = delete;

	protected:
		void Initialize() final;
		void PostInitialize() final;

		void Update() final;
		void LateUpdate() final;
		bool OnQuit(int exitCode) final;
		void Release() final;

		void OnResize(const Math::Rect& rect) final;

	private:
	};
};

#endif
