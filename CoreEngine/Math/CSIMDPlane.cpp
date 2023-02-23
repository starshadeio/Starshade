//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CSIMDPlane.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSIMDPlane.h"
#include "CMathFloat.h"

namespace Math
{
	// Arithmetic operators.
	SIMDPlane SIMDPlane::operator + (const SIMDPlane& p) const
	{
		return _mm_add_ps(m_xmm, p.m_xmm);
	}

	SIMDPlane SIMDPlane::operator - (const SIMDPlane& p) const
	{
		return _mm_sub_ps(m_xmm, p.m_xmm);
	}

	SIMDPlane SIMDPlane::operator + (float f) const
	{
		return _mm_add_ps(m_xmm, _mm_set_ps1(f));
	}

	SIMDPlane SIMDPlane::operator - (float f) const
	{
		return _mm_sub_ps(m_xmm, _mm_set_ps1(f));
	}

	SIMDPlane SIMDPlane::operator * (float f) const
	{
		return _mm_mul_ps(m_xmm, _mm_set_ps1(f));
	}

	SIMDPlane SIMDPlane::operator / (float f) const
	{
		return _mm_div_ps(m_xmm, _mm_set_ps1(f));
	}

	// Assignment operators.
	SIMDPlane& SIMDPlane::operator += (const SIMDPlane& p)
	{
		m_xmm = _mm_add_ps(m_xmm, p.m_xmm);
		return *this;
	}

	SIMDPlane& SIMDPlane::operator -= (const SIMDPlane& p)
	{
		m_xmm = _mm_sub_ps(m_xmm, p.m_xmm);
		return *this;
	}

	SIMDPlane& SIMDPlane::operator += (float f)
	{
		m_xmm = _mm_add_ps(m_xmm, _mm_set_ps1(f));
		return *this;
	}

	SIMDPlane& SIMDPlane::operator -= (float f)
	{
		m_xmm = _mm_sub_ps(m_xmm, _mm_set_ps1(f));
		return *this;
	}

	SIMDPlane& SIMDPlane::operator *= (float f)
	{
		m_xmm = _mm_mul_ps(m_xmm, _mm_set_ps1(f));
		return *this;
	}

	SIMDPlane& SIMDPlane::operator /= (float f)
	{
		m_xmm = _mm_div_ps(m_xmm, _mm_set_ps1(f));
		return *this;
	}

	//
	// Comparison operators.
	//

	vf32 SIMDPlane::operator == (const SIMDPlane& p) const
	{
		return _mm_cmpeq_ps(m_xmm, p.m_xmm);
	}

	vf32 SIMDPlane::operator != (const SIMDPlane& p) const
	{
		return _mm_cmpneq_ps(m_xmm, p.m_xmm);
	}

	//
	// Static methods.
	//

	vf32 SIMDPlane::LengthSq(const SIMDPlane& p)
	{
		return p.LengthSq();
	}

	vf32 SIMDPlane::Length(const SIMDPlane& p)
	{
		return p.Length();
	}

	void SIMDPlane::Normalize(SIMDPlane& p)
	{
		p.Normalize();
	}

	SIMDPlane SIMDPlane::Normalized(const SIMDPlane& p)
	{
		return p.Normalized();
	}

	SIMDVector SIMDPlane::Normal(const SIMDPlane& p)
	{
		return p.Normal();
	}

	vf32 SIMDPlane::Dot(const SIMDPlane& p, const SIMDVector& v)
	{
		return p.Dot(v);
	}

	vf32 SIMDPlane::DotCoord(const SIMDPlane& p, const SIMDVector& v)
	{
		return p.DotCoord(v);
	}

	vf32 SIMDPlane::DotNormal(const SIMDPlane& p, const SIMDVector& v)
	{
		return p.DotCoord(v);
	}

	void SIMDPlane::FromPointNormal(SIMDPlane& p, const SIMDVector& point, const SIMDVector& normal)
	{
		return p.FromPointNormal(point, normal);
	}

	void SIMDPlane::FromPoints(SIMDPlane& p, const SIMDVector& a, const SIMDVector& b, const SIMDVector& c)
	{
		p.FromPoints(a, b, c);
	}
};
