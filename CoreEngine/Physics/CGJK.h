//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CGJK.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGJK_H
#define CGJK_H

#include "../Math/CSIMDMatrix.h"
#include "../Math/CMathVector4.h"

namespace Physics
{
	class CGJK
	{
	public:
		static bool Intersection(const class CVolume* pVolumeA, const class CVolume* pVolumeB);
		static Math::SIMDVector Distance(const class CVolume* pVolumeA, const class CVolume* pVolumeB, bool bInset);
		static bool MovingContact(const class CVolume* pVolumeA, const class CVolume* pVolumeB, const Math::SIMDVector& rDir, Math::SIMDVector& contact, Math::SIMDVector& normal, float& t, Math::SIMDVector& sepAxis);
		static bool RestingContact(const class CVolume* pVolumeA, const class CVolume* pVolumeB, Math::SIMDVector& contactA, Math::SIMDVector& contactB, Math::SIMDVector& normal);

	private:
		static int TestSimplex(Math::SIMDVector* pDir, Math::SIMDVector* pPoint);
		static int SimplexPoint(u32 a, Math::SIMDVector* pDir, Math::SIMDVector* pPoint);
		static int SimplexLine(u32 a, u32 b, Math::SIMDVector* pDir, Math::SIMDVector* pPoint);
		static int SimplexTriangle(u32 a, u32 b, u32 c, Math::SIMDVector* pDir, Math::SIMDVector* pPoint);
		static int SimplexTetrahedron(u32 a, u32 b, u32 c, u32 d, Math::SIMDVector* pDir, Math::SIMDVector* pPoint);
		
		static Math::SIMDVector ClosestPoint(const Math::SIMDVector& A, const Math::SIMDVector& B);
		static Math::SIMDVector ClosestPoint(const Math::SIMDVector& A, const Math::SIMDVector& B, const Math::SIMDVector& C);

		static Math::SIMDVector Support(const Math::SIMDVector& dir, float insetA = 0.0f, float insetB = 0.0f);
		
		// Helpers.
		inline static bool DotTest(const Math::SIMDVector& V, const Math::SIMDVector VO) 
		{
			return _mm_cvtss_f32(Math::SIMDVector::Dot(V, VO)) > 0.0f;
		}
		
		inline static void ResetSimplex()
		{
			m_simplexSize = 0;
		}

		inline static void AddSimplex(const Math::SIMDVector& A)
		{
			m_simplex.vecs[m_simplexSize++] = A;
		}
		
		inline static void SetSimplex(Math::SIMDVector A)
		{
			m_simplexSize = 1;
			m_simplex.rows[0] = A.m_xmm;
		}
		
		inline static void SetSimplex(Math::SIMDVector A, Math::SIMDVector B)
		{
			m_simplexSize = 2;
			m_simplex.rows[0] = B.m_xmm;
			m_simplex.rows[1] = A.m_xmm;
		}
		
		inline static void SetSimplex(Math::SIMDVector A, Math::SIMDVector B, Math::SIMDVector C)
		{
			m_simplexSize = 3;
			m_simplex.rows[0] = C.m_xmm;
			m_simplex.rows[1] = B.m_xmm;
			m_simplex.rows[2] = A.m_xmm;
		}

	private:
		static thread_local Math::SIMDMatrix m_simplex;
		static thread_local Math::Vector4 m_lambda;
		static thread_local u32 m_simplexSize;

		static thread_local const class CVolume* m_pVolumeA;
		static thread_local const class CVolume* m_pVolumeB;
	};
};

#endif
