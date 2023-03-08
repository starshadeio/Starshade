//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoChunkGrid.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMOCHUNKGRID_H
#define CGIZMOCHUNKGRID_H

#include "CGizmoTranslate.h"
#include "../Universe/CChunkGrid.h"
#include <Globals/CGlobals.h>
#include <Math/CMathVector3.h>
#include <Math/CMathFNV.h>
#include <Math/CSIMDRay.h>

namespace App
{
	class CGizmoChunkGrid
	{
	public:
		static const u32 GIZMO_HASH_SIDE_LEFT = Math::FNV1a_32("workspace_side_left");
		static const u32 GIZMO_HASH_SIDE_RIGHT = Math::FNV1a_32("workspace_side_right");
		static const u32 GIZMO_HASH_SIDE_BOTTOM = Math::FNV1a_32("workspace_side_bottom");
		static const u32 GIZMO_HASH_SIDE_TOP = Math::FNV1a_32("workspace_side_top");
		static const u32 GIZMO_HASH_SIDE_BACK = Math::FNV1a_32("workspace_side_back");
		static const u32 GIZMO_HASH_SIDE_FRONT = Math::FNV1a_32("workspace_side_front");

	public:
		struct Data
		{
			float snap = 0.25f;
			
			float cubeSize = 0.15f;
			float cubeHoveredScale = 1.25f;

			class IGizmoPivot* pGizmoPivot = nullptr;
		};

	public:
		CGizmoChunkGrid();
		~CGizmoChunkGrid();
		CGizmoChunkGrid(const CGizmoChunkGrid&) = delete;
		CGizmoChunkGrid(CGizmoChunkGrid&&) = delete;
		CGizmoChunkGrid& operator = (const CGizmoChunkGrid&) = delete;
		CGizmoChunkGrid& operator = (CGizmoChunkGrid&&) = delete;
		
		void Initialize();
		void Interact();
		void Draw();
		
		void GatherExtents();

		// Accessors.
		inline const Math::Vector3& GetLastMinExtents() const { return m_lastMinExtents; }
		inline const Math::Vector3& GetLastMaxExtents() const { return m_lastMaxExtents; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetChunkGrid(Universe::CChunkGrid* pChunkGrid) { m_pChunkGrid = pChunkGrid; }

	private:
		void OnEnter(u32 hash);
		void OnExit(u32 hash);
		void OnPressed(u32 hash, const Math::CSIMDRay& ray, float t);
		void WhileHeld(u32 hash, const Math::CSIMDRay& ray);
		void OnReleased(u32 hash);
		
		bool TranslateAxis(const Math::SIMDVector& axis, const Math::CSIMDRay& ray, Math::SIMDVector& outPt);
		bool CalculatePoint(u32 hash, const Math::CSIMDRay& ray, Math::SIMDVector& outPt);

	private:
		Data m_data;

		bool m_bHovered[6];
		bool m_bClicked[6];
		
		Math::Vector3 m_minExtents;
		Math::Vector3 m_maxExtents;
		Math::Vector3 m_lastMinExtents;
		Math::Vector3 m_lastMaxExtents;
		Math::SIMDVector m_clickPoint;

		CGizmoTranslate m_gizmoTranslate;
		
		Universe::CChunkGrid* m_pChunkGrid;
	};
};

#endif
