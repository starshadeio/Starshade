//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CSIMDPlane.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSIMDPLANE_H
#define CSIMDPLANE_H

#include "CSIMDVector.h"
#include "../Globals/CGlobals.h"
#include "../Utilities/CMemAlign.h"

namespace Math
{
	struct SIMDPlane : CMemAlign<16>
	{
		vf32 m_xmm;

		// Construct/Convert.
		SIMDPlane() { }
		SIMDPlane(vf32 XMM) : m_xmm(XMM) { }
		SIMDPlane(float f) { m_xmm = _mm_set_ps1(f); }
		SIMDPlane(float a, float b, float c, float d) { m_xmm = _mm_setr_ps(a, b, c, d); }
		SIMDPlane(const SIMDVector& point, const SIMDVector& normal)
		{
			FromPointNormal(point, normal);
		}

		SIMDPlane(const SIMDVector& a0, const SIMDVector& b0, const SIMDVector& c0)
		{
			FromPoints(a0, b0, c0);
		}

		inline const float* ToFloat() const { return m_xmm.m128_f32; }
		inline float operator [] (size_t index) const { return m_xmm.m128_f32[index]; }

		// Binary operators.
		SIMDPlane operator + (const SIMDPlane& p) const;
		SIMDPlane operator - (const SIMDPlane& p) const;
		SIMDPlane operator + (float f) const;
		SIMDPlane operator - (float f) const;
		SIMDPlane operator * (float f) const;
		SIMDPlane operator / (float f) const;

		// Assignment operators.
		SIMDPlane& operator += (const SIMDPlane& p);
		SIMDPlane& operator -= (const SIMDPlane& p);
		SIMDPlane& operator += (float f);
		SIMDPlane& operator -= (float f);
		SIMDPlane& operator *= (float f);
		SIMDPlane& operator /= (float f);

		// Conditional operators.
		vf32 operator == (const SIMDPlane& p) const;
		vf32 operator != (const SIMDPlane& p) const;

		// General methods.
		static vf32 LengthSq(const SIMDPlane& p);
		static vf32 Length(const SIMDPlane& p);

		inline vf32 LengthSq() const
		{
			vf32 a = _mm_mul_ps(m_xmm, m_xmm);
			vf32 b = _mm_hadd_ps(a, a);
			return _mm_hadd_ps(b, b);
		}

		inline vf32 Length() const
		{
			return _mm_sqrt_ps(LengthSq());
		}

		inline void Normalize()
		{
			m_xmm = _mm_div_ps(m_xmm, Length());
		}

		inline SIMDPlane Normalized() const
		{
			return SIMDPlane(_mm_div_ps(m_xmm, Length()));
		}

		inline SIMDVector Normal() const
		{
			return _mm_mul_ps(m_xmm, _mm_setr_ps(1.0f, 1.0f, 1.0f, 0.0f));
		}

		static void Normalize(SIMDPlane& p);
		static SIMDPlane Normalized(const SIMDPlane& p);
		static SIMDVector Normal(const SIMDPlane& p);

		// Product methods.
		static vf32 Dot(const SIMDPlane& p, const SIMDVector& v);
		static vf32 DotCoord(const SIMDPlane& p, const SIMDVector& v);
		static vf32 DotNormal(const SIMDPlane& p, const SIMDVector& v);

		inline vf32 Dot(const SIMDVector& v) const
		{
			vf32 a = _mm_mul_ps(m_xmm, v.m_xmm);
			a = _mm_hadd_ps(a, a);
			return _mm_hadd_ps(a, a);
		}

		inline vf32 DotCoord(const SIMDVector& v) const
		{
			vf32 a = _mm_mul_ps(_mm_mul_ps(m_xmm, _mm_setr_ps(1.0f, 1.0f, 1.0f, 0.0f)), v.m_xmm);
			a = _mm_hadd_ps(a, a);
			return _mm_add_ps(_mm_hadd_ps(a, a), _mm_shuffle_ps(m_xmm, m_xmm, _MM_SHUFFLE(3, 3, 3, 3)));
		}

		inline vf32 DotNormal(const SIMDVector& v) const
		{
			vf32 a = _mm_mul_ps(_mm_mul_ps(m_xmm, _mm_setr_ps(1.0f, 1.0f, 1.0f, 0.0f)), v.m_xmm);
			a = _mm_hadd_ps(a, a);
			return _mm_hadd_ps(a, a);
		}

		// Conversion methods.
		static void FromPointNormal(SIMDPlane& p, const SIMDVector& point, const SIMDVector& normal);
		static void FromPoints(SIMDPlane& p, const SIMDVector& a0, const SIMDVector& b0, const SIMDVector& c0);

		inline void FromPointNormal(const SIMDVector& point, const SIMDVector& normal)
		{
			m_xmm = _mm_add_ps(_mm_setr_ps(0.0f, 0.0f, 0.0f, _mm_cvtss_f32(SIMDVector::Dot(point, normal))), normal.m_xmm);
		}

		inline void FromPoints(const SIMDVector& a, const SIMDVector& b, const SIMDVector& c)
		{
			SIMDVector ab = b - a;
			SIMDVector ac = c - a;
			SIMDVector abXac = SIMDVector::PointwiseQuotient(SIMDVector::Cross(ab, ac), SIMD_VEC_ONE);
			m_xmm = (abXac - _mm_setr_ps(0.0f, 0.0f, 0.0f, _mm_cvtss_f32(SIMDVector::Dot(abXac, a)))).m_xmm;
		}
	};
};

#endif
