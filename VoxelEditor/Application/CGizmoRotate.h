//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoRotate.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMOROTATE_H
#define CGIZMOROTATE_H

#include <Globals/CGlobals.h>
#include <Math/CMathVector3.h>
#include <Math/CMathFNV.h>
#include <Math/CSIMDQuaternion.h>
#include <Math/CSIMDRay.h>

namespace App
{
	class CGizmoRotate
	{
	public:
		static const u32 GIZMO_HASH_AXIS_X = Math::FNV1a_32("rotate_axis_x");
		static const u32 GIZMO_HASH_AXIS_Y = Math::FNV1a_32("rotate_axis_y");
		static const u32 GIZMO_HASH_AXIS_Z = Math::FNV1a_32("rotate_axis_z");
		static const u32 GIZMO_HASH_EDGE = Math::FNV1a_32("rotate_edge");
		static const u32 GIZMO_HASH_CENTER = Math::FNV1a_32("rotate_center");

	public:
		struct Data
		{
			float snap = Math::g_Pi / 8;

			float scale = 80.0f;
			float sphereSize = 2.0f;
			float sphereOuterSize = 2.25f;
			
			u64 onReleaseHash;
			class IGizmoPivot* pGizmoPivot = nullptr;
		};

	public:
		CGizmoRotate();
		~CGizmoRotate();
		CGizmoRotate(const CGizmoRotate&) = delete;
		CGizmoRotate(CGizmoRotate&&) = delete;
		CGizmoRotate& operator = (const CGizmoRotate&) = delete;
		CGizmoRotate& operator = (CGizmoRotate&&) = delete;
		
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

	private:
		Data m_data;

		bool m_bHovered[5];
		bool m_bClicked[5];
		
		Math::SIMDVector m_viewDir;
		
		Math::SIMDVector m_clickPoint;
		Math::SIMDVector m_normal;
		Math::SIMDVector m_tangent;
		Math::SIMDVector m_bitangent;
		
		Math::SIMDQuaternion m_rotation;
	};
};

#endif
