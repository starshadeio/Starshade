//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CSIMDQuaternion.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSIMDQuaternion.h"
#include "CMathFloat.h"
#include "CMathVector4.h"
#include "CMathMatrix3x3.h"

namespace Math
{
	// Binary operators.
	SIMDQuaternion SIMDQuaternion::operator * (const SIMDQuaternion& q) const
	{
		vf32 v0 = _mm_mul_ps(_mm_shuffle_ps(m_xmm, m_xmm, _MM_SHUFFLE(3, 3, 3, 3)), _mm_shuffle_ps(q.m_xmm, q.m_xmm, _MM_SHUFFLE(3, 2, 1, 0)));
		vf32 v1 = _mm_mul_ps(_mm_shuffle_ps(m_xmm, m_xmm, _MM_SHUFFLE(0, 2, 1, 0)), _mm_shuffle_ps(q.m_xmm, q.m_xmm, _MM_SHUFFLE(0, 3, 3, 3)));
		vf32 v2 = _mm_mul_ps(_mm_shuffle_ps(m_xmm, m_xmm, _MM_SHUFFLE(1, 0, 2, 1)), _mm_shuffle_ps(q.m_xmm, q.m_xmm, _MM_SHUFFLE(1, 1, 0, 2)));
		vf32 v3 = _mm_mul_ps(_mm_shuffle_ps(m_xmm, m_xmm, _MM_SHUFFLE(2, 1, 0, 2)), _mm_shuffle_ps(q.m_xmm, q.m_xmm, _MM_SHUFFLE(2, 0, 2, 1)));

		v1 = _mm_xor_ps(v1, _mm_setr_ps(0.0f, 0.0f, 0.0f, -0.0f));
		v2 = _mm_xor_ps(v2, _mm_setr_ps(0.0f, 0.0f, 0.0f, -0.0f));
		v3 = _mm_xor_ps(v3, _mm_set1_ps(-0.0f));

		return SIMDQuaternion(_mm_add_ps(_mm_add_ps(_mm_add_ps(v0, v1), v2), v3));
	}
	
	// Transforms a directional vector by this quaternion. 
	//  TODO: Create a more SIMD friendly version of this.
	SIMDVector SIMDQuaternion::operator * (const SIMDVector& v) const {
		Vector4 quat = *(Vector4*)m_xmm.m128_f32;
		Vector4 vec = *(Vector4*)v.m_xmm.m128_f32;

		float num = quat.x * 2.0f;
		float num2 = quat.y * 2.0f;
		float num3 = quat.z * 2.0f;

		float num4 = quat.x * num;
		float num5 = quat.y * num2;
		float num6 = quat.z * num3;

		float num7 = quat.x * num2;
		float num8 = quat.x * num3;
		float num9 = quat.y * num3;

		float num10 = quat.w * num;
		float num11 = quat.w * num2;
		float num12 = quat.w * num3;
		
		return SIMDVector(
			(1.0f - (num5 + num6)) * vec.x + (num7 - num12) * vec.y + (num8 + num11) * vec.z,
			(num7 + num12) * vec.x + (1.0f - (num4 + num6)) * vec.y + (num9 - num10) * vec.z,
			(num8 - num11) * vec.x + (num9 + num10) * vec.y + (1.0f - (num4 + num5)) * vec.z
		);
	}

	SIMDQuaternion SIMDQuaternion::operator + (const SIMDQuaternion& q) const
	{
		return _mm_add_ps(m_xmm, q.m_xmm);
	}

	SIMDQuaternion SIMDQuaternion::operator - (const SIMDQuaternion& q) const
	{
		return _mm_sub_ps(m_xmm, q.m_xmm);
	}

	SIMDQuaternion SIMDQuaternion::operator * (float f) const
	{
		return _mm_mul_ps(m_xmm, _mm_set1_ps(f));
	}

	SIMDQuaternion SIMDQuaternion::operator / (float f) const
	{
		return _mm_div_ps(m_xmm, _mm_set1_ps(f));
	}

	// Assignment operators.
	SIMDQuaternion& SIMDQuaternion::operator *= (const SIMDQuaternion& q)
	{
		*this = *this * q;
		return *this;
	}

	SIMDQuaternion& SIMDQuaternion::operator += (const SIMDQuaternion& q)
	{
		m_xmm = _mm_add_ps(m_xmm, q.m_xmm);
		return *this;
	}

	SIMDQuaternion& SIMDQuaternion::operator -= (const SIMDQuaternion& q)
	{
		m_xmm = _mm_sub_ps(m_xmm, q.m_xmm);
		return *this;
	}

	SIMDQuaternion& SIMDQuaternion::operator *= (float f)
	{
		m_xmm = _mm_mul_ps(m_xmm, _mm_set1_ps(f));
		return *this;
	}

	SIMDQuaternion& SIMDQuaternion::operator /= (float f)
	{
		m_xmm = _mm_div_ps(m_xmm, _mm_set1_ps(f));
		return *this;
	}
	
