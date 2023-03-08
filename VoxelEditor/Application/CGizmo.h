//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmo.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMO_H
#define CGIZMO_H

#include "CGizmoDraw.h"
#include "CGizmoInteract.h"
#include "CGizmoPivot.h"
#include "CGizmoNode.h"
#include "CGizmoWorkspace.h"

namespace App
{
	class CGizmo
	{
	public:
		static const u64 CMD_KEY_GIZMO_SELECT = Math::FNV1a_64(L"gizmo_select");
		static const u64 CMD_KEY_GIZMO_TRANSLATE = Math::FNV1a_64(L"gizmo_translate");
		static const u64 CMD_KEY_GIZMO_ROTATE = Math::FNV1a_64(L"gizmo_rotate");
		static const u64 CMD_KEY_GIZMO_SCALE = Math::FNV1a_64(L"gizmo_scale");
		static const u64 CMD_KEY_GIZMO_WORKSPACE = Math::FNV1a_64(L"gizmo_workspace");
		
		static const u64 CMD_KEY_GIZMO_SELECT_OFF = Math::FNV1a_64(L"gizmo_select_off");
		static const u64 CMD_KEY_GIZMO_TRANSLATE_OFF = Math::FNV1a_64(L"gizmo_translate_off");
		static const u64 CMD_KEY_GIZMO_ROTATE_OFF = Math::FNV1a_64(L"gizmo_rotate_off");
		static const u64 CMD_KEY_GIZMO_SCALE_OFF = Math::FNV1a_64(L"gizmo_scale_off");
		static const u64 CMD_KEY_GIZMO_WORKSPACE_OFF = Math::FNV1a_64(L"gizmo_workspace_off");

	public:
		enum class GizmoType
		{
			Select,
			Translate,
			Rotate,
			Scale,
			Workspace
		};

	public:
		CGizmo();
		~CGizmo();
		CGizmo(const CGizmo&) = delete;
		CGizmo(CGizmo&&) = delete;
		CGizmo& operator = (const CGizmo&) = delete;
		CGizmo& operator = (CGizmo&&) = delete;

		void Initialize();
		void Update();
		void LateUpdate();
		void Release();

		// Accessors.
		inline bool IsActive() const { return m_bActive; }
		
		inline GizmoType GetActiveType() const { return m_gizmoType; }

		CGizmoNode& Node() { return m_gizmoNode; }
		CGizmoWorkspace& Workspace() { return m_gizmoWorkspace; }
		CGizmoDraw& Draw() { return m_gizmoDraw; }
		CGizmoInteract& Interact() { return m_gizmoInteract; }

		// Modifiers.
		inline void SetActive(bool bActive) { m_bActive = bActive; }

	private:
		bool GizmoSelect();
		bool GizmoTranslate();
		bool GizmoRotate();
		bool GizmoScale();
		bool GizmoWorkspace();

		void OnCmdWorkspaceOff();

		void SwitchGizmo(GizmoType next);

	private:
		bool m_bActive;
		
		GizmoType m_gizmoType;

		CGizmoDraw m_gizmoDraw;
		CGizmoInteract m_gizmoInteract;
		CGizmoNode m_gizmoNode;
		CGizmoWorkspace m_gizmoWorkspace;
	};
};

#endif
