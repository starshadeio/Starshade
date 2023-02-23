//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CSIMDMatrix.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSIMDMatrix.h"
#include "CMathFloat.h"

namespace Math
{
	// Function for performing a row by column multiplication.
	// This is used internally by matrix concatenation methods.
	vf32 ConcatenateRow(vf32 r, const SIMDMatrix& tranpose)
	{
		vf32 dx = _mm_mul_ps(r, tranpose.rows[0]);
		vf32 dy = _mm_mul_ps(r, tranpose.rows[1]);
		vf32 dz = _mm_mul_ps(r, tranpose.rows[2]);
		vf32 dw = _mm_mul_ps(r, tranpose.rows[3]);

		dx = _mm_hadd_ps(dx, dx);
		dx = _mm_hadd_ps(dx, dx);

		dy = _mm_hadd_ps(dy, dy);
		dy = _mm_hadd_ps(dy, dy);

		dz = _mm_hadd_ps(dz, dz);
		dz = _mm_hadd_ps(dz, dz);

		dw = _mm_hadd_ps(dw, dw);
		dw = _mm_hadd_ps(dw, dw);

		vf32 res1 = _mm_shuffle_ps(dx, dy, _MM_SHUFFLE(2, 2, 0, 0));
		vf32 res2 = _mm_shuffle_ps(dz, dw, _MM_SHUFFLE(2, 2, 0, 0));
		return _mm_shuffle_ps(res1, res2, _MM_SHUFFLE(2, 0, 2, 0));
	}

	// Operators.
	SIMDMatrix SIMDMatrix::operator * (const SIMDMatrix& M) const
	{
		SIMDMatrix tmp = *this;
		tmp.Transpose();

		return SIMDMatrix(
			ConcatenateRow(M.rows[0], tmp),
			ConcatenateRow(M.rows[1], tmp),
			ConcatenateRow(M.rows[2], tmp),
			ConcatenateRow(M.rows[3], tmp)
		);
	}

	SIMDMatrix& SIMDMatrix::operator *= (const SIMDMatrix& M)
	{
		SIMDMatrix tmp = *this;
		tmp.Transpose();

		*this = SIMDMatrix(
			ConcatenateRow(M.rows[0], tmp),
			ConcatenateRow(M.rows[1], tmp),
			ConcatenateRow(M.rows[2], tmp),
			ConcatenateRow(M.rows[3], tmp)
		);

		return *this;
	}

	// Product methods.
	SIMDVector SIMDMatrix::TransformNormal(const SIMDVector& normal) const
	{
		SIMDMatrix mtx = *this;
		_MM_TRANSPOSE4_PS(mtx.rows[0], mtx.rows[1], mtx.rows[2], mtx.rows[3]);
		vf32 r0 = _mm_mul_ps(mtx.rows[0], _mm_shuffle_ps(normal.m_xmm, normal.m_xmm, _MM_SHUFFLE(3, 0, 0, 0)));
		r0 = _mm_add_ps(r0, _mm_mul_ps(mtx.rows[1], _mm_shuffle_ps(normal.m_xmm, normal.m_xmm, _MM_SHUFFLE(3, 1, 1, 1))));

		return _mm_add_ps(r0, _mm_mul_ps(mtx.rows[2], _mm_shuffle_ps(normal.m_xmm, normal.m_xmm, _MM_SHUFFLE(3, 2, 2, 2))));
	}

