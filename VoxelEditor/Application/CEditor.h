//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CEditor.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CEDITOR_H
#define CEDITOR_H

#include "CAppState.h"
#include "CProjectManager.h"
#include "CNodeEditor.h"
#include "CEditorKeybinds.h"
#include "../UI/CEditorPanel.h"
#include <Physics/CPhysicsUpdate.h>
#include <fstream>

namespace App
{
	class CEditor
	{
	public:
		static CEditor& Instance()
		{
			static CEditor instance;
			return instance;
		}

	private:
		CEditor();
		~CEditor();
		CEditor(const CEditor&) = delete;
		CEditor(CEditor&&) = delete;
		CEditor& operator = (const CEditor&) = delete;
		CEditor& operator = (CEditor&&) = delete;

	public:
		void Initialize();
		void PostInitialize();
		void Update();
		void LateUpdate();
		void PhysicsUpdate();
		bool OnQuit(int exitCode);
		void Release();
		
		void OnResize(const Math::Rect& rect);

		// Accessors.
		inline CAppState& State() { return m_state; }
		inline CProjectManager& Project() { return m_projectManager; }
		inline CNodeEditor& Nodes() { return m_nodeEditor; }
		
	private:
		void OnCmdPlay(bool bSpawn);
		void OnCmdEdit();

		void OnCmdNew();
		void OnCmdSave();
		void OnCmdLoad();
		void OnCmdBuild();

	private:
		CAppState m_state;
		CProjectManager m_projectManager;
		CNodeEditor m_nodeEditor;

		Physics::CPhysicsUpdateRef m_physicsUpdate;

		CEditorKeybinds m_editorKeybinds;
		UI::CEditorPanel m_editorPanel;
	};
};

#endif
