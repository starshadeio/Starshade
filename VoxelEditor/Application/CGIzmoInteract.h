//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoInteract.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMOINTERACT_H
#define CGIZMOINTERACT_H

#include "CGizmoData.h"
#include <Math/CSIMDRay.h>
#include <queue>

namespace App
{
	class CGizmoInteract
	{
	private:
		class QueueComp
		{
		public:
			bool operator() (const GizmoInteract& a, const GizmoInteract& b)
			{
				return a.priority > b.priority;
			}
		};

	public:
		CGizmoInteract();
		~CGizmoInteract();
		CGizmoInteract(const CGizmoInteract&) = delete;
		CGizmoInteract(CGizmoInteract&&) = delete;
		CGizmoInteract& operator = (const CGizmoInteract&) = delete;
		CGizmoInteract& operator = (CGizmoInteract&&) = delete;

		void Initialize();
		void Update();

		void AddInteractable(const GizmoInteract& interactData);

		static bool EdgePoint(const Math::SIMDVector& pt, const Math::SIMDVector& dir, const Math::CSIMDRay& ray, float& t);
		static bool PlanePoint(const Math::SIMDVector& pt, const Math::SIMDVector& normal, const Math::CSIMDRay& ray, float& t);

		// Accessors.
		inline bool IsHovered() const { return m_bLastIntersected; }

	private:
		void OnCmdSelectPressed();
		void OnCmdSelectReleased();

		bool IntersectRayCylinder(const Math::CSIMDRay& ray, const Math::SIMDVector& p, const Math::SIMDVector& q, float r, float &tMax);
		bool IntersectSegmentCylinder(const Math::SIMDVector& sa, const Math::SIMDVector& sb, const Math::SIMDVector& p, const Math::SIMDVector& q, float r, float &t);

		bool TestLine(const GizmoInteractLine& line, const Math::CSIMDRay& ray, float& tMax);
		bool TestQuad(const GizmoInteractQuad& quad, const Math::CSIMDRay& ray, float& tMax);
		bool TestCircle(const GizmoInteractCircle& circle, const Math::CSIMDRay& ray, float& tMax);
		bool TestSphere(const GizmoInteractSphere& sphere, const Math::CSIMDRay& ray, float& tMax);
		bool TestCube(const GizmoInteractCube& cube, const Math::CSIMDRay& ray, float& tMax);

	private:
		bool m_bHeld;
		bool m_bLastIntersected;
		float m_lastT;
		Math::CSIMDRay m_lastRay;
		GizmoInteract m_lastIntersected;
		std::priority_queue<GizmoInteract, std::vector<GizmoInteract>, QueueComp> m_interactQueue;
	};
};

#endif