	SIMDVector SIMDMatrix::TransformPoint(const SIMDVector& point) const
	{
		SIMDMatrix mtx = *this;
		_MM_TRANSPOSE4_PS(mtx.rows[0], mtx.rows[1], mtx.rows[2], mtx.rows[3]);
		vf32 r0 = _mm_mul_ps(mtx.rows[0], _mm_shuffle_ps(point.m_xmm, point.m_xmm, _MM_SHUFFLE(3, 0, 0, 0)));
		r0 = _mm_add_ps(r0, _mm_mul_ps(mtx.rows[1], _mm_shuffle_ps(point.m_xmm, point.m_xmm, _MM_SHUFFLE(3, 1, 1, 1))));
		r0 = _mm_add_ps(r0, _mm_mul_ps(mtx.rows[2], _mm_shuffle_ps(point.m_xmm, point.m_xmm, _MM_SHUFFLE(3, 2, 2, 2))));

		return _mm_add_ps(r0, _mm_and_ps(mtx.rows[3], _mm_setr_ps(g_NegNan, g_NegNan, g_NegNan, 0.0f)));
	}

	// Transformation methods.
	SIMDMatrix SIMDMatrix::Translate(const SIMDVector& position)
	{
		SIMDMatrix mtx = SIMD_MTX4X4_IDENTITY;
		mtx.rows[3] = _mm_or_ps(position.m_xmm, _mm_setr_ps(0, 0, 0, 1));
		_MM_TRANSPOSE4_PS(mtx.rows[0], mtx.rows[1], mtx.rows[2], mtx.rows[3]);

		return mtx;
	}

