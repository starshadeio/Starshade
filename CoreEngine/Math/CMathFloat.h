//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathFloat.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATHFLOAT_H
#define CMATHFLOAT_H

#include <cmath>

namespace Math
{
	// Mathematical constants.
	const float g_Pi = 3.14159265f;
	const float g_2Pi = 2.0f * g_Pi;
	const float g_PiOver2 = 0.5f * g_Pi;
	const float g_PiOver4 = 0.25f * g_Pi;
	const float g_PiOver180 = g_Pi / 180.0f;
	const float g_180OverPi = 180.0f / g_Pi;
	const float g_1OverPi = 1.0f / g_Pi;

	const float g_SqrtOf2 = sqrtf(2.0f);
	const float g_1OverSqrtOf2 = 1.0f / g_SqrtOf2;
	const float g_SqrtOf2Over2 = g_SqrtOf2 / 2.0f;

	const float g_1Over3 = 1.0f / 3.0f;
	const float g_2Over3 = 2.0f / 3.0f;
	const float g_1Over6 = 1.0f / 6.0f;
	const float g_1Over255 = 1.0f / 255.0f;

	const float g_MaxFlt = 3.402823466e+38F;
	const float g_Epsilon = 1.192092896e-07F;
	const float g_EpsilonTol = 1.192092896e-06F;

	const unsigned int g_UIntMax = 0xFFFFFFFF;
	const int g_IntMax = 0x7FFFFFFF;
	const float g_Nan = *reinterpret_cast<const float*>(&g_IntMax);
	const float g_NegNan = *reinterpret_cast<const float*>(&g_UIntMax);

	//
	// Mathematical utilities.
	//

	inline int Sign(int i) { return -(i < 0) + (i > 0); }
	inline float Sign(float f) { return float(-(f < 0.0f) + (f > 0.0f)); }

	inline int Clamp(int i, int mn, int mx) { return (i < mn) ? mn : ((i > mx) ? mx : i); }
	inline float Clamp(float f, float mn, float mx) { return (f < mn) ? mn : ((f > mx) ? mx : f); }

	inline float Frac(float f) { return f - std::floorf(f); }
	inline float Saturate(float f) { return f < 0.0f ? 0.0f : f > 1.0f ? 1.0f : f; }

	inline float SmoothStep(float mn, float mx, float f)
	{
		const float t = Saturate((f - mn) / (mx - mn));
		return t * t * (3.0f - 2.0f * t);
	}

	inline float SmoothStep(float t)
	{
		return t * t * (3.0f - 2.0f * t);
	}

	inline float Quintic(float t)
	{
		return t * t * t * (6.0f * t * t - 15.0f * t + 10.0f);
	}

	inline float WrapRadians(float radians)
	{
		return remainder(radians, g_2Pi);
	}

	//
	// Interpolation.
	//

	inline float Lerp(float from, float to, float t)
	{
		return from + (to - from) * t;
	}

	inline float MoveTowards(float from, float to, float t)
	{
		if(from < to)
		{
			return Clamp(from + t, from, to);
		}
		else
		{
			return Clamp(from - t, to, from);
		}
	}
};

#endif
