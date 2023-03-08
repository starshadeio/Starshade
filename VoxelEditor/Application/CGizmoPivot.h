//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoPivot.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMOPIVOT_H
#define CGIZMOPIVOT_H

#include "IGizmoPivot.h"
#include "CGizmoTranslate.h"
#include "CGizmoRotate.h"
#include "CGizmoScale.h"
#include "CNodeObjectEx.h"
#include "CNodeEx.h"
#include <Logic/CNodeTransform.h>
#include <Graphics/CPrimCube.h>
#include <Math/CMathFNV.h>

namespace App
{
	class CGizmoPivot : public IGizmoPivot
	{
	public:
		struct Data
		{
			float scale = 80.0f;
		};

	public:
		CGizmoPivot();
		~CGizmoPivot();
		CGizmoPivot(const CGizmoPivot&) = delete;
		CGizmoPivot(CGizmoPivot&&) = delete;
		CGizmoPivot& operator = (const CGizmoPivot&) = delete;
		CGizmoPivot& operator = (CGizmoPivot&&) = delete;

		void Update() final;
		void LateUpdate() final;

		// Accessors.
		inline CNodeEx* GetNode() { return m_pNode; }

		inline float GetScreenScale() const final { return m_screenScale; }
		inline float GetStartScale() const final { return m_startScale; }

		inline const Math::SIMDVector& GetPosition() const final { return m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetPosition(); }
		inline const Math::SIMDVector& GetLastPosition() const final { return m_lastPosition; }
		inline const Math::SIMDQuaternion& GetRotation() const final { return m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetRotation(); }
		inline const Math::SIMDQuaternion& GetLastRotation() const final { return m_lastRotation; }
		inline const Math::SIMDVector& GetScale() const final { return m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetLocalScale(); }
		inline const Math::SIMDVector& GetLastScale() const final { return m_lastScale; }

		inline const Math::SIMDVector& GetRight() const final { return m_right; }
		inline const Math::SIMDVector& GetUp() const final { return m_up; }
		inline const Math::SIMDVector& GetForward() const final { return m_forward; }

		inline const Math::SIMDVector GetLocalRight() const final { return m_rightLocal; }
		inline const Math::SIMDVector GetLocalUp() const final { return m_upLocal; }
		inline const Math::SIMDVector GetLocalForward() const final { return m_forwardLocal; }
		
		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetNode(CNodeEx* pNode) { m_pNode = pNode; }

		void SetPosition(const Math::SIMDVector& position, bool bSetLast = false) final;
		void SetRotation(const Math::SIMDQuaternion& rotation, bool bSetLast = false) final;
		void SetScale(const Math::SIMDVector& scale, bool bSetLast = false) final;
		
	public:
		void GatherStart() final;
		void CalculateScreenScale() final;
		float CalculateDistanceScale(const Math::SIMDVector& pt) final;

	private:
		Data m_data;

		float m_screenScale;
		float m_startScale;
		
		Math::SIMDVector m_lastPosition;
		Math::SIMDQuaternion m_lastRotation;
		Math::SIMDVector m_lastScale;

		Math::SIMDVector m_right;
		Math::SIMDVector m_up;
		Math::SIMDVector m_forward;
		Math::SIMDVector m_rightLocal;
		Math::SIMDVector m_upLocal;
		Math::SIMDVector m_forwardLocal;

		class CNodeEx* m_pNode;
	};
};

#endif