	SIMDMatrix SIMDMatrix::Rotate(const SIMDQuaternion& rotation)
	{
		const float* f = rotation.ToFloat();

		const float xsq2 = 2.0f * f[0] * f[0];
		const float ysq2 = 2.0f * f[1] * f[1];
		const float zsq2 = 2.0f * f[2] * f[2];

		const float xy2 = 2.0f * f[0] * f[1];
		const float xz2 = 2.0f * f[0] * f[2];
		const float xw2 = 2.0f * f[0] * f[3];

		const float yz2 = 2.0f * f[1] * f[2];
		const float yw2 = 2.0f * f[1] * f[3];

		const float zw2 = 2.0f * f[2] * f[3];

		return SIMDMatrix(
			1.0f - ysq2 - zsq2, xy2 - zw2, xz2 + yw2, 0.0f,
			xy2 + zw2, 1.0f - xsq2 - zsq2, yz2 - xw2, 0.0f,
			xz2 - yw2, yz2 + xw2, 1.0f - xsq2 - ysq2, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	SIMDMatrix SIMDMatrix::Rotate(const SIMDVector& euler)
	{
		const float* f = euler.ToFloat();

		const float sh = sinf(f[1]);
		const float ch = cosf(f[1]);
		const float sp = sinf(f[0]);
		const float cp = cosf(f[0]);
		const float sb = sinf(f[2]);
		const float cb = cosf(f[2]);

		/*return SIMDMatrix(
			ch * cp, sh * sb - ch * sp * cb, ch * sp * sb + sh * cb, 0.0f,
			sp, cp * cb, -cp * sb, 0.0f,
			-sh * cp, sh * sp * cb + ch * sb, -sh * sp * sb + ch * cb, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);*/

		return SIMDMatrix(
			ch * cb + sh * sp * sb, sb * cp, -sh * cb + ch * sp * sb, 0.0f,
			-ch * sb + sh * sp * cb, cb * cp, sb * sh + ch * sp * cb, 0.0f,
			sh * cp, -sp, ch * cp, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	SIMDMatrix SIMDMatrix::RotateX(float theta)
	{
		SIMDMatrix mtx = SIMD_MTX4X4_IDENTITY;

		const float s = sinf(theta);
		const float c = cosf(theta);
		mtx.rows[1] = _mm_setr_ps(0.0f, c, s, 0.0f);
		mtx.rows[2] = _mm_setr_ps(0.0f, -s, c, 0.0f);

		return mtx;
	}

	SIMDMatrix SIMDMatrix::RotateY(float theta)
	{
		SIMDMatrix mtx = SIMD_MTX4X4_IDENTITY;

		const float s = sinf(theta);
		const float c = cosf(theta);
		mtx.rows[0] = _mm_setr_ps(c, 0.0f, -s, 0.0f);
		mtx.rows[2] = _mm_setr_ps(s, 0.0f, c, 0.0f);

		return mtx;
	}

	SIMDMatrix SIMDMatrix::RotateZ(float theta)
	{
		SIMDMatrix mtx = SIMD_MTX4X4_IDENTITY;

		const float s = sinf(theta);
		const float c = cosf(theta);
		mtx.rows[0] = _mm_setr_ps(c, s, 0.0f, 0.0f);
		mtx.rows[1] = _mm_setr_ps(-s, c, 0.0f, 0.0f);

		return mtx;
	}

	SIMDMatrix SIMDMatrix::Scale(const SIMDVector& scale)
	{
		SIMDMatrix mtx = SIMD_MTX4X4_IDENTITY;

		mtx.rows[0] = _mm_mul_ps(mtx.rows[0], scale.m_xmm);
		mtx.rows[1] = _mm_mul_ps(mtx.rows[1], scale.m_xmm);
		mtx.rows[2] = _mm_mul_ps(mtx.rows[2], scale.m_xmm);

		return mtx;
	}

	// General methods.
	void SIMDMatrix::ToIdentity()
	{
		rows[0] = _mm_setr_ps(1.0f, 0.0f, 0.0f, 0.0f);
		rows[1] = _mm_setr_ps(0.0f, 1.0f, 0.0f, 0.0f);
		rows[2] = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);
		rows[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
	}

	void SIMDMatrix::Transpose()
	{
		_MM_TRANSPOSE4_PS(rows[0], rows[1], rows[2], rows[3]);
	}

	// Makes the 3x3 matrix orthogonal.
	void SIMDMatrix::GramSchmidt3x3()
	{
		SIMDMatrix mtx = *this;
		_MM_TRANSPOSE4_PS(mtx.rows[0], mtx.rows[1], mtx.rows[2], mtx.rows[3]);
		SIMDVector a(rows[0]);
		SIMDVector b(rows[1]);
		SIMDVector c(rows[2]);

		a.Normalize();
		b -= a * _mm_cvtss_f32(SIMDVector::Dot(a, b));
		b.Normalize();
		c -= a * _mm_cvtss_f32(SIMDVector::Dot(a, c)) + b * _mm_cvtss_f32(SIMDVector::Dot(b, c));
		c.Normalize();

		mtx.rows[0] = a.m_xmm;
		mtx.rows[1] = b.m_xmm;
		mtx.rows[2] = c.m_xmm;

		_MM_TRANSPOSE4_PS(mtx.rows[0], mtx.rows[1], mtx.rows[2], mtx.rows[3]);
	}

	SIMDMatrix SIMDMatrix::Transpose(const SIMDMatrix& M)
	{
		SIMDMatrix M1 = M;
		_MM_TRANSPOSE4_PS(M1.rows[0], M1.rows[1], M1.rows[2], M1.rows[3]);
		return M1;
	}

	SIMDMatrix SIMDMatrix::LookAt(const SIMDVector& from, const SIMDVector& to)
	{
		SIMDVector right, up;
		SIMDVector forward = (to - from).Normalized();
		if(fabsf(_mm_cvtss_f32(SIMDVector::Dot(forward, Math::SIMD_VEC_UP))) < 0.9999f)
		{
			right = SIMDVector::Cross(Math::SIMD_VEC_UP, forward).Normalized();
			up = SIMDVector::Cross(forward, right);
		}
		else
		{
			up = forward;
			forward = SIMDVector::Cross(Math::SIMD_VEC_RIGHT, up).Normalized();
			right = SIMDVector::Cross(up, forward);
		}

		SIMDMatrix rotation;
		rotation.rows[0] = right.m_xmm;
		rotation.rows[1] = up.m_xmm;
		rotation.rows[2] = forward.m_xmm;
		rotation.rows[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
		rotation.Transpose();
		return rotation;
	}

	// View methods.
	SIMDMatrix SIMDMatrix::LookAtLH(const SIMDVector& eye, const SIMDVector& at, const SIMDVector& up)
	{
		SIMDMatrix mtx;

		SIMDVector zAxis = (at - eye).Normalized();
		SIMDVector xAxis = (SIMDVector::Cross(up, zAxis)).Normalized();
		SIMDVector yAxis = (SIMDVector::Cross(zAxis, xAxis)).Normalized();

		mtx.rows[0] = _mm_add_ps(xAxis.m_xmm, _mm_setr_ps(0.0f, 0.0f, 0.0f, -_mm_cvtss_f32(SIMDVector::Dot(xAxis, eye))));
		mtx.rows[1] = _mm_add_ps(yAxis.m_xmm, _mm_setr_ps(0.0f, 0.0f, 0.0f, -_mm_cvtss_f32(SIMDVector::Dot(yAxis, eye))));
		mtx.rows[2] = _mm_add_ps(zAxis.m_xmm, _mm_setr_ps(0.0f, 0.0f, 0.0f, -_mm_cvtss_f32(SIMDVector::Dot(zAxis, eye))));
		mtx.rows[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

		return mtx;
	}

	SIMDMatrix SIMDMatrix::LookAtRH(const SIMDVector& eye, const SIMDVector& at, const SIMDVector& up)
	{
		SIMDMatrix mtx;

		SIMDVector zAxis = (eye - at).Normalized();
		SIMDVector xAxis = (SIMDVector::Cross(up, zAxis)).Normalized();
		SIMDVector yAxis = (SIMDVector::Cross(zAxis, xAxis)).Normalized();

		mtx.rows[0] = _mm_add_ps(xAxis.m_xmm, _mm_setr_ps(0.0f, 0.0f, 0.0f, -_mm_cvtss_f32(SIMDVector::Dot(xAxis, eye))));
		mtx.rows[1] = _mm_add_ps(yAxis.m_xmm, _mm_setr_ps(0.0f, 0.0f, 0.0f, -_mm_cvtss_f32(SIMDVector::Dot(yAxis, eye))));
		mtx.rows[2] = _mm_add_ps(zAxis.m_xmm, _mm_setr_ps(0.0f, 0.0f, 0.0f, -_mm_cvtss_f32(SIMDVector::Dot(zAxis, eye))));
		mtx.rows[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

		return mtx;
	}

	// Perspective methods.
	SIMDMatrix SIMDMatrix::PerspectiveFovLH(float fov, float aspect, float nv, float fv)
	{
		SIMDMatrix mtx;

		const float y = 1.0f / tanf(fov / 2);
		const float x = y / aspect;
		const float z = fv / (fv - nv);

		mtx.rows[0] = _mm_setr_ps(x, 0.0f, 0.0f, 0.0f);
		mtx.rows[1] = _mm_setr_ps(0.0f, y, 0.0f, 0.0f);
		mtx.rows[2] = _mm_setr_ps(0.0f, 0.0f, z, -nv * z);
		mtx.rows[3] = _mm_setr_ps(0.0f, 0.0f, 1.0f, 0.0f);

		return mtx;
	}

	SIMDMatrix SIMDMatrix::PerspectiveFovLHInv(float fov, float aspect, float nv, float fv)
	{
		SIMDMatrix mtx;

		const float y = 1.0f / tanf(fov / 2);
		const float x = y / aspect;
		const float z = fv / (fv - nv);

		mtx.rows[0] = _mm_setr_ps(1.0f / x, 0.0f, 0.0f, 0.0f);
		mtx.rows[1] = _mm_setr_ps(0.0f, 1.0f / y, 0.0f, 0.0f);
		mtx.rows[2] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
		mtx.rows[3] = _mm_setr_ps(0.0f, 0.0f, -1.0f / (nv * z), 1.0f / nv);

		return mtx;
	}

	SIMDMatrix SIMDMatrix::PerspectiveFovRH(float fov, float aspect, float nv, float fv)
	{
		SIMDMatrix mtx;

		float y = 1.0f / tanf(fov / 2);
		float x = y / aspect;
		float z = fv / (fv - nv);

		mtx.rows[0] = _mm_setr_ps(x, 0.0f, 0.0f, 0.0f);
		mtx.rows[1] = _mm_setr_ps(0.0f, y, 0.0f, 0.0f);
		mtx.rows[2] = _mm_setr_ps(0.0f, 0.0f, z, nv * z);
		mtx.rows[3] = _mm_setr_ps(0.0f, 0.0f, -1.0f, 0.0f);

		return mtx;
	}

	SIMDMatrix SIMDMatrix::PerspectiveFovRHInv(float fov, float aspect, float nv, float fv)
	{
		SIMDMatrix mtx;

		const float y = 1.0f / tanf(fov / 2);
		const float x = y / aspect;
		const float z = fv / (fv - nv);

		mtx.rows[0] = _mm_setr_ps(1.0f / x, 0.0f, 0.0f, 0.0f);
		mtx.rows[1] = _mm_setr_ps(0.0f, 1.0f / y, 0.0f, 0.0f);
		mtx.rows[2] = _mm_setr_ps(0.0f, 0.0f, 0.0f, -1.0f);
		mtx.rows[3] = _mm_setr_ps(0.0f, 0.0f, 1.0f / (nv * z), 1.0f / nv);

		return mtx;
	}

	// Orthographics methods.
	SIMDMatrix SIMDMatrix::OrthographicLH(float width, float height, float nv, float fv)
	{
		SIMDMatrix mtx;

		mtx.rows[0] = _mm_setr_ps(2.0f / width, 0.0f, 0.0f, 0.0f);
		mtx.rows[1] = _mm_setr_ps(0.0f, 2.0f / height, 0.0f, 0.0f);
		mtx.rows[2] = _mm_setr_ps(0.0f, 0.0f, 1.0f / (fv - nv), -nv / (fv - nv));
		mtx.rows[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

		return mtx;
	}

	SIMDMatrix SIMDMatrix::OrthographicLHInv(float width, float height, float nv, float fv)
	{
		SIMDMatrix mtx;

		mtx.rows[0] = _mm_setr_ps(width / 2.0f, 0.0f, 0.0f, 0.0f);
		mtx.rows[1] = _mm_setr_ps(0.0f, height / 2.0f, 0.0f, 0.0f);
		mtx.rows[2] = _mm_setr_ps(0.0f, 0.0f, (fv - nv) / 1.0f, nv / (fv - nv));
		mtx.rows[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

		return mtx;
	}

	SIMDMatrix SIMDMatrix::OrthographicRH(float width, float height, float nv, float fv)
	{
		SIMDMatrix mtx;

		mtx.rows[0] = _mm_setr_ps(2.0f / width, 0.0f, 0.0f, 0.0f);
		mtx.rows[1] = _mm_setr_ps(0.0f, 2.0f / height, 0.0f, 0.0f);
		mtx.rows[2] = _mm_setr_ps(0.0f, 0.0f, 1.0f / (fv - nv), nv / (fv - nv));
		mtx.rows[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

		return mtx;
	}

	SIMDMatrix SIMDMatrix::OrthographicRHInv(float width, float height, float nv, float fv)
	{
		SIMDMatrix mtx;

		mtx.rows[0] = _mm_setr_ps(width / 2.0f, 0.0f, 0.0f, 0.0f);
		mtx.rows[1] = _mm_setr_ps(0.0f, height / 2.0f, 0.0f, 0.0f);
		mtx.rows[2] = _mm_setr_ps(0.0f, 0.0f, (fv - nv) / 1.0f, -nv / (fv - nv));
		mtx.rows[3] = _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);

		return mtx;
	}
};
