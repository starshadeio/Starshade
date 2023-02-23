//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathVectorInt2.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATHVECTORINT2_H
#define CMATHVECTORINT2_H

#include "CMathVectorInt3.h"

namespace Math
{
	struct VectorInt2
	{
		union
		{
			struct { int x, y; };
			int v[2];
		};

		VectorInt2() { }
		VectorInt2(int i) : x(i), y(i) { }
		VectorInt2(int x, int y) : x(x), y(y) { }

		// Unary operators.
		inline int& operator [] (size_t index) { return v[index]; }
		inline int operator [] (size_t index) const { return v[index]; }
		inline VectorInt2 operator + () const { return VectorInt2(x, y); }
		inline VectorInt2 operator - () const { return VectorInt2(-x, -y); }

		inline operator const VectorInt4() { return VectorInt4(x, y, 0, 0); }
		inline operator const VectorInt3() { return VectorInt3(x, y, 0); }

		// Binary operators.
		inline VectorInt2 operator + (const VectorInt2& v) const { return VectorInt2(x + v.x, y + v.y); }
		inline VectorInt2 operator + (int i) const { return VectorInt2(x + i, y + i); }

		inline VectorInt2 operator - (const VectorInt2& v) const { return VectorInt2(x - v.x, y - v.y); }
		inline VectorInt2 operator - (int i) const { return VectorInt2(x - i, y - i); }

		inline VectorInt2 operator * (int i) const { return VectorInt2(x * i, y * i); }
		inline VectorInt2 operator / (int i) const { return VectorInt2(x / i, y / i); }

		// Assignment operators.
		inline VectorInt2& operator += (VectorInt2 v)
		{
			x += v.x; y += v.y;
			return *this;
		}

		inline VectorInt2& operator += (int i)
		{
			x += i; y += i;
			return *this;
		}

		inline VectorInt2& operator -= (VectorInt2 v)
		{
			x -= v.x; y -= v.y;
			return *this;
		}

		inline VectorInt2& operator -= (int i)
		{
			x -= i; y -= i;
			return *this;
		}

		inline VectorInt2& operator *= (int i)
		{
			x *= i; y *= i;
			return *this;
		}

		inline VectorInt2& operator /= (int i)
		{
			x /= i; y /= i;
			return *this;
		}

		// Conditions.
		inline bool operator == (const VectorInt2& v) const
		{
			return x == v.x && y == v.y;
		}

		inline bool operator != (const VectorInt2& v) const
		{
			return x != v.x || y != v.y;
		}

		// Products.
		static VectorInt2 PointwiseProduct(const VectorInt2& v0, const VectorInt2& v1) { return v0.PointwiseProduct(v1); }
		static VectorInt2 PointwiseQuotient(const VectorInt2& v0, const VectorInt2& v1) { return v0.PointwiseQuotient(v1); }

		inline VectorInt2 PointwiseProduct(const VectorInt2& v) const
		{
			return VectorInt2(x * v.x, y * v.y);
		}

		inline VectorInt2 PointwiseQuotient(const VectorInt2& v) const
		{
			return VectorInt2(x / v.x, y / v.y);
		}
	};

	const VectorInt2 VECINT2_ZERO(0);
	const VectorInt2 VECINT2_ONE(1);
};

#endif
