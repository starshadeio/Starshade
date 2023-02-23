//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathColor.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATHCOLOR_H
#define CMATHCOLOR_H

#include "CMathFloat.h"
#include "CMathVector3.h"

namespace Math
{
	struct Color
	{
		union
		{
			struct { float r, g, b, a; };
			struct { float x, y, z, w; };
			float v[4];
		};

		Color() { }
		Color(float f, float a = 1.0f) : r(f), g(f), b(f), a(a) { }
		Color(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) { }

		// Unary operators.
		float& operator [] (size_t index) { return v[index]; }
		inline operator const float* () const { return v; }

		// Binary operators.
		inline Color operator + (const Color& c) const { return Color(r + c.r, g + c.g, b + c.b, a + c.a); }
		inline Color operator + (float f) const { return Color(r + f, g + f, b + f, a + f); }

		inline Color operator - (const Color& c) const { return Color(r - c.r, g - c.g, b - c.b, a - c.a); }
		inline Color operator - (float f) const { return Color(r - f, g - f, b - f, a - f); }

		inline Color operator * (float f) const { return Color(r * f, g * f, b * f, a * f); }
		inline Color operator / (float f) const { return Color(r / f, g / f, b / f, a / f); }

		// Assignment operators.
		inline Color& operator += (Color c)
		{
			r += c.r; g += c.g; b += c.b; a += c.a;
			return *this;
		}

		inline Color& operator += (float f)
		{
			r += f; g += f; b += f; a += f;
			return *this;
		}

		inline Color& operator -= (Color c)
		{
			r -= c.r; g -= c.g; b -= c.b; a -= c.a;
			return *this;
		}

		inline Color& operator -= (float f)
		{
			r -= f; g -= f; b -= f; a -= f;
			return *this;
		}

		inline Color& operator *= (float f)
		{
			r *= f; g *= f; b *= f; a *= f;
			return *this;
		}

		inline Color& operator /= (float f)
		{
			r /= f; g /= f; b /= f; a /= f;
			return *this;
		}

		// Conditionals.
		inline bool operator == (const Color& c) const
		{
			return r == c.r && g == c.g && b == c.b && a == c.a;
		}

		inline bool operator != (const Color& c) const
		{
			return r != c.r || g != c.g || b != c.b || a != c.a;
		}

		// Specialty methods.
		static float LengthSq(const Color& c) { return c.LengthSq(); }
		static float Length(const Color& c) { return c.Length(); }
		static void Normalize(Color& c) { c.Normalize(); }
		static Color Normalized(const Color& c) { return c.Normalized(); }
		static void Saturate(Color& c) { c.Saturate(); }
		static float Grayscale(const Color& c) { return c.Grayscale(); }
		static Vector3 rgb(const Color& c) { return c.rgb(); }

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
			r *= lenRec;
			g *= lenRec;
			b *= lenRec;
			a *= lenRec;
		}

		inline Color Normalized() const
		{
			const float lenRec = 1.0f / Length();
			return Color(r * lenRec, g * lenRec, b * lenRec, a * lenRec);
		}

		inline void Saturate()
		{
			r = Math::Saturate(r);
			g = Math::Saturate(g);
			b = Math::Saturate(b);
			a = Math::Saturate(a);
		}

		inline float Grayscale() const { return (0.299f * r + 0.587f * g + 0.114f * b); }
		inline Vector3 rgb() const { return Vector3(r, g, b); }

		// Products.
		static Color PointwiseProduct(const Color& c0, const Color& c1) { return c0.PointwiseProduct(c1); }
		static Color PointwiseQuotient(const Color& c0, const Color& c1) { return c0.PointwiseQuotient(c1); }
		static float Dot(const Color& c0, const Color& c1) { return c0.Dot(c1); }

		inline Color PointwiseProduct(const Color& c) const
		{
			return Color(r * c.r, g * c.g, b * c.b, a * c.a);
		}

		inline Color PointwiseQuotient(const Color& c) const
		{
			return Color(r / c.r, g / c.g, b / c.b, a / c.a);
		}

		inline float Dot(const Color& c) const
		{
			return r * c.r + g * c.g + b * c.b + a * c.a;
		}

		// Interpolation.
		static Color Lerp(const Color& from, const Color& to, float t) { return from.Lerp(to, t); }
		static Color MoveTowards(const Color& from, const Color& to, float t) { return from.MoveTowards(to, t); }

		inline Color Lerp(const Color& to, float t) const
		{
			return *this + (to - *this) * t;
		}

		inline Color MoveTowards(const Color& to, float t) const
		{
			const Color diff = to - *this;
			if(diff.LengthSq() < 1e-8f)
			{
				return to;
			}
			
			const Color target = *this + diff.Normalized() * t;
			if(Dot(diff, to - target) <= 0.0f)
			{
				return to;
			}

			return target;
		}

		// Conversion methods.
		void CreateFromHUE(float hue);
		void CreateFromHSV(const Color& hsv);
		void CreateFromHSL(const Color& hsl);
		void CreateFromHCY(const Color& hcy);
		void CreateFromHCL(const Color& hcl);
		Color ToHCV();
		Color ToHSV();
		Color ToHSL();
		Color ToHCY();
		Color ToHCL();

		// Static conversion methods.
		static Color ColorFromHUE(float hue);
		static Color ColorFromHSV(const Color& hsv);
		static Color ColorFromHSL(const Color& hsl);
		static Color ColorFromHCY(const Color& hcy);
		static Color ColorFromHCL(const Color& hcl);
		static Color ToHCV(const Color& c);
		static Color ToHSV(const Color& c);
		static Color ToHSL(const Color& c);
		static Color ToHCY(const Color& c);
		static Color ToHCL(const Color& c);

		static Color ToGamma(const Color& c);
		static Color ToLinear(const Color& c);
	};

	const Color COLOR_ONE(1.0f);
	const Color COLOR_ZERO(0.0f, 0.0f);

	const Color COLOR_WHITE(1.0f);
	const Color COLOR_BLACK(0.0f);

	const Color COLOR_RED(1.0f, 0.0f, 0.0f);
	const Color COLOR_YELLOW(1.0f, 1.0f, 0.0f);
	const Color COLOR_GREEN(0.0f, 1.0f, 0.0f);
	const Color COLOR_BLUE(0.0f, 0.0f, 1.0f);
};

#endif
