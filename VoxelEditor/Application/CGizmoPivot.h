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
	class CGizmoPivot
	{
	public:
		static const u64 CMD_KEY_GIZMO_SELECT = Math::FNV1a_64(L"gizmo_select");
		static const u64 CMD_KEY_GIZMO_TRANSLATE = Math::FNV1a_64(L"gizmo_translate");
		static const u64 CMD_KEY_GIZMO_ROTATE = Math::FNV1a_64(L"gizmo_rotate");
		static const u64 CMD_KEY_GIZMO_SCALE = Math::FNV1a_64(L"gizmo_scale");

		static const u64 CMD_KEY_TRANSLATE = Math::FNV1a_64(L"translate");
		static const u64 CMD_KEY_ROTATE = Math::FNV1a_64(L"rotate");
		static const u64 CMD_KEY_SCALE = Math::FNV1a_64(L"scale");

	private:
		static const u64 CMD_KEY_GIZMO_SELECT_OFF = Math::FNV1a_64(L"gizmo_select_off");
		static const u64 CMD_KEY_GIZMO_TRANSLATE_OFF = Math::FNV1a_64(L"gizmo_translate_off");
		static const u64 CMD_KEY_GIZMO_ROTATE_OFF = Math::FNV1a_64(L"gizmo_rotate_off");
		static const u64 CMD_KEY_GIZMO_SCALE_OFF = Math::FNV1a_64(L"gizmo_scale_off");

	public:
		enum class GizmoType
		{
			Select,
			Translate,
			Rotate,
			Scale,
		};

	public:
		struct Data
		{
			float scale = 80.0f;
			float coneSize = 0.175f;
			float planeSize = 0.2625f;

			float axisWidth = 2.0f;
			float axisHoveredWidth = 3.0f;
			float coneHoveredScale = 1.25f;
		};

	public:
		CGizmoPivot();
		~CGizmoPivot();
		CGizmoPivot(const CGizmoPivot&) = delete;
		CGizmoPivot(CGizmoPivot&&) = delete;
		CGizmoPivot& operator = (const CGizmoPivot&) = delete;
		CGizmoPivot& operator = (CGizmoPivot&&) = delete;

		void Initialize();
		void Update();
		void LateUpdate();
		void Release();

		// Accessors.
		inline const Data& GetData() const { return m_data; }

		inline GizmoType GetActiveType() const { return m_gizmoType; }

		inline float GetScreenScale() const { return m_screenScale; }
		inline float GetStartScale() const { return m_startScale; }

		inline const Math::SIMDVector& GetPosition() const { return m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetPosition(); }
		inline const Math::SIMDVector& GetLastPosition() const { return m_lastPosition; }
		inline const Math::SIMDQuaternion& GetRotation() const { return m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetRotation(); }
		inline const Math::SIMDQuaternion& GetLastRotation() const { return m_lastRotation; }
		inline const Math::SIMDVector& GetScale() const { return m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetLocalScale(); }
		inline const Math::SIMDVector& GetLastScale() const { return m_lastScale; }

		inline const Math::SIMDVector& GetRight() const { return m_right; }
		inline const Math::SIMDVector& GetUp() const { return m_up; }
		inline const Math::SIMDVector& GetForward() const { return m_forward; }

		inline const Math::SIMDVector GetLocalRight() const { return m_rightLocal; }
		inline const Math::SIMDVector GetLocalUp() const { return m_upLocal; }
		inline const Math::SIMDVector GetLocalForward() const { return m_forwardLocal; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetNode(CNodeEx* pNode) { m_pNode = pNode; }

		inline void SetPosition(const Math::SIMDVector& position) { m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->SetPosition(position); }
		inline void SetRotation(const Math::SIMDQuaternion& rotation) { m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->SetRotation(rotation); }
		inline void SetScale(const Math::SIMDVector& scale) { m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->SetLocalScale(scale); }
		
	private:
		bool GizmoSelect();
		bool GizmoTranslate();
		bool GizmoRotate();
		bool GizmoScale();

		bool Translate(const void* param, bool bInverse);
		bool Rotate(const void* param, bool bInverse);
		bool Scale(const void* param, bool bInverse);

		void SwitchGizmo(GizmoType next);
		
	public:
		void GatherStart();
		void CalculateScreenScale();
		float CalculateDistanceScale(const Math::SIMDVector& pt);

	private:
		Data m_data;
		
		float m_screenScale;
		GizmoType m_gizmoType;
		
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

		CGizmoTranslate m_gizmoTranslate;
		CGizmoRotate m_gizmoRotate;
		CGizmoScale m_gizmoScale;

		class CNodeEx* m_pNode;
	};
};

#endif
