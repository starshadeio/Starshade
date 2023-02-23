//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CSIMDVector.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSIMDVector.h"
#include <cmath>

namespace Math
{
	// Unary operators.
	SIMDVector SIMDVector::operator + () const
	{
		return *this;
	}

	SIMDVector SIMDVector::operator - () const
	{
		return _mm_xor_ps(m_xmm, _mm_setr_ps(-0.0f, -0.0f, -0.0f, 0.0f));
	}

	// Binary operators.
	SIMDVector SIMDVector::operator + (const SIMDVector& v) const
	{
		return _mm_add_ps(m_xmm, v.m_xmm);
	}

	SIMDVector SIMDVector::operator + (float f) const
	{
		return _mm_add_ps(m_xmm, _mm_setr_ps(f, f, f, 0.0f));
	}

	SIMDVector SIMDVector::operator - (const SIMDVector& v) const
	{
		return _mm_sub_ps(m_xmm, v.m_xmm);
	}

	SIMDVector SIMDVector::operator - (float f) const
	{
		return _mm_sub_ps(m_xmm, _mm_setr_ps(f, f, f, 0.0f));
	}

	SIMDVector SIMDVector::operator * (float f) const
	{
		return _mm_mul_ps(m_xmm, _mm_setr_ps(f, f, f, 1.0f));
	}

	SIMDVector SIMDVector::operator / (float f) const
	{
		return _mm_div_ps(m_xmm, _mm_setr_ps(f, f, f, 1.0f));
	}

	// Assignment operators.
	SIMDVector& SIMDVector::operator += (const SIMDVector& v)
	{
		m_xmm = _mm_add_ps(m_xmm, v.m_xmm);
		return *this;
	}

	SIMDVector& SIMDVector::operator += (float f)
	{
		m_xmm = _mm_add_ps(m_xmm, _mm_setr_ps(f, f, f, 0.0f));
		return *this;
	}

	SIMDVector& SIMDVector::operator -= (const SIMDVector& v)
	{
		m_xmm = _mm_sub_ps(m_xmm, v.m_xmm);
		return *this;
	}

	SIMDVector& SIMDVector::operator -= (float f)
	{
		m_xmm = _mm_sub_ps(m_xmm, _mm_setr_ps(f, f, f, 0.0f));
		return *this;
	}

	SIMDVector& SIMDVector::operator *= (float f)
	{
		m_xmm = _mm_mul_ps(m_xmm, _mm_setr_ps(f, f, f, 1.0f));
		return *this;
	}

	SIMDVector& SIMDVector::operator /= (float f)
	{
		m_xmm = _mm_div_ps(m_xmm, _mm_setr_ps(f, f, f, 1.0f));
		return *this;
	}

	// Conditional operators.
	bool SIMDVector::operator == (const SIMDVector& v) const
	{
		__m128i vcmp = _mm_castps_si128(_mm_cmpneq_ps(m_xmm, v.m_xmm));
		u16 test = _mm_movemask_epi8(vcmp);
		return test == 0;
	}

	bool SIMDVector::operator != (const SIMDVector& v) const
	{
		__m128i vcmp = _mm_castps_si128(_mm_cmpneq_ps(m_xmm, v.m_xmm));
		u16 test = _mm_movemask_epi8(vcmp);
		return test != 0;
	}

	// Specialty methods.
	vf32 SIMDVector::LengthSq() const
	{
		return Dot(*this);
	}

	vf32 SIMDVector::Length() const
	{
		return _mm_sqrt_ps(LengthSq());
	}

	void SIMDVector::Normalize()
	{
		m_xmm = _mm_div_ps(m_xmm, _mm_set1_ps(_mm_cvtss_f32(Length())));
	}

	SIMDVector SIMDVector::Normalized() const
	{
		return _mm_div_ps(m_xmm, _mm_set1_ps(_mm_cvtss_f32(Length())));
	}

	vf32 SIMDVector::LengthSq(const SIMDVector& v)
	{
		return v.LengthSq();
	}

	vf32 SIMDVector::Length(const SIMDVector& v)
	{
		return v.Length();
	}

