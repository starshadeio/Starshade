//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmo.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmo.h"

namespace App
{
	CGizmo::CGizmo() : 
		m_bActive(true)
	{
	}

	CGizmo::~CGizmo()
	{
	}

	void CGizmo::Initialize()
	{
		m_gizmoPivot.Initialize();
		m_gizmoInteract.Initialize();
		m_gizmoDraw.Initialize();
	}

	void CGizmo::Update()
	{
		if(!m_bActive) { return; }

		m_gizmoPivot.Update();

		// Interaction update should always come after all other gizmo updates.
		m_gizmoInteract.Update();
	}

	void CGizmo::LateUpdate()
	{
		if(!m_bActive) { return; }

		m_gizmoPivot.LateUpdate();
	}

	void CGizmo::Release()
	{
		m_gizmoDraw.Release();
		m_gizmoPivot.Release();
	}
};
