//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CNodeEditor.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODEEDITOR_H
#define CNODEEDITOR_H

#include "CNodeSelect.h"
#include "CNodeEx.h"
#include "CGizmo.h"
#include "../Actors/CUser.h"
#include "../Universe/CNodeGrid.h"
#include "../Universe/CChunkEditor.h"
#include "../Graphics/CMeshSpawnPoint.h"
#include <Universe/CEnvironment.h>
#include <fstream>

namespace App
{
	class CNodeEditor
	{
	public:
		CNodeEditor();
		~CNodeEditor();
		CNodeEditor(const CNodeEditor&) = delete;
		CNodeEditor(CNodeEditor&&) = delete;
		CNodeEditor& operator = (const CNodeEditor&) = delete;
		CNodeEditor& operator = (CNodeEditor&&) = delete;

		void Initialize();
		void PostInitialize();
		void Update();
		void LateUpdate();
		void PhysicsUpdate();
		void Release();
		
		void OnResize(const Math::Rect& rect);

		void New();
		void Save() const;
		void Load();
		void Build() const;
		
		void OnPlay(bool bSpawn);
		void OnEdit();
		
		// Accessors.
		inline const CNodeSelect& GetSelector() const { return m_nodeSelect; }

		inline CGizmo& Gizmo() { return m_gizmo; }

	private:
		CGizmo m_gizmo;

		CNodeEx m_rootNode;
		CNodeSelect m_nodeSelect;

		Graphics::CMeshSpawnPoint m_meshSpawnPoint;
		
		Actor::CUser m_user;
		Universe::CNodeGrid m_grid;
		Universe::CChunkEditor m_chunkEditor;
		Universe::CEnvironment m_environment;

		CNodeEx* m_pSelectedNode;
	};
};

#endif
