//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathMatrix2x2.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMathMatrix2x2.h"

namespace Math
{

	//
	// Binary operators.
	//

	inline Matrix2x2 Matrix2x2::operator + (const Matrix2x2& m) const
	{
		return Matrix2x2(rows[0] + m.rows[0], rows[1] + m.rows[1]);
	}

	inline Matrix2x2 Matrix2x2::operator + (float f) const
	{
		return Matrix2x2(rows[0] + f, rows[1] + f);
	}

	inline Matrix2x2 Matrix2x2::operator - (const Matrix2x2& m) const
	{
		return Matrix2x2(rows[0] - m.rows[0], rows[1] - m.rows[1]);
	}

	inline Matrix2x2 Matrix2x2::operator - (float f) const
	{
		return Matrix2x2(rows[0] - f, rows[1] - f);
	}

	Matrix2x2 Matrix2x2::operator * (const Matrix2x2& m) const
	{
		return Matrix2x2(
			_11 * m._11 + _12 * m._21,
			_11 * m._12 + _12 * m._22,

			_21 * m._11 + _22 * m._21,
			_21 * m._12 + _22 * m._22
		);
	}

	Vector2 Matrix2x2::operator * (const Vector2& v) const
	{
		return Vector2(
			_11 * v[0] + _12 * v[1],
			_21 * v[0] + _22 * v[1]
		);
	}

	inline Matrix2x2 Matrix2x2::operator * (float f) const
	{
		return Matrix2x2(rows[0] * f, rows[1] * f);
	}

	//
	// Assignment operators.
	//

	Matrix2x2& Matrix2x2::operator += (const Matrix2x2& m)
	{
		rows[0] += m.rows[0];
		rows[1] += m.rows[1];
		rows[2] += m.rows[2];
		return *this;
	}

	Matrix2x2& Matrix2x2::operator += (float f)
	{
		rows[0] += f;
		rows[1] += f;
		rows[2] += f;
		return *this;
	}

	Matrix2x2& Matrix2x2::operator -= (const Matrix2x2& m)
	{
		rows[0] -= m.rows[0];
		rows[1] -= m.rows[1];
		rows[2] -= m.rows[2];
		return *this;
	}

	Matrix2x2& Matrix2x2::operator -= (float f)
	{
		rows[0] -= f;
		rows[1] -= f;
		rows[2] -= f;
		return *this;
	}

	Matrix2x2& Matrix2x2::operator *= (const Matrix2x2& m)
	{
		*this = *this * m;
		return *this;
	}

	Matrix2x2& Matrix2x2::operator *= (float f)
	{
		rows[0] *= f;
		rows[1] *= f;
		rows[2] *= f;
		return *this;
	}

	//
	// Methods.
	//

	float Matrix2x2::Trace() const
	{
		return _11 + _22;
	}

	float Matrix2x2::Determinate() const
	{
		return (_11 * _22) - (_12 * _21);
	}

	// Standard 2x2 eigen decomposition algorithm.
	void Matrix2x2::EigenDecomposition(Math::Vector2& eigenvalues, Matrix2x2& eigenvectors) const
	{
		// Eigenvalues.
		const float tr = Trace();
		const float gap = sqrtf(tr * tr - Determinate() * 4.0f);
		eigenvalues[0] = (tr - gap) * 0.5f;
		eigenvalues[1] = (tr + gap) * 0.5f;

		{ // Eigenvector 1
			const Matrix2x2 m1 = *this - MTX2X2_IDENTITY * eigenvalues[1];
			const Vector2 v0(m1.rows[0][0], m1.rows[1][0]);
			const Vector2 v1(m1.rows[0][1], m1.rows[1][1]);
			if(v0.LengthSq() > v1.LengthSq())
			{
				eigenvectors.rows[0] = v0.Normalized();
			}
			else
			{
				eigenvectors.rows[0] = v1.Normalized();
			}
		}

		{ // Eigenvector 2
			const Matrix2x2 m2 = *this - MTX2X2_IDENTITY * eigenvalues[0];
			const Vector2 v0(m2.rows[0][0], m2.rows[1][0]);
			const Vector2 v1(m2.rows[0][1], m2.rows[1][1]);
			if(v0.LengthSq() > v1.LengthSq())
			{
				eigenvectors.rows[1] = v0.Normalized();
			}
			else
			{
				eigenvectors.rows[1] = v1.Normalized();
			}
		}
	}

	Matrix2x2 Matrix2x2::PseudoInverse() const
	{
		Math::Vector2 eval;
		Math::Matrix2x2 evec;
		EigenDecomposition(eval, evec);

		auto Reciprocal = [](float f) { return fabsf(f) < 1e-5f ? 0.0f : 1.0f / f; };

		Math::Matrix2x2 sigma(
			Reciprocal(eval[0]), 0.0f,
			0.0f, Reciprocal(eval[1])
		);

		return Math::Matrix2x2::Transpose(evec) * sigma * evec;
	}

	//
	// Static methods.
	//

	Matrix2x2 Matrix2x2::FromVector(const Math::Vector2& v0, const Math::Vector2& v1)
	{
		return Matrix2x2(v0[0] * v1[0], v0[0] * v1[1],
			v0[1] * v1[0], v0[1] * v1[1]);
	}

	Matrix2x2 Matrix2x2::Transpose(const Matrix2x2& m)
	{
		return Matrix2x2(
			m.rows[0][0], m.rows[1][0],
			m.rows[0][1], m.rows[1][1]
		);
	}
};