	// Conditional operators.
	bool SIMDQuaternion::operator == (const SIMDQuaternion& v) const
	{
		__m128i vcmp = _mm_castps_si128(_mm_cmpneq_ps(m_xmm, v.m_xmm));
		u16 test = _mm_movemask_epi8(vcmp);
		return test == 0;
	}

	bool SIMDQuaternion::operator != (const SIMDQuaternion& v) const
	{
		__m128i vcmp = _mm_castps_si128(_mm_cmpneq_ps(m_xmm, v.m_xmm));
		u16 test = _mm_movemask_epi8(vcmp);
		return test != 0;
	}

	// Rotational methods.
	SIMDQuaternion SIMDQuaternion::RotateDirection(const SIMDVector& forward, const SIMDVector& upward)
	{
		SIMDVector right = SIMDVector::Cross(upward, forward).Normalized();
		SIMDVector up = SIMDVector::Cross(forward, right).Normalized();

		const Math::Matrix3x3 R = Math::Matrix3x3(
			right[0], up[0], forward[0],
			right[1], up[1], forward[1],
			right[2], up[2], forward[2]
		);

		const float trace = R.At(0, 0) + R.At(1, 1) + R.At(2, 2);
		float q[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		if(trace > 0.0f)
		{
			const float s = sqrtf(1.0f + trace);
			q[3] = s * 0.5f;
			
			const float t = 0.5f / s;
			q[0] = (R.At(2, 1) - R.At(1, 2)) * t;
			q[1] = (R.At(0, 2) - R.At(2, 0)) * t;
			q[2] = (R.At(1, 0) - R.At(0, 1)) * t;
		}
		else
		{
			int i = 0;
			if(R.At(1, 1) > R.At(0, 0)) i = 1;
			if(R.At(2, 2) > R.At(i, i)) i = 2;

			static const int NEXT[3] = { 1, 2, 0 };
			int j = NEXT[i];
			int k = NEXT[j];

			float s = sqrtf((R.At(i, i) - (R.At(j, j) + R.At(k, k))) + 1.0f);
			q[i] = s * 0.5f;

			float t;
			if(s != 0.0f) t = 0.5f / s;
			else t = s;

			q[3] = (R.At(k, j) - R.At(j, k)) * t;
			q[j] = (R.At(j, i) + R.At(i, j)) * t;
			q[k] = (R.At(k, i) + R.At(i, k)) * t;
		}

		return SIMDQuaternion(q[0], q[1], q[2], q[3]);
	}

	SIMDQuaternion SIMDQuaternion::RotateX(float theta)
	{
		float thetaOver2 = theta * -0.5f;
		return _mm_setr_ps(sinf(thetaOver2), 0.0f, 0.0f, cosf(thetaOver2));
	}

	SIMDQuaternion SIMDQuaternion::RotateY(float theta)
	{
		float thetaOver2 = theta * -0.5f;
		return _mm_setr_ps(0.0f, sinf(thetaOver2), 0.0f, cosf(thetaOver2));
	}

	SIMDQuaternion SIMDQuaternion::RotateZ(float theta)
	{
		float thetaOver2 = theta * -0.5f;
		return _mm_setr_ps(0.0f, 0.0f, sinf(thetaOver2), cosf(thetaOver2));
	}

	SIMDQuaternion SIMDQuaternion::RotateAxis(const SIMDVector& axis, float theta)
	{
		float thetaOver2 = theta * -0.5f;
		return _mm_add_ps((axis * sinf(thetaOver2)).m_xmm,  _mm_setr_ps(0.0f, 0.0f, 0.0f, cosf(thetaOver2)));
	}

	SIMDQuaternion SIMDQuaternion::RotateEuler(const SIMDVector& euler)
	{
		const float* f = euler.ToFloat();

		float f0 = f[0] * -0.5f;
		float sx = sinf(f0); float cx = cosf(f0);

		f0 = f[1] * -0.5f;
		float sy = sinf(f0); float cy = cosf(f0);

		f0 = f[2] * -0.5f;
		float sz = sinf(f0); float cz = cosf(f0);

		// Object -> Intertial
		return _mm_setr_ps(
			cy * sx * cz + sy * cx * sz,
			sy * cx * cz - cy * sx * sz,
			cy * cx * sz - sy * sx * cz,
			cy * cx * cz + sy * sx * sz
		);
	}

	// Method for converting this quaternion to a euler angle vector.
	SIMDVector SIMDQuaternion::ToEuler() const
	{
		const float* f = m_xmm.m128_f32;
		float h, p, b;

		// Object -> Inertial (new -> old)
		float sp = -2.0f * (f[1] * f[2] - f[3] * f[0]);
		if(fabsf(sp) > 0.9999f)
		{
			p = Math::g_PiOver2 * sp;
			h = atan2f(-f[0] * f[2] + f[3] * f[1], 0.5f - f[1] * f[1] - f[2] * f[2]);
			b = 0.0f;
		}
		else
		{
			p = asinf(sp);
			h = atan2f(f[0] * f[2] + f[3] * f[1], 0.5f - f[0] * f[0] - f[1] * f[1]);
			b = atan2f(f[0] * f[1] + f[3] * f[2], 0.5f - f[0] * f[0] - f[2] * f[2]);
		}

		return Math::SIMDVector(p, h, b);
	}

	SIMDQuaternion SIMDQuaternion::RotateX(const SIMDQuaternion& q, float theta)
	{
		SIMDQuaternion res = q;
		res.RotateX(theta);
		return res;
	}

	SIMDQuaternion SIMDQuaternion::RotateY(const SIMDQuaternion& q, float theta)
	{
		SIMDQuaternion res = q;
		res.RotateY(theta);
		return res;
	}

	SIMDQuaternion SIMDQuaternion::RotateZ(const SIMDQuaternion& q, float theta)
	{
		SIMDQuaternion res = q;
		res.RotateZ(theta);
		return res;
	}

	SIMDQuaternion SIMDQuaternion::RotateEuler(const SIMDQuaternion& q, const SIMDVector& euler)
	{
		SIMDQuaternion res = q;
		res.RotateEuler(euler);
		return res;
	}

	SIMDVector SIMDQuaternion::ToEuler(const SIMDQuaternion& q)
	{
		return q.ToEuler();
	}

	// General methods.
	SIMDQuaternion SIMDQuaternion::Normalize() const
	{
		vf32 v = _mm_mul_ps(m_xmm, m_xmm);
		v = _mm_hadd_ps(v, v); v = _mm_hadd_ps(v, v);
		return _mm_div_ps(m_xmm, _mm_sqrt_ps(v));
	}

	SIMDQuaternion SIMDQuaternion::Conjugate() const
	{
		return SIMDQuaternion(_mm_xor_ps(m_xmm, _mm_setr_ps(0.0f, 0.0f, 0.0f, -0.0f)));
	}

	SIMDQuaternion SIMDQuaternion::Normalize(const SIMDQuaternion& q)
	{
		return q.Normalize();
	}

	SIMDQuaternion SIMDQuaternion::Conjugate(const SIMDQuaternion& q)
	{
		return q.Conjugate();
	}

	// Product methods.
	void SIMDQuaternion::Concatenate(const SIMDQuaternion& q)
	{
		*this *= q;
	}

	vf32 SIMDQuaternion::Dot(const SIMDQuaternion& q) const
	{
		vf32 prod = _mm_mul_ps(m_xmm, q.m_xmm);

		vf32 shuf = _mm_movehdup_ps(prod);
		prod = _mm_add_ps(prod, shuf);
		shuf = _mm_movehl_ps(shuf, prod);

		return _mm_add_ss(prod, shuf);
	}

	SIMDQuaternion SIMDQuaternion::Concatenate(const SIMDQuaternion& q0, const SIMDQuaternion& q1)
	{
		return q0 * q1;
	}

	vf32 SIMDQuaternion::Dot(const SIMDQuaternion& q0, const SIMDQuaternion& q1)
	{
		return q0.Dot(q1);
	}

	// Interpolation methods.
	SIMDQuaternion SIMDQuaternion::Lerp(const SIMDQuaternion& to, float t) const
	{
		return (*this * (1.0f - t) + to * t).Normalize();
	}

	SIMDQuaternion SIMDQuaternion::Slerp(const SIMDQuaternion& to, float t) const
	{
		SIMDQuaternion q3;
		vf32 dot = Dot(to);

		/*	dot = cos(theta)
			if (dot < 0), q1 and q2 are more than 90 degrees apart,
			so we can invert one to reduce spinning	*/
		if(_mm_comilt_ss(dot, _mm_set_ps1(0.0f)))
		{
			dot = _mm_xor_ps(dot, _mm_set_ps1(-0.0f));
			q3 = _mm_xor_ps(to.m_xmm, _mm_set_ps1(-0.0f));
		}
		else
		{
			q3 = to;
		}

		if(_mm_comilt_ss(dot, _mm_set_ps1(0.95f)))
		{
			float angle = acosf(_mm_cvtss_f32(dot));
			return (*this * sinf(angle * (1 - t)) + q3 * sinf(angle * t)) / sinf(angle);
		}
		else
		{ // if the angle is small, use linear interpolation								
			return Lerp(q3, t);
		}
	}

	SIMDQuaternion SIMDQuaternion::Lerp(const SIMDQuaternion& from, const SIMDQuaternion& to, float t)
	{
		return from.Lerp(to, t);
	}

	SIMDQuaternion SIMDQuaternion::Slerp(const SIMDQuaternion& from, const SIMDQuaternion& to, float t)
	{
		return from.Slerp(to, t);
	}
};
