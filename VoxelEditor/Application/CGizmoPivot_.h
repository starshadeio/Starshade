//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoPivot_.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMOPIVOT__H
#define CGIZMOPIVOT__H

#include "IGizmoPivot.h"
#include "CGizmoTranslate.h"
#include "CGizmoRotate.h"
#include "CGizmoScale.h"
#include <Objects/CVObject.h>
#include <Logic/CTransform.h>
#include <functional>

namespace App
{
	class CGizmoPivot_ : public IGizmoPivot
	{
	public:
		struct Data
		{
			float scale = 80.0f;
		};

		struct Callbacks
		{
			std::function<const Math::SIMDVector&()> getPosition;
			std::function<const Math::SIMDQuaternion&()> getRotation;
			std::function<const Math::SIMDVector&()> getScale;

			std::function<const Math::SIMDVector()> getRight;
			std::function<const Math::SIMDVector()> getUp;
			std::function<const Math::SIMDVector()> getForward;
			
			std::function<void(const Math::SIMDVector&)> setPosition;
			std::function<void(const Math::SIMDQuaternion&)> setRotation;
			std::function<void(const Math::SIMDVector&)> setScale;
		};

	public:
		CGizmoPivot_();
		~CGizmoPivot_();
		CGizmoPivot_(const CGizmoPivot_&) = delete;
		CGizmoPivot_(CGizmoPivot_&&) = delete;
		CGizmoPivot_& operator = (const CGizmoPivot_&) = delete;
		CGizmoPivot_& operator = (CGizmoPivot_&&) = delete;

		void Update() final;
		void LateUpdate() final;

		// Accessors.
		inline CVObject* GetObject() const { return m_pObject; }

		inline float GetScreenScale() const final { return m_screenScale; }
		inline float GetStartScale() const final { return m_startScale; }

		inline const Math::SIMDVector& GetPosition() const final { return m_pObject ? m_pObject->GetTransform()->GetPosition() : m_callbacks.getPosition(); }
		inline const Math::SIMDVector& GetLastPosition() const final { return m_lastPosition; }
		inline const Math::SIMDQuaternion& GetRotation() const final { return m_pObject ? m_pObject->GetTransform()->GetRotation() : m_callbacks.getRotation(); }
		inline const Math::SIMDQuaternion& GetLastRotation() const final { return m_lastRotation; }
		inline const Math::SIMDVector& GetScale() const final { return m_pObject ? m_pObject->GetTransform()->GetLocalScale() : m_callbacks.getScale(); }
		inline const Math::SIMDVector& GetLastScale() const final { return m_lastScale; }

		inline const Math::SIMDVector& GetRight() const final { return m_right; }
		inline const Math::SIMDVector& GetUp() const final { return m_up; }
		inline const Math::SIMDVector& GetForward() const final { return m_forward; }

		inline const Math::SIMDVector GetLocalRight() const final { return m_rightLocal; }
		inline const Math::SIMDVector GetLocalUp() const final { return m_upLocal; }
		inline const Math::SIMDVector GetLocalForward() const final { return m_forwardLocal; }
		
		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetCallbacks(const Callbacks& callbacks) { m_callbacks = callbacks; }
		inline void SetObject(CVObject* pObject) { m_pObject = pObject; }

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

		Callbacks m_callbacks;

		CVObject* m_pObject;
	};
};

#endif
