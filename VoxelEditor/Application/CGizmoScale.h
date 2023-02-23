//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoScale.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMOSCALE_H
#define CGIZMOSCALE_H

#include <Globals/CGlobals.h>
#include <Math/CMathVector3.h>
#include <Math/CMathFNV.h>
#include <Math/CSIMDRay.h>

namespace App
{
	class CGizmoScale
	{
	public:
		static const u32 GIZMO_HASH_AXIS_X = Math::FNV1a_32("scale_axis_x");
		static const u32 GIZMO_HASH_AXIS_Y = Math::FNV1a_32("scale_axis_y");
		static const u32 GIZMO_HASH_AXIS_Z = Math::FNV1a_32("scale_axis_z");
		static const u32 GIZMO_HASH_CENTER = Math::FNV1a_32("scale_center");

	public:
		struct Data
		{
			float snap = 1.0f;

			float scale = 80.0f;
			float cubeSize = 0.15f;
			float centerCubeSize = 0.2f;

			float axisWidth = 2.0f;
			float axisHoveredWidth = 3.0f;
			float cubeHoveredScale = 1.125f;

			class CGizmoPivot* pGizmoPivot = nullptr;
		};

	public:
		CGizmoScale();
		~CGizmoScale();
		CGizmoScale(const CGizmoScale&) = delete;
		CGizmoScale(CGizmoScale&&) = delete;
		CGizmoScale& operator = (const CGizmoScale&) = delete;
		CGizmoScale& operator = (CGizmoScale&&) = delete;
		
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

		bool m_bHovered[4];
		bool m_bClicked[4];

		Math::Vector3 m_startScale;
		Math::Vector3 m_scale;
		Math::SIMDVector m_scaleNormal;
		Math::SIMDVector m_scaleTangent;
		Math::SIMDVector m_scaleBitangent;
		
		Math::SIMDVector m_clickPoint;
	};
};

#endif
