//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathVector4.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATHVECTOR4_H
#define CMATHVECTOR4_H

#include "CMathFloat.h"
//#include "CSIMDVector.h"

namespace Math
{
	struct Vector4
	{
		union
		{
			struct { float x, y, z, w; };
			float v[4];
		};

		Vector4() { }
		Vector4(float f) : x(f), y(f), z(f), w(f) { }
		Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }

		// Unary operators.
		inline float& operator [] (size_t index) { return v[index]; }
		inline float operator [] (size_t index) const { return v[index]; }

		inline Vector4 operator + () const { return Vector4(x, y, z, w); }
		inline Vector4 operator - () const { return Vector4(-x, -y, -z, -w); }

		// Binary operators.
		inline Vector4 operator + (const Vector4& v) const { return { x + v.x, y + v.y, z + v.z, w + v.w }; }
		inline Vector4 operator + (float f) const { return { x + f, y + f, z + f, w + f }; }

		inline Vector4 operator - (const Vector4& v) const { return { x - v.x, y - v.y, z - v.z, w - v.w }; }
		inline Vector4 operator - (float f) const { return { x - f, y - f, z - f, w - f }; }

		inline Vector4 operator * (float f) const { return { x * f, y * f, z * f, w * f }; }
		inline Vector4 operator / (float f) const { return { x / f, y / f, z / f, w / f }; }

		// Assignment operators.
		inline Vector4& operator += (Vector4 v)
		{
			x += v.x; y += v.y; z += v.z; w += v.w;
			return *this;
		}

		inline Vector4& operator += (float f)
		{
			x += f; y += f; z += f; w += f;
			return *this;
		}

		inline Vector4& operator -= (Vector4 v)
		{
			x -= v.x; y -= v.y; z -= v.z; w -= v.w;
			return *this;
		}

		inline Vector4& operator -= (float f)
		{
			x -= f; y -= f; z -= f; w -= f;
			return *this;
		}

		inline Vector4& operator *= (float f)
		{
			x *= f; y *= f; z *= f; w *= f;
			return *this;
		}

		inline Vector4& operator /= (float f)
		{
			x /= f; y /= f; z /= f; w /= f;
			return *this;
		}

		// Conditional operators.
		inline bool operator == (const Vector4& v) const
		{
			return x == v.x && y == v.y && z == v.z && w == v.w;
		}

		inline bool operator != (const Vector4& v) const
		{
			return x != v.x || y != v.y || z != v.z || w != v.w;
		}

		// Specialty methods.
		static float LengthSq(const Vector4& v) { return v.LengthSq(); }
		static float Length(const Vector4& v) { return v.Length(); }
		static void Normalize(Vector4& v) { v.Normalize(); }
		static Vector4 Normalized(const Vector4& v) { return v.Normalized(); }

		inline float LengthSq() const
		{
			return Dot(*this);
		}

		inline float Length() const
		{
			return sqrtf(LengthSq());
		}

		inline void Normalize()
		{
			const float lenRec = 1.0f / Length();
			x *= lenRec;
			y *= lenRec;
			z *= lenRec;
			w *= lenRec;
		}

		inline Vector4 Normalized() const
		{
			const float lenRec = 1.0f / Length();
			return Vector4(x * lenRec, y * lenRec, z * lenRec, w * lenRec);
		}

		// Products.
		static Vector4 PointwiseProduct(const Vector4& v0, const Vector4& v1) { return v0.PointwiseProduct(v1); }
		static Vector4 PointwiseQuotient(const Vector4& v0, const Vector4& v1) { return v0.PointwiseQuotient(v1); }
		static float Dot(const Vector4& v0, const Vector4& v1) { return v0.Dot(v1); }

		inline Vector4 PointwiseProduct(const Vector4& v) const
		{
			return Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
		}

		inline Vector4 PointwiseQuotient(const Vector4& v) const
		{
			return Vector4(x / v.x, y / v.y, z / v.z, w / v.w);
		}

		inline float Dot(const Vector4& v) const
		{
			return x * v.x + y * v.y + z * v.z + w * v.w;
		}

		// Interpolations.
		static Vector4 Lerp(const Vector4& from, const Vector4& to, float t) { return from.Lerp(to, t); }
		static Vector4 MoveTowards(const Vector4& from, const Vector4& to, float t) { return from.MoveTowards(to, t); }

		inline Vector4 Lerp(const Vector4& to, float t) const
		{
			return *this + (to - *this) * t;
		}

		inline Vector4 MoveTowards(const Vector4& to, float t) const
		{
			const Vector4 diff = to - *this;
			if(diff.LengthSq() < 1e-8f)
			{
				return to;
			}

			const Vector4 target = *this + diff.Normalized() * t;
			if(Dot(diff, to - target) <= 0.0f)
			{
				return to;
			}
			
			return target;
		}
	};

	const Vector4 VEC4_ZERO(0.0f);
	const Vector4 VEC4_ONE(1.0f);

	const Vector4 VEC4_RIGHT(1.0f, 0.0f, 0.0f, 0.0f);
	const Vector4 VEC4_LEFT(-1.0f, 0.0f, 0.0f, 0.0f);
	const Vector4 VEC4_UP(0.0f, 1.0f, 0.0f, 0.0f);
	const Vector4 VEC4_DOWN(0.0f, -1.0f, 0.0f, 0.0f);
	const Vector4 VEC4_FORWARD(0.0f, 0.0f, 1.0f, 0.0f);
	const Vector4 VEC4_BACKWARD(0.0f, 0.0f, -1.0f, 0.0f);
};

#endif
