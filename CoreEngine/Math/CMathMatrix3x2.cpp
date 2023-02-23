//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathMatrix3x2.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMathMatrix3x2.h"

namespace Math
{
	// Concatenation operator.
	Matrix3x2 Matrix3x2::operator * (const Matrix3x2& M) const
	{
		return Matrix3x2(
			_11 * M._11 + _12 * M._21,
			_11 * M._12 + _12 * M._22,

			_21 * M._11 + _22 * M._21,
			_21 * M._12 + _22 * M._22,

			_31 * M._11 + _32 * M._21 + M._31,
			_31 * M._12 + _32 * M._22 + M._32
		);
	}
	
	// Concatenation assignment operator.
	Matrix3x2& Matrix3x2::operator *= (const Matrix3x2& M)
	{
		*this = Matrix3x2(
			_11 * M._11 + _12 * M._21,
			_11 * M._12 + _12 * M._22,

			_21 * M._11 + _22 * M._21,
			_21 * M._12 + _22 * M._22,

			_31 * M._11 + _32 * M._21 + M._31,
			_31 * M._12 + _32 * M._22 + M._32
		);

		return *this;
	}

	// Multiply by a scalar operator.
	Matrix3x2 Matrix3x2::operator * (float f) const
	{
		return Matrix3x2(
			_11 * f,
			_12 * f,
			_21 * f,
			_22 * f,
			_31 * f,
			_32 * f
		);
	}
	
	// Multiply by a scalar assignment operator.
	Matrix3x2& Matrix3x2::operator *= (float f)
	{
		*this = Matrix3x2(
			_11 * f,
			_12 * f,
			_21 * f,
			_22 * f,
			_31 * f,
			_32 * f
		);

		return *this;
	}

	// Method for revert matrix back to the identity.
	void Matrix3x2::ToIdentity()
	{
		*this = MTX3X2_IDENTITY;
	}

	// Method for transposing the matrix.
	void Matrix3x2::Transpose2x2()
	{
		const float tmp = _12;
		_12 = _21;
		_21 = tmp;
	}
	
	// Method for inverting the matrix.
	void Matrix3x2::Inverse()
	{
		const float detInv = 1.0f / Determinant();

		*this = Matrix3x2(
			_22, -_12,
			-_21, _11,
			_21 * _32 - _31 * _22, _31 * _12 - _11 * _32
		) * detInv;
	}
	
	// Method for transforming a normal vector with this matrix.
	Vector2 Matrix3x2::TransformNormal(const Vector2& normal) const
	{
		return Vector2(
			normal.x * _11 + normal.y * _21, 
			normal.x * _12 + normal.y * _22
		);
	}
	
	// Method for transforming a point vector with this matrix.
	Vector2 Matrix3x2::TransformPoint(const Vector2& point) const
	{
		return Vector2(
			point.x * _11 + point.y * _21 + _31, 
			point.x * _12 + point.y * _22 + _32
		);
	}
	
	// Method for calculating the determinant the matrix.
	float Matrix3x2::Determinant() const
	{
		return _11 * _22 - _12 * _21;
	}

	//
	// Static methods.
	//

	// Function for creating a translation matrix with a given position.
	Matrix3x2 Matrix3x2::Translate(const Vector2& position)
	{
		return Matrix3x2(
			1.0f, 0.0f,
			0.0f, 1.0f,
			position.x, position.y
		);
	}
	
	// Function for creating a rotation matrix with a given angle.
	Matrix3x2 Matrix3x2::Rotate(float angle)
	{
		const float c = cosf(angle);
		const float s = sinf(angle);
		return Matrix3x2(
			c,-s,
			s, c,
			0, 0
		);
	}

	// Function for creating a scaling matrix with a given scale.
	Matrix3x2 Matrix3x2::Scale(const Vector2& scale)
	{
		return Matrix3x2(
			scale.x, 0.0f,
			0.0f, scale.y,
			0.0f, 0.0f
		);
	}
};
