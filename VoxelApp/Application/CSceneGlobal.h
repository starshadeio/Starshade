//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel App
//
// File: Application/CSceneGlobal.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSCENEGLOBAL_H
#define CSCENEGLOBAL_H

#include "CNodeLoader.h"
#include <Universe/CEnvironment.h>
#include <Graphics/CPostSky.h>
#include <Graphics/CPostEdge.h>
#include <Graphics/CPostSSAO.h>
#include <Graphics/CPostLighting.h>
#include <Graphics/CPostToneMapping.h>
#include <Graphics/CPostFXAA.h>
#include <Application/CScene.h>
#include <Application/CInputData.h>
#include <Actors/CPlayer.h>
#include <Actors/CPlayerPawn.h>

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
		void Release() final;

		virtual void OnResize(const Math::Rect& rect) final;
		
	private:
		void Quit(const App::InputCallbackData& callback);

	private:
		Graphics::CPostSky m_postSky;
		Graphics::CPostEdge m_postEdge;
		Graphics::CPostSSAO m_postSSAO;
		Graphics::CPostLighting m_postLighting;
		Graphics::CPostToneMapping m_postToneMapping;
		Graphics::CPostFXAA m_postFXAA;

		Actor::CPlayer m_player;
		Universe::CEnvironment m_environment;

		CNodeLoader m_nodeLoader;
	};
};

#endif
