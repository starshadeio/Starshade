//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/IGizmoPivot.h
//
//-------------------------------------------------------------------------------------------------

#ifndef IGIZMOPIVOT_H
#define IGIZMOPIVOT_H

#include "CGizmoTranslate.h"
#include "CGizmoRotate.h"
#include "CGizmoScale.h"

namespace App
{
	class IGizmoPivot
	{
	public:
		IGizmoPivot() { }
		virtual ~IGizmoPivot() { }
		IGizmoPivot(const IGizmoPivot&) = delete;
		IGizmoPivot(IGizmoPivot&&) = delete;
		IGizmoPivot& operator = (const IGizmoPivot&) = delete;
		IGizmoPivot& operator = (IGizmoPivot&&) = delete;

		virtual void Update() = 0;
		virtual void LateUpdate() = 0;

		// Accessors.
		virtual inline float GetScreenScale() const = 0;
		virtual inline float GetStartScale() const = 0;

		virtual inline const Math::SIMDVector& GetPosition() const = 0;
		virtual inline const Math::SIMDVector& GetLastPosition() const = 0;
		virtual inline const Math::SIMDQuaternion& GetRotation() const = 0;
		virtual inline const Math::SIMDQuaternion& GetLastRotation() const = 0;
		virtual inline const Math::SIMDVector& GetScale() const = 0;
		virtual inline const Math::SIMDVector& GetLastScale() const = 0;

		virtual inline const Math::SIMDVector& GetRight() const = 0;
		virtual inline const Math::SIMDVector& GetUp() const = 0;
		virtual inline const Math::SIMDVector& GetForward() const = 0;

		virtual inline const Math::SIMDVector GetLocalRight() const = 0;
		virtual inline const Math::SIMDVector GetLocalUp() const = 0;
		virtual inline const Math::SIMDVector GetLocalForward() const = 0;
		
		// Modifiers.
		virtual void SetPosition(const Math::SIMDVector& position, bool bSetLast = false) = 0;
		virtual void SetRotation(const Math::SIMDQuaternion& rotation, bool bSetLast = false) = 0;
		virtual void SetScale(const Math::SIMDVector& scale, bool bSetLast = false) = 0;
		
		virtual void GatherStart() = 0;
		virtual void CalculateScreenScale() = 0;
		virtual float CalculateDistanceScale(const Math::SIMDVector& pt) = 0;

	private:
	};
};

#endif
