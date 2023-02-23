//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathVectorInt4.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATHVECTORINT4_H
#define CMATHVECTORINT4_H

namespace Math
{
	struct VectorInt4
	{
		union
		{
			struct { int x, y, z, w; };
			int v[4];
		};

		VectorInt4() { }
		VectorInt4(int i) : x(i), y(i), z(i), w(i) { }
		VectorInt4(int x, int y, int z, int w) : x(x), y(y), z(z), w(w) { }

		// Unary operators.
		inline int& operator [] (size_t index) { return v[index]; }
		inline int operator [] (size_t index) const { return v[index]; }
		inline VectorInt4 operator + () const { return VectorInt4(x, y, z, w); }
		inline VectorInt4 operator - () const { return VectorInt4(-x, -y, -z, -w); }

		// Binary operators.
		inline VectorInt4 operator + (const VectorInt4& v) const { return VectorInt4(x + v.x, y + v.y, z + v.z, w + v.w); }
		inline VectorInt4 operator + (int i) const { return VectorInt4(x + i, y + i, z + i, w + i); }

		inline VectorInt4 operator - (const VectorInt4& v) const { return VectorInt4(x - v.x, y - v.y, z - v.z, w - v.w); }
		inline VectorInt4 operator - (int i) const { return VectorInt4(x - i, y - i, z - i, w - i); }

		inline VectorInt4 operator * (int i) const { return VectorInt4(x * i, y * i, z * i, w * i); }
		inline VectorInt4 operator / (int i) const { return VectorInt4(x / i, y / i, z / i, w / i); }

		// Assignment operators.
		inline VectorInt4& operator += (VectorInt4 v)
		{
			x += v.x; y += v.y; z += v.z; w += v.w;
			return *this;
		}

		inline VectorInt4& operator += (int i)
		{
			x += i; y += i; z += i; w += i;
			return *this;
		}

		inline VectorInt4& operator -= (VectorInt4 v)
		{
			x -= v.x; y -= v.y; z -= v.z; w -= v.w;
			return *this;
		}

		inline VectorInt4& operator -= (int i)
		{
			x -= i; y -= i; z -= i; w -= i;
			return *this;
		}

		inline VectorInt4& operator *= (int i)
		{
			x *= i; y *= i; z *= i; w *= i;
			return *this;
		}

		inline VectorInt4& operator /= (int i)
		{
			x /= i; y /= i; z /= i; w /= i;
			return *this;
		}

		// Conditions.
		inline bool operator == (const VectorInt4& v) const
		{
			return x == v.x && y == v.y && z == v.z && w == v.w;
		}

		inline bool operator != (const VectorInt4& v) const
		{
			return x != v.x || y != v.y || z != v.z || w != v.w;
		}

		// Products.
		static VectorInt4 PointwiseProduct(const VectorInt4& v0, const VectorInt4& v1) { return v0.PointwiseProduct(v1); }
		static VectorInt4 PointwiseQuotient(const VectorInt4& v0, const VectorInt4& v1) { return v0.PointwiseQuotient(v1); }

		inline VectorInt4 PointwiseProduct(const VectorInt4& v) const
		{
			return VectorInt4(x * v.x, y * v.y, z * v.z, w * v.w);
		}

		inline VectorInt4 PointwiseQuotient(const VectorInt4& v) const
		{
			return VectorInt4(x / v.x, y / v.y, z / v.z, w / v.w);
		}
	};

	const VectorInt4 VECINT4_ZERO(0);
	const VectorInt4 VECINT4_ONE(1);
};

#endif
