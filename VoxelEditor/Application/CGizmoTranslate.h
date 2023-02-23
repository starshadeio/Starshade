//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoTranslate.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMOTRANSLATE_H
#define CGIZMOTRANSLATE_H

#include <Globals/CGlobals.h>
#include <Math/CMathVector3.h>
#include <Math/CMathFNV.h>
#include <Math/CSIMDRay.h>

namespace App
{
	class CGizmoTranslate
	{
	public:
		static const u32 GIZMO_HASH_AXIS_X = Math::FNV1a_32("translate_axis_x");
		static const u32 GIZMO_HASH_AXIS_Y = Math::FNV1a_32("translate_axis_y");
		static const u32 GIZMO_HASH_AXIS_Z = Math::FNV1a_32("translate_axis_z");
		static const u32 GIZMO_HASH_PLANE_X = Math::FNV1a_32("translate_plane_x");
		static const u32 GIZMO_HASH_PLANE_Y = Math::FNV1a_32("translate_plane_y");
		static const u32 GIZMO_HASH_PLANE_Z = Math::FNV1a_32("translate_plane_z");

	public:
		struct Data
		{
			float snap = 0.125f;

			float scale = 80.0f;
			float coneSize = 0.175f;
			float planeSize = 0.2625f;

			float axisWidth = 2.0f;
			float axisHoveredWidth = 3.0f;
			float coneHoveredScale = 1.25f;

			class CGizmoPivot* pGizmoPivot = nullptr;
		};

	public:
		CGizmoTranslate();
		~CGizmoTranslate();
		CGizmoTranslate(const CGizmoTranslate&) = delete;
		CGizmoTranslate(CGizmoTranslate&&) = delete;
		CGizmoTranslate& operator = (const CGizmoTranslate&) = delete;
		CGizmoTranslate& operator = (CGizmoTranslate&&) = delete;

		void Interact();
		void Draw();

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void OnEnter(u32 hash);
		void OnExit(u32 hash);
		void OnPressed(u32 hash, const Math::CSIMDRay& ray, float t);
		void WhileHeld(u32 hash, const Math::CSIMDRay& ray);
		void OnReleased(u32 hash);

		bool TranslateAxis(const Math::SIMDVector& axis, const Math::CSIMDRay& ray, Math::SIMDVector& outPt);
		bool TranslatePlane(const Math::SIMDVector& normal, const Math::CSIMDRay& ray, Math::SIMDVector& outPt);

		bool CalculatePoint(u32 hash, const Math::CSIMDRay& ray, Math::SIMDVector& outPt);

	private:
		Data m_data;

		bool m_bHovered[6];
		bool m_bClicked[6];
		
		Math::SIMDVector m_clickPoint;
	};
};

#endif
