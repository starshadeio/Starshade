//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CSIMDVector.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSIMDVECTOR_H
#define CSIMDVECTOR_H

#include "../Globals/CGlobals.h"
#include "../Utilities/CMemAlign.h"

namespace Math
{
	struct SIMDVector : CMemAlign<16>
	{
		vf32 m_xmm;

		// Construct/Convert.
		SIMDVector() { }
		SIMDVector(float f) : m_xmm(_mm_setr_ps(f, f, f, 0.0f)) { }
		SIMDVector(vf32 xmm) : m_xmm(xmm) { }
		SIMDVector(float x, float y, float z) : m_xmm(_mm_setr_ps(x, y, z, 0.0f)) { }

		inline const float* ToFloat() const { return m_xmm.m128_f32; }
		inline float operator [] (size_t index) const { return m_xmm.m128_f32[index]; }

		// Unary operators.
		SIMDVector operator + () const;
		SIMDVector operator - () const;

		// Binary operators.
		SIMDVector operator + (const SIMDVector& v) const;
		SIMDVector operator + (float f) const;

		SIMDVector operator - (const SIMDVector& v) const;
		SIMDVector operator - (float f) const;

		SIMDVector operator * (float f) const;
		SIMDVector operator / (float f) const;

		// Assignment operators.
		SIMDVector& operator += (const SIMDVector& v);
		SIMDVector& operator += (float f);

		SIMDVector& operator -= (const SIMDVector& v);
		SIMDVector& operator -= (float f);

		SIMDVector& operator *= (float f);
		SIMDVector& operator /= (float f);

		// Conditional operators.
		bool operator == (const SIMDVector& v) const;
		bool operator != (const SIMDVector& v) const;

		// General methods.
		vf32 LengthSq() const;
		vf32 Length() const;
		void Normalize();
		SIMDVector Normalized() const;

		static vf32 LengthSq(const SIMDVector& v);
		static vf32 Length(const SIMDVector& v);
		static void Normalize(SIMDVector& v);
		static SIMDVector Normalized(const SIMDVector& v);

		// Product methods.
		SIMDVector PointwiseProduct(const SIMDVector& v) const;
		SIMDVector PointwiseQuotient(const SIMDVector& v) const;
		vf32 Dot(const SIMDVector& v) const;
		SIMDVector Cross(const SIMDVector& v) const;

		static SIMDVector PointwiseProduct(const SIMDVector& v0, const SIMDVector& v1);
		static SIMDVector PointwiseQuotient(const SIMDVector& v0, const SIMDVector& v1);
		static vf32 Dot(const SIMDVector& v0, const SIMDVector& v1);
		static SIMDVector Cross(const SIMDVector& v0, const SIMDVector& v1);

		// Interpolation methods.
		SIMDVector Lerp(const SIMDVector& to, const SIMDVector& t) const;
		SIMDVector Lerp(const SIMDVector& to, float t) const;
		SIMDVector MoveTowards(const SIMDVector& to, float t) const;
		
		static SIMDVector Lerp(const SIMDVector& from, const SIMDVector& to, const SIMDVector& t);
		static SIMDVector Lerp(const SIMDVector& from, const SIMDVector& to, float t);
		static SIMDVector MoveTowards(const SIMDVector& from, const SIMDVector& to, float t);
	};

	// Constants.
	const SIMDVector SIMD_VEC_ZERO(0.0f);
	const SIMDVector SIMD_VEC_ONE(1.0f);

	const SIMDVector SIMD_VEC_LEFT(-1.0f, 0.0f, 0.0f);
	const SIMDVector SIMD_VEC_RIGHT(1.0f, 0.0f, 0.0f);
	const SIMDVector SIMD_VEC_UP(0.0f, 1.0f, 0.0f);
	const SIMDVector SIMD_VEC_DOWN(0.0f, -1.0f, 0.0f);
	const SIMDVector SIMD_VEC_FORWARD(0.0f, 0.0f, 1.0f);
	const SIMDVector SIMD_VEC_BACKWARD(0.0f, 0.0f, -1.0f);
};

#endif
