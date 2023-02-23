//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathMatrix3x3.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATHMATRIX3X3_H
#define CMATHMATRIX3X3_H

#include "CMathVector3.h"

namespace Math
{
	struct Matrix3x3
	{
		union
		{
			struct
			{
				float _11, _12, _13;
				float _21, _22, _23;
				float _31, _32, _33;
			};

			struct
			{
				Math::Vector3 rows[3];
			};

			float f[9];
		};

		Matrix3x3() { }

		Matrix3x3(float _11, float _12, float _13,
			float _21, float _22, float _23,
			float _31, float _32, float _33) :
			_11(_11), _12(_12), _13(_13),
			_21(_21), _22(_22), _23(_23),
			_31(_31), _32(_32), _33(_33) {
		}

		Matrix3x3(const Math::Vector3& row0, const Math::Vector3& row1, const Math::Vector3& row2) :
			rows { row0, row1, row2 } {
		}

		Matrix3x3(const Math::Vector3& v0, const Math::Vector3& v1) :
			Matrix3x3(v0[0] * v1[0], v0[0] * v1[1], v0[0] * v1[2],
				v0[1] * v1[0], v0[1] * v1[1], v0[1] * v1[2],
				v0[2] * v1[0], v0[2] * v1[1], v0[2] * v1[2]) {
		}

		// Unary operators.
		inline float& operator [] (size_t index) { return f[index]; }
		inline float operator [] (size_t index) const { return f[index]; }

		inline float& At(size_t r, size_t c) { return rows[r][c]; }
		inline float At(size_t r, size_t c) const { return rows[r][c]; }

		inline Matrix3x3 operator + () const { return *this; }
		inline Matrix3x3 operator - () const { return Matrix3x3(-rows[0], -rows[1], -rows[2]); }

		// Binary operators.
		Matrix3x3 operator + (const Matrix3x3& m) const;
		Matrix3x3 operator + (float f) const;

		Matrix3x3 operator - (const Matrix3x3& m) const;
		Matrix3x3 operator - (float f) const;

		Matrix3x3 operator * (const Matrix3x3& m) const;
		Vector3 operator * (const Vector3& v) const;
		Matrix3x3 operator * (float f) const;

		// Assignment operators.
		Matrix3x3& operator += (const Matrix3x3& m);
		Matrix3x3& operator += (float f);

		Matrix3x3& operator -= (const Matrix3x3& m);
		Matrix3x3& operator -= (float f);

		Matrix3x3& operator *= (const Matrix3x3& m);
		Matrix3x3& operator *= (float f);

		// Methods.
		float Trace() const;
		float Determinate() const;
		void EigenDecomposition(Math::Vector3& eigenvalues, Matrix3x3& eigenvectors) const;
		Matrix3x3 PseudoInverse() const;

		// Static methods.
		static Matrix3x3 Transpose(const Matrix3x3& m);
	};

	const Matrix3x3 MTX3X3_ZERO(
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f
	);

	const Matrix3x3 MTX3X3_IDENTITY(
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	);
};

#endif