	void SIMDVector::Normalize(SIMDVector& v)
	{
		v.Normalize();
	}

	SIMDVector SIMDVector::Normalized(const SIMDVector& v)
	{
		return v.Normalized();
	}

	// Products.
	SIMDVector SIMDVector::PointwiseProduct(const SIMDVector& v) const
	{
		return _mm_mul_ps(m_xmm, v.m_xmm);
	}

	// 'w' remains 0 with implicit 1 where needed, so 'or' on a 1 to the 'w' component for valid divide.
	SIMDVector SIMDVector::PointwiseQuotient(const SIMDVector& v) const
	{
		return _mm_div_ps(m_xmm, _mm_or_ps(v.m_xmm, _mm_setr_ps(0.0f, 0.0, 0.0, 1.0f)));
	}

	vf32 SIMDVector::Dot(const SIMDVector& v) const
	{
		vf32 prod = _mm_mul_ps(m_xmm, v.m_xmm);

		vf32 shuf = _mm_movehdup_ps(prod);
		prod = _mm_add_ps(prod, shuf);
		shuf = _mm_movehl_ps(shuf, prod);

		return _mm_add_ss(prod, shuf);
	}

	SIMDVector SIMDVector::Cross(const SIMDVector& v) const
	{
		/* yz - zy
				zx - xz
				xy - yx */
		vf32 res = _mm_sub_ps(
			_mm_mul_ps(m_xmm, _mm_shuffle_ps(v.m_xmm, v.m_xmm, _MM_SHUFFLE(3, 0, 2, 1))), // x y z w * y z x w
			_mm_mul_ps(v.m_xmm, _mm_shuffle_ps(m_xmm, m_xmm, _MM_SHUFFLE(3, 0, 2, 1))));

		return _mm_shuffle_ps(res, res, _MM_SHUFFLE(3, 0, 2, 1));
	}


	SIMDVector SIMDVector::PointwiseProduct(const SIMDVector& v0, const SIMDVector& v1)
	{
		return v0.PointwiseProduct(v1);
	}

	SIMDVector SIMDVector::PointwiseQuotient(const SIMDVector& v0, const SIMDVector& v1)
	{
		return v0.PointwiseQuotient(v1);
	}

	vf32 SIMDVector::Dot(const SIMDVector& v0, const SIMDVector& v1)
	{
		return v0.Dot(v1);
	}

	SIMDVector SIMDVector::Cross(const SIMDVector& v0, const SIMDVector& v1)
	{
		return v0.Cross(v1);
	}

	// Interpolations.
	SIMDVector SIMDVector::Lerp(const SIMDVector& to, const SIMDVector& t) const
	{
		return _mm_add_ps(m_xmm, _mm_mul_ps(_mm_sub_ps(to.m_xmm, m_xmm), t.m_xmm));
	}

	SIMDVector SIMDVector::Lerp(const SIMDVector& to, float t) const
	{
		return _mm_add_ps(m_xmm, _mm_mul_ps(_mm_sub_ps(to.m_xmm, m_xmm), _mm_set_ps1(t)));
	}

	SIMDVector SIMDVector::MoveTowards(const SIMDVector& to, float t) const
	{
		const SIMDVector diff = to - *this;
		if(_mm_cvtss_f32(diff.LengthSq()) < 1e-8f)
		{
			return to;
		}

		const SIMDVector target = *this + diff.Normalized() * t;
		if(_mm_cvtss_f32(Dot(diff, to - target)) <= 0.0f)
		{
			return to;
		}

		return target;
	}

	SIMDVector SIMDVector::Lerp(const SIMDVector& from, const SIMDVector& to, const SIMDVector& t)
	{
		return from.Lerp(to, t);
	}
	
	SIMDVector SIMDVector::Lerp(const SIMDVector& from, const SIMDVector& to, float t)
	{
		return from.Lerp(to, t);
	}

	SIMDVector SIMDVector::MoveTowards(const SIMDVector& from, const SIMDVector& to, float t)
	{
		return from.MoveTowards(to, t);
	}
};
