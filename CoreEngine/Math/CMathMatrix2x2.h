//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathMatrix2x2.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATHMATRIX2X2_H
#define CMATHMATRIX2X2_H

#include "CMathVector2.h"

namespace Math
{
	struct Matrix2x2
	{
		union
		{
			struct
			{
				float _11, _12;
				float _21, _22;
			};

			struct
			{
				Math::Vector2 rows[2];
			};

			float f[4];
		};

		Matrix2x2() { }

		Matrix2x2(float _11, float _12,
			float _21, float _22) :
			_11(_11), _12(_12),
			_21(_21), _22(_22) {
		}

		Matrix2x2(const Math::Vector2& row0, const Math::Vector2& row1) :
			rows { row0, row1 } {
		}

		// Unary operators.
		inline float& operator [] (size_t index) { return f[index]; }
		inline float operator [] (size_t index) const { return f[index]; }

		inline float& At(size_t r, size_t c) { rows[r][c]; }
		inline float At(size_t r, size_t c) const { rows[r][c]; }

		inline Matrix2x2 operator + () const { return *this; }
		inline Matrix2x2 operator - () const { return Matrix2x2(-rows[0], -rows[1]); }

		// Binary operators.
		Matrix2x2 operator + (const Matrix2x2& m) const;
		Matrix2x2 operator + (float f) const;

		Matrix2x2 operator - (const Matrix2x2& m) const;
		Matrix2x2 operator - (float f) const;

		Matrix2x2 operator * (const Matrix2x2& m) const;
		Vector2 operator * (const Vector2& v) const;
		Matrix2x2 operator * (float f) const;

		// Assignment operators.
		Matrix2x2& operator += (const Matrix2x2& m);
		Matrix2x2& operator += (float f);

		Matrix2x2& operator -= (const Matrix2x2& m);
		Matrix2x2& operator -= (float f);

		Matrix2x2& operator *= (const Matrix2x2& m);
		Matrix2x2& operator *= (float f);

		// Methods.
		float Trace() const;
		float Determinate() const;
		void EigenDecomposition(Math::Vector2& eigenvalues, Matrix2x2& eigenvectors) const;
		Matrix2x2 PseudoInverse() const;

		// Static methods.
		static Matrix2x2 FromVector(const Math::Vector2& v0, const Math::Vector2& v1);
		static Matrix2x2 Transpose(const Matrix2x2& m);
	};

	const Matrix2x2 MTX2X2_ZERO(
		0.0f, 0.0f,
		0.0f, 0.0f
	);

	const Matrix2x2 MTX2X2_IDENTITY(
		1.0f, 0.0f,
		0.0f, 1.0f
	);
};

#endif
