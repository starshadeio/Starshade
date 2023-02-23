//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathVector3.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATHVECTOR3_H
#define CMATHVECTOR3_H

#include "CMathFloat.h"
#include "CMathVector4.h"
#include "CSIMDVector.h"

namespace Math
{
	struct Vector3
	{
		union
		{
			struct { float x, y, z; };
			float v[3];
		};

		Vector3() { }
		Vector3(float f) : x(f), y(f), z(f) { }
		Vector3(float x, float y, float z) : x(x), y(y), z(z) { }

		// Unary operators.
		inline float& operator [] (size_t index) { return v[index]; }
		inline float operator [] (size_t index) const { return v[index]; }

		inline Vector3 operator + () const { return Vector3(x, y, z); }
		inline Vector3 operator - () const { return Vector3(-x, -y, -z); }

		inline operator const SIMDVector() const { return SIMDVector(x, y, z); }
		inline operator const Vector4() const { return Vector4(x, y, z, 0.0f); }

		// Binary operators.
		inline Vector3 operator + (const Vector3& v) const { return { x + v.x, y + v.y, z + v.z }; }
		inline Vector3 operator + (float f) const { return { x + f, y + f, z + f }; }

		inline Vector3 operator - (const Vector3& v) const { return { x - v.x, y - v.y, z - v.z }; }
		inline Vector3 operator - (float f) const { return { x - f, y - f, z - f }; }

		inline Vector3 operator * (float f) const { return { x * f, y * f, z * f }; }
		inline Vector3 operator / (float f) const { return { x / f, y / f, z / f }; }

		// Assignment operators.
		inline Vector3& operator += (Vector3 v)
		{
			x += v.x; y += v.y; z += v.z;
			return *this;
		}

		inline Vector3& operator += (float f)
		{
			x += f; y += f; z += f;
			return *this;
		}

		inline Vector3& operator -= (Vector3 v)
		{
			x -= v.x; y -= v.y; z -= v.z;
			return *this;
		}

		inline Vector3& operator -= (float f)
		{
			x -= f; y -= f; z -= f;
			return *this;
		}

		inline Vector3& operator *= (float f)
		{
			x *= f; y *= f; z *= f;
			return *this;
		}

		inline Vector3& operator /= (float f)
		{
			x /= f; y /= f; z /= f;
			return *this;
		}

		// Conditional operators.
		inline bool operator == (const Vector3& v) const
		{
			return x == v.x && y == v.y && z == v.z;
		}

		inline bool operator != (const Vector3& v) const
		{
			return x != v.x || y != v.y || z != v.z;
		}

		// Specialty methods.
		static float LengthSq(const Vector3& v) { return v.LengthSq(); }
		static float Length(const Vector3& v) { return v.Length(); }
		static void Normalize(Vector3& v) { v.Normalize(); }
		static Vector3 Normalized(const Vector3& v) { return v.Normalized(); }

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
		}

		inline Vector3 Normalized() const
		{
			const float lenRec = 1.0f / Length();
			return Vector3(x * lenRec, y * lenRec, z * lenRec);
		}

		// Products.
		static Vector3 PointwiseProduct(const Vector3& v0, const Vector3& v1) { return v0.PointwiseProduct(v1); }
		static Vector3 PointwiseQuotient(const Vector3& v0, const Vector3& v1) { return v0.PointwiseQuotient(v1); }
		static float Dot(const Vector3& v0, const Vector3& v1) { return v0.Dot(v1); }
		static Vector3 Cross(const Vector3& v0, const Vector3& v1) { return v0.Cross(v1); }

		inline Vector3 PointwiseProduct(const Vector3& v) const
		{
			return Vector3(x * v.x, y * v.y, z * v.z);
		}

		inline Vector3 PointwiseQuotient(const Vector3& v) const
		{
			return Vector3(x / v.x, y / v.y, z / v.z);
		}

		inline float Dot(const Vector3& v) const
		{
			return x * v.x + y * v.y + z * v.z;
		}

		inline Vector3 Cross(const Vector3& v) const
		{
			return Vector3(
				y * v.z - z * v.y,
				z * v.x - x * v.z,
				x * v.y - y * v.x
			);
		}

		// Interpolations.
		static Vector3 Lerp(const Vector3& from, const Vector3& to, float t) { return from.Lerp(to, t); }
		static Vector3 MoveTowards(const Vector3& from, const Vector3& to, float t) { return from.MoveTowards(to, t); }

		inline Vector3 Lerp(const Vector3& to, float t) const
		{
			return *this + (to - *this) * t;
		}

		inline Vector3 MoveTowards(const Vector3& to, float t) const
		{
			const Vector3 diff = to - *this;
			if(diff.LengthSq() < 1e-8f)
			{
				return to;
			}

			const Vector3 target = *this + diff.Normalized() * t;
			if(Dot(diff, to - target) <= 0.0f)
			{
				return to;
			}

			return target;
		}
	};

	const Vector3 VEC3_ZERO(0.0f);
	const Vector3 VEC3_ONE(1.0f);

	const Vector3 VEC3_RIGHT(1.0f, 0.0f, 0.0f);
	const Vector3 VEC3_LEFT(-1.0f, 0.0f, 0.0f);
	const Vector3 VEC3_UP(0.0f, 1.0f, 0.0f);
	const Vector3 VEC3_DOWN(0.0f, -1.0f, 0.0f);
	const Vector3 VEC3_FORWARD(0.0f, 0.0f, 1.0f);
	const Vector3 VEC3_BACKWARD(0.0f, 0.0f, -1.0f);
};

#endif
