//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoWorkspace.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMOWORKSPACE_H
#define CGIZMOWORKSPACE_H

#include "CGizmoChunkGrid.h"
#include "CGizmoPivot_.h"
#include "../Universe/CChunkGrid.h"

namespace App
{
	class CGizmoWorkspace
	{
	public:
		static const u64 CMD_KEY_TRANSLATE = Math::FNV1a_64(L"workspace_translate");
		static const u64 CMD_KEY_EXTENTS = Math::FNV1a_64(L"workspace_extents");

	public:
		CGizmoWorkspace();
		~CGizmoWorkspace();
		CGizmoWorkspace(const CGizmoWorkspace&) = delete;
		CGizmoWorkspace(CGizmoWorkspace&&) = delete;
		CGizmoWorkspace& operator = (const CGizmoWorkspace&) = delete;
		CGizmoWorkspace& operator = (CGizmoWorkspace&&) = delete;

		void Initialize();
		void Update();
		void LateUpdate();
		void Release();

		// Modifier.
		inline void SetChunkGrid(Universe::CChunkGrid* pChunkGrid) { m_pChunkGrid = pChunkGrid; m_gizmoChunkGrid.SetChunkGrid(m_pChunkGrid); }

	private:
		bool Translate(const void* param, bool bInverse);
		bool Extents(const void* param, bool bInverse);

		const Math::SIMDVector& GetPosition();
		const Math::SIMDQuaternion& GetRotation();
		const Math::SIMDVector& GetScale();
		
		Math::SIMDVector GetRight();
		Math::SIMDVector GetUp();
		Math::SIMDVector GetForward();

		void SetPosition(const Math::SIMDVector& position);
		void SetRotation(const Math::SIMDQuaternion& rotation);
		void SetScale(const Math::SIMDVector& scale);

	private:
		CGizmoChunkGrid m_gizmoChunkGrid;
		CGizmoPivot_ m_gizmoPivot;

		Universe::CChunkGrid* m_pChunkGrid;
	};
};

#endif
