//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathVector2.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATHVECTOR2_H
#define CMATHVECTOR2_H

#include "CMathFloat.h"
#include "CMathVector3.h"
#include "CMathVector4.h"
#include "CSIMDVector.h"

namespace Math
{
	struct Vector2
	{
		union
		{
			struct { float x, y; };
			float v[2];
		};

		Vector2() { }
		Vector2(float f) : x(f), y(f) { }
		Vector2(float x, float y) : x(x), y(y) { }

		// Unary operators.
		inline float& operator [] (size_t index) { return v[index]; }
		inline float operator [] (size_t index) const { return v[index]; }

		inline Vector2 operator + () const { return Vector2(x, y); }
		inline Vector2 operator - () const { return Vector2(-x, -y); }
		
		inline operator const SIMDVector() const { return SIMDVector(x, y, 0.0f); }
		inline operator const Vector3() const { return Vector3(x, y, 0.0f); }
		inline operator const Vector4() const { return Vector4(x, y, 0.0f, 0.0f); }

		// Binary operators.
		inline Vector2 operator + (const Vector2& v) const { return { x + v.x, y + v.y }; }
		inline Vector2 operator + (float f) const { return { x + f, y + f }; }

		inline Vector2 operator - (const Vector2& v) const { return { x - v.x, y - v.y }; }
		inline Vector2 operator - (float f) const { return { x - f, y - f }; }

		inline Vector2 operator * (float f) const { return { x * f, y * f }; }
		inline Vector2 operator / (float f) const { return { x / f, y / f }; }

		// Assignment operators.
		inline Vector2& operator += (Vector2 v)
		{
			x += v.x; y += v.y;
			return *this;
		}

		inline Vector2& operator += (float f)
		{
			x += f; y += f;
			return *this;
		}

		inline Vector2& operator -= (Vector2 v)
		{
			x -= v.x; y -= v.y;
			return *this;
		}

		inline Vector2& operator -= (float f)
		{
			x -= f; y -= f;
			return *this;
		}

		inline Vector2& operator *= (float f)
		{
			x *= f; y *= f;
			return *this;
		}

		inline Vector2& operator /= (float f)
		{
			x /= f; y /= f;
			return *this;
		}

		// Conditional operators.
		inline bool operator == (const Vector2& v) const
		{
			return x == v.x && y == v.y;
		}

		inline bool operator != (const Vector2& v) const
		{
			return x != v.x || y != v.y;
		}

		// Specialty methods.
		static float LengthSq(const Vector2& v) { return v.LengthSq(); }
		static float Length(const Vector2& v) { return v.Length(); }
		static void Normalize(Vector2& v) { v.Normalize(); }
		static Vector2 Normalized(const Vector2& v) { return v.Normalized(); }

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
		}

		inline Vector2 Normalized() const
		{
			const float lenRec = 1.0f / Length();
			return Vector2(x * lenRec, y * lenRec);
		}

		// Products.
		static Vector2 PointwiseProduct(const Vector2& v0, const Vector2& v1) { return v0.PointwiseProduct(v1); }
		static Vector2 PointwiseQuotient(const Vector2& v0, const Vector2& v1) { return v0.PointwiseQuotient(v1); }
		static float Dot(const Vector2& v0, const Vector2& v1) { return v0.Dot(v1); }

		inline Vector2 PointwiseProduct(const Vector2& v) const
		{
			return Vector2(x * v.x, y * v.y);
		}

		inline Vector2 PointwiseQuotient(const Vector2& v) const
		{
			return Vector2(x / v.x, y / v.y);
		}

		inline float Dot(const Vector2& v) const
		{
			return x * v.x + y * v.y;
		}

		// Interpolations.
		static Vector2 Lerp(const Vector2& from, const Vector2& to, float t) { return from.Lerp(to, t); }
		static Vector2 MoveTowards(const Vector2& from, const Vector2& to, float t) { return from.MoveTowards(to, t); }

		inline Vector2 Lerp(const Vector2& to, float t) const
		{
			return *this + (to - *this) * t;
		}

		inline Vector2 MoveTowards(const Vector2& to, float t) const
		{
			const Vector2 diff = to - *this;
			if(diff.LengthSq() < 1e-8f)
			{
				return to;
			}
			
			const Vector2 target = *this + diff.Normalized() * t;
			if(Dot(diff, to - target) <= 0.0f)
			{
				return to;
			}
			
			return target;
		}
	};

	const Vector2 VEC2_ZERO(0.0f);
	const Vector2 VEC2_ONE(1.0f);

	const Vector2 VEC2_RIGHT(1.0f, 0.0f);
	const Vector2 VEC2_LEFT(-1.0f, 0.0f);
	const Vector2 VEC2_UP(0.0f, 1.0f);
	const Vector2 VEC2_DOWN(0.0f, -1.0f);
};

#endif
