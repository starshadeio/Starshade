//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathMatrix3x2.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATHMATRIX3X2_H
#define CMATHMATRIX3X2_H

#include "CMathVector2.h"

namespace Math
{
	struct Matrix3x2
	{
		union
		{
			struct { float _11, _12, _21, _22, _31, _32; };
			float m[6];
		};

		Matrix3x2() { }
		Matrix3x2(float _11, float _12, float _21, float _22, float _31, float _32) :
			_11(_11), _12(_12), _21(_21), _22(_22), _31(_31), _32(_32) { }

		Matrix3x2 operator * (const Matrix3x2& M) const;
		Matrix3x2& operator *= (const Matrix3x2& M);
		Matrix3x2 operator * (float f) const;
		Matrix3x2& operator *= (float f);

		void ToIdentity();
		void Transpose2x2();
		void Inverse();
		Vector2 TransformNormal(const Vector2& normal) const;
		Vector2 TransformPoint(const Vector2& point) const;
		float Determinant() const;
		
		// Static methods.
		
		static Matrix3x2 Translate(const Vector2& position);
		static Matrix3x2 Rotate(float angle);
		static Matrix3x2 Scale(const Vector2& scale);
	};

	const Matrix3x2 MTX3X2_IDENTITY = Matrix3x2(
		1.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f
	);
};

#endif
