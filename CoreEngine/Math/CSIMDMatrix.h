//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CSIMDMatrix.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSIMDMATRIX_H
#define CSIMDMATRIX_H

#include "../Globals/CGlobals.h"
#include "../Utilities/CMemAlign.h"
#include "CSIMDVector.h"
#include "CSIMDQuaternion.h"

namespace Math
{
	// This engine assumes column vectors: Ax=b.
	struct SIMDMatrix : public CMemAlign<16>
	{
		union
		{
			float f32[16];
			vf32 rows[4];
			SIMDVector vecs[4];
		};

		// Constructors.
		SIMDMatrix() { }

		SIMDMatrix(vf32 row0, vf32 row1, vf32 row2, vf32 row3)
		{
			rows[0] = row0;
			rows[1] = row1;
			rows[2] = row2;
			rows[3] = row3;
		}

		SIMDMatrix(float _11, float _12, float _13, float _14,
			float _21, float _22, float _23, float _24,
			float _31, float _32, float _33, float _34,
			float _41, float _42, float _43, float _44)
		{
			rows[0] = _mm_setr_ps(_11, _12, _13, _14);
			rows[1] = _mm_setr_ps(_21, _22, _23, _24);
			rows[2] = _mm_setr_ps(_31, _32, _33, _34);
			rows[3] = _mm_setr_ps(_41, _42, _43, _44);
		}

		// Operators.
		SIMDMatrix operator * (const SIMDMatrix& M) const;
		SIMDMatrix& operator *= (const SIMDMatrix& M);

		// Product methods.
		SIMDVector TransformNormal(const SIMDVector& normal) const;
		SIMDVector TransformPoint(const SIMDVector& point) const;

		// Transformation methods.
		static SIMDMatrix Translate(const SIMDVector& position);
		static SIMDMatrix Rotate(const SIMDQuaternion& rotation);
		static SIMDMatrix Rotate(const SIMDVector& euler);
		static SIMDMatrix RotateX(float theta);
		static SIMDMatrix RotateY(float theta);
		static SIMDMatrix RotateZ(float theta);
		static SIMDMatrix Scale(const SIMDVector& scale);

		// General methods.
		void ToIdentity();
		void Transpose();
		void GramSchmidt3x3();

		static SIMDMatrix Transpose(const SIMDMatrix& M);
		static SIMDMatrix LookAt(const SIMDVector& from, const SIMDVector& to);

		// View methods.
		static SIMDMatrix LookAtLH(const SIMDVector& eye, const SIMDVector& at, const SIMDVector& up);
		static SIMDMatrix LookAtRH(const SIMDVector& eye, const SIMDVector& at, const SIMDVector& up);

		// Perspective methods.
		static SIMDMatrix PerspectiveFovLH(float fov, float aspect, float nv, float fv);
		static SIMDMatrix PerspectiveFovLHInv(float fov, float aspect, float nv, float fv);
		static SIMDMatrix PerspectiveFovRH(float fov, float aspect, float nv, float fv);
		static SIMDMatrix PerspectiveFovRHInv(float fov, float aspect, float nv, float fv);

		// Orthographics methods.
		static SIMDMatrix OrthographicLH(float width, float height, float nv, float fv);
		static SIMDMatrix OrthographicLHInv(float width, float height, float nv, float fv);
		static SIMDMatrix OrthographicRH(float width, float height, float nv, float fv);
		static SIMDMatrix OrthographicRHInv(float width, float height, float nv, float fv);
	};

	// Constants.
	const SIMDMatrix SIMD_MTX4X4_IDENTITY = SIMDMatrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	// Constants.
	const SIMDMatrix SIMD_MTX4X4_ZERO = SIMDMatrix(
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f
	);
};

#endif
