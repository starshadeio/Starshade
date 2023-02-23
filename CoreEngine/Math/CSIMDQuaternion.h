//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CSIMDQuaternion.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSIMDQUATERNION_H
#define CSIMDQUATERNION_H

#include "CSIMDVector.h"
#include "../Globals/CGlobals.h"
#include "../Utilities/CMemAlign.h"

namespace Math
{
	struct SIMDQuaternion : public CMemAlign<16>
	{
		vf32 m_xmm;

		// Construct/Convert
		SIMDQuaternion() { }
		SIMDQuaternion(vf32 XMM) : m_xmm(XMM) { }
		explicit SIMDQuaternion(float f) { m_xmm = _mm_set_ps1(f); }
		SIMDQuaternion(float x, float y, float z, float w) { m_xmm = _mm_setr_ps(x, y, z, w); }

		inline const float* ToFloat() const { return m_xmm.m128_f32; }

		// Binary operators.
		SIMDQuaternion operator * (const SIMDQuaternion& q) const;
		SIMDVector operator * (const SIMDVector& v) const;

		SIMDQuaternion operator + (const SIMDQuaternion& q) const;
		SIMDQuaternion operator - (const SIMDQuaternion& q) const;
		SIMDQuaternion operator * (float f) const;
		SIMDQuaternion operator / (float f) const;

		// Assignment operators.
		SIMDQuaternion& operator *= (const SIMDQuaternion& q);

		SIMDQuaternion& operator += (const SIMDQuaternion& q);
		SIMDQuaternion& operator -= (const SIMDQuaternion& q);
		SIMDQuaternion& operator *= (float f);
		SIMDQuaternion& operator /= (float f);
		
		// Conditional operators.
		bool operator == (const SIMDQuaternion& v) const;
		bool operator != (const SIMDQuaternion& v) const;

		// Rotational methods.
		static SIMDQuaternion RotateDirection(const SIMDVector& forward, const SIMDVector& upward = Math::SIMD_VEC_UP);
		static SIMDQuaternion RotateX(float theta);
		static SIMDQuaternion RotateY(float theta);
		static SIMDQuaternion RotateZ(float theta);
		static SIMDQuaternion RotateAxis(const SIMDVector& axis, float theta);
		static SIMDQuaternion RotateEuler(const SIMDVector& euler);
		SIMDVector ToEuler() const;

		static SIMDQuaternion RotateX(const SIMDQuaternion& q, float theta);
		static SIMDQuaternion RotateY(const SIMDQuaternion& q, float theta);
		static SIMDQuaternion RotateZ(const SIMDQuaternion& q, float theta);
		static SIMDQuaternion RotateEuler(const SIMDQuaternion& q, const SIMDVector& euler);
		static SIMDVector ToEuler(const SIMDQuaternion& q);

		// General methods.
		SIMDQuaternion Normalize() const;
		SIMDQuaternion Conjugate() const;

		static SIMDQuaternion Normalize(const SIMDQuaternion& q);
		static SIMDQuaternion Conjugate(const SIMDQuaternion& q);

		// Product methods.
		void Concatenate(const SIMDQuaternion& q);
		vf32 Dot(const SIMDQuaternion& q) const;

		static SIMDQuaternion Concatenate(const SIMDQuaternion& q0, const SIMDQuaternion& q1);
		static vf32 Dot(const SIMDQuaternion& q0, const SIMDQuaternion& q1);

		// Interpolation methods.
		SIMDQuaternion Lerp(const SIMDQuaternion& to, float t) const;
		SIMDQuaternion Slerp(const SIMDQuaternion& to, float t) const;

		static SIMDQuaternion Lerp(const SIMDQuaternion& from, const SIMDQuaternion& to, float t);
		static SIMDQuaternion Slerp(const SIMDQuaternion& from, const SIMDQuaternion& to, float t);
	};

	// Constants.
	const SIMDQuaternion SIMD_QUAT_IDENTITY(0.0f, 0.0f, 0.0f, 1.0f);
};

#endif
