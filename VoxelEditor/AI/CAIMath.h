//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: AI/CAIMath.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAIMATH_H
#define CAIMATH_H

#include <cmath>
#include <cfloat>

namespace Math
{
	inline float Sigmoid(float x)
	{
		return 1.0f / (1.0f + expf(-x));
	}

	inline float SigmoidToPrime(float x)
	{
		return x * (1.0f - x);
	}

	inline float SigmoidPrime(float x)
	{
		return SigmoidToPrime(Sigmoid(x));
	}

	inline float Softplus(float x)
	{
		return logf(1.0f + expf(x));
	}

	inline float SoftplusPrime(float x)
	{
		return Sigmoid(x);
	}

	inline float LogSoftmax(float x, float* pList, size_t sz)
	{
		float mx = FLT_MIN;
		for(size_t i = 0; i < sz; ++i)
		{
			mx = std::max(mx, pList[i]);
		}

		float sum = 0.0f;
		for(size_t i = 0; i < sz; ++i)
		{
			sum += exp(pList[i] - mx);
		}

		return (x - mx) - log(sum);
	}
};

#endif
