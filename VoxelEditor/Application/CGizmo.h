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

namespace App
{
	class CGizmo
	{
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

		CGizmoPivot& Pivot() { return m_gizmoPivot; }
		CGizmoDraw& Draw() { return m_gizmoDraw; }
		CGizmoInteract& Interact() { return m_gizmoInteract; }

		// Modifiers.
		inline void SetActive(bool bActive) { m_bActive = bActive; }

	private:
		bool m_bActive;

		CGizmoDraw m_gizmoDraw;
		CGizmoInteract m_gizmoInteract;
		CGizmoPivot m_gizmoPivot;
	};
};

#endif
