//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CGJK.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGJK.h"
#include "CVolume.h"
#include "CRigidbody.h"
#include <Windows.h>
#include <cassert>

namespace Physics
{
	thread_local Math::SIMDMatrix CGJK::m_simplex = { };
	thread_local Math::Vector4 CGJK::m_lambda = { };
	thread_local u32 CGJK::m_simplexSize = 0;

	thread_local const class CVolume* CGJK::m_pVolumeA = nullptr;
	thread_local const class CVolume* CGJK::m_pVolumeB = nullptr;

	static const int MAX_ITERATIONS = 256;

	bool CGJK::Intersection(const class CVolume* pVolumeA, const class CVolume* pVolumeB)
	{
		m_pVolumeA = pVolumeA;
		m_pVolumeB = pVolumeB;

		Math::SIMDVector v = Support(Math::SIMD_VEC_FORWARD);
		Math::SIMDVector w = Support(-v);
		ResetSimplex();
		
		int iterations = 0;
		float err = Math::g_EpsilonTol;
		while(_mm_cvtss_f32(_mm_sub_ps(v.Dot(v), v.Dot(w))) > err && iterations++ < MAX_ITERATIONS)
		{
			AddSimplex(w);

			// Calculate new error from simplex data.
			err = 0.0f;
			for(u32 i = 0; i < m_simplexSize; ++i)
			{
				const float len = _mm_cvtss_f32(m_simplex.vecs[i].LengthSq());
				if(len > err)
				{
					err = len;
				}
			}

			err *= Math::g_EpsilonTol;
			
			if(m_simplexSize == 0) { return false; }
			if(m_simplexSize == 4)
			{ // Intersection found!
				return true;
			}

			w = Support(-v);
		}
		
		return false;
	}

	Math::SIMDVector CGJK::Distance(const class CVolume* pVolumeA, const class CVolume* pVolumeB, bool bInset)
	{
		m_pVolumeA = pVolumeA;
		m_pVolumeB = pVolumeB;

		float ra = bInset ? pVolumeA->GetSkinDepth() : 0.0f;
		float rb = bInset ? pVolumeB->GetSkinDepth() : 0.0f;

		Math::SIMDVector v = Support(Math::SIMD_VEC_FORWARD, ra, rb);
		Math::SIMDVector w = Support(-v, ra, rb);
		ResetSimplex();
		
		int iterations = 0;
		float err = Math::g_EpsilonTol;
		while(_mm_cvtss_f32(_mm_sub_ps(v.Dot(v), v.Dot(w))) > err && iterations++ < MAX_ITERATIONS)
		{
			AddSimplex(w);

			// Calculate new error from simplex data.
			err = 0.0f;
			for(u32 i = 0; i < m_simplexSize; ++i)
			{
				const float len = _mm_cvtss_f32(m_simplex.vecs[i].LengthSq());
				if(len > err)
				{
					err = len;
				}
			}

			err *= Math::g_EpsilonTol;
			
			if(m_simplexSize == 0) { return 0.0f; }
			if(m_simplexSize == 4)
			{ // Intersection found!
				return 0.0f;
			}

			w = Support(-v, ra, rb);
		}
		
		return v;
	}

	bool CGJK::MovingContact(const class CVolume* pVolumeA, const class CVolume* pVolumeB, const Math::SIMDVector& rDir, Math::SIMDVector& contact, Math::SIMDVector& normal, float& t, Math::SIMDVector& sepAxis)
	{
		m_pVolumeA = pVolumeA;
		m_pVolumeB = pVolumeB;

		t = 0.0f;
		contact = 0.0f;
		normal = 0.0f;
		Math::SIMDVector v = sepAxis;

		Math::SIMDVector p;
		ResetSimplex();

		bool bFirst = true;
		bool bHit = true;
		int iterations = 0;
		
		float err = Math::g_EpsilonTol;
		while(bFirst || _mm_cvtss_f32(v.Dot(v)) > err && iterations++ < MAX_ITERATIONS)
		{
			p = Support(-v);

			const float vp = _mm_cvtss_f32(v.Dot(p));
			const float vr = _mm_cvtss_f32(v.Dot(rDir));
			if(vp > vr * t)
			{
				if(vr > 0.0f)
				{
					t = vp / vr;
					if(t > 1.0f)
					{
						bHit = false;
						break;
					}

					contact = rDir * t;
					ResetSimplex();
					normal = -v.Normalized();
				}
				else if(!bFirst)
				{
					bHit = false;
					break;
				}
			}

			bFirst = false;
			AddSimplex(p - contact);

			// Calculate new error from simplex data.
			err = 0.0f;
			for(u32 i = 0; i < m_simplexSize; ++i)
			{
				const float len = _mm_cvtss_f32(m_simplex.vecs[i].LengthSq());
				if(len > err)
				{
					err = len;
				}
			}

			err *= Math::g_EpsilonTol;

			m_simplexSize = TestSimplex(nullptr, &v);
		}

		if(!bHit)
		{
			sepAxis = v;
		}
		
		return bHit;
	}

	bool CGJK::RestingContact(const class CVolume* pVolumeA, const class CVolume* pVolumeB, Math::SIMDVector& contactA, Math::SIMDVector& contactB, Math::SIMDVector& normal)
	{
		m_pVolumeA = pVolumeA;
		m_pVolumeB = pVolumeB;
		
		const float ra = m_pVolumeA->GetSkinDepth();
		const float rb = m_pVolumeB->GetSkinDepth();

		Math::SIMDVector v = Support(Math::SIMD_VEC_FORWARD);
		Math::SIMDVector w = Support(-v);
		ResetSimplex();

		int iterations = 0;
		
		float err = Math::g_EpsilonTol;
		while(iterations++ < MAX_ITERATIONS)
		{
			float vv = _mm_cvtss_f32(v.Dot(v));
			float vw = _mm_cvtss_f32(v.Dot(w));
			if(vv - vw <= err) { break; }
			if(vw > 0.0f && (vw * vw) / vv > powf(ra + rb, 2.0f)) {
				return false;
			}

			AddSimplex(w);
			
			// Calculate new error from simplex data.
			err = 0.0f;
			for(u32 i = 0; i < m_simplexSize; ++i)
			{
				const float len = _mm_cvtss_f32(m_simplex.vecs[i].LengthSq());
				if(len > err)
				{
					err = len;
				}
			}

			err *= Math::g_EpsilonTol;
			
			m_simplexSize = TestSimplex(nullptr, &v);

			w = Support(-v);
		}
		
		if(_mm_cvtss_f32(v.Dot(v)) > err * err)
		{
			float len = _mm_cvtss_f32(v.Length());
			normal = -v / len;
			contactA = -normal * std::max(0.0f, ra - (len - rb));
			contactB = normal * std::max(0.0f, rb - (len - ra));
		}
		else
		{
			// Still can't generate contacts.
			//  For now we're using contracted volumes to generate penetration data.
			//   In the future it might be worth it to put something like EPA in this step for a more robust solution.
			//   Penetration depth should never really be that significant however, so this method might be better than EPA seeing as it's faster.
			v = Support(Math::SIMD_VEC_FORWARD, ra, rb);
			w = Support(-v, ra, rb);
			ResetSimplex();

			iterations = 0;
			err = Math::g_EpsilonTol;
			while(_mm_cvtss_f32(_mm_sub_ps(v.Dot(v), v.Dot(w))) > err && iterations++ < MAX_ITERATIONS)
			{
				AddSimplex(w);
			
				m_simplexSize = TestSimplex(nullptr, &v);

				if(m_simplexSize == 4)
				{ // Intersection found! NOT GOOD!
					return false;
				}

				w = Support(-v, ra, rb);
			}
			
			if(_mm_cvtss_f32(v.LengthSq()) < Math::g_EpsilonTol * Math::g_EpsilonTol)
			{
				return false;
			}

			float len = _mm_cvtss_f32(v.Length());
			normal = -v / len;
			contactA = -normal * std::max(0.0f, ra * 2.0f - (len - rb));
			contactB = normal * std::max(0.0f, rb * 2.0f - (len - ra));
		}

		return true;
	}

	//-----------------------------------------------------------------------------------------------
	// Simplex methods.
	//-----------------------------------------------------------------------------------------------
	
	int CGJK::TestSimplex(Math::SIMDVector* pDir, Math::SIMDVector* pPoint)
	{
		int indices = 0;
		if(m_simplexSize == 1) indices = SimplexPoint(0, pDir, pPoint);
		else if(m_simplexSize == 2) indices = SimplexLine(1, 0, pDir, pPoint);
		else if(m_simplexSize == 3) indices = SimplexTriangle(2, 1, 0, pDir, pPoint);
		else if(m_simplexSize == 4) indices = SimplexTetrahedron(3, 2, 1, 0, pDir, pPoint);

		return indices;
	}

	int CGJK::SimplexPoint(u32 a, Math::SIMDVector* pDir, Math::SIMDVector* pPoint)
	{
		if(pDir) *pDir = -m_simplex.vecs[a];
		if(pPoint) *pPoint = m_simplex.vecs[a];
		SetSimplex(m_simplex.vecs[a]);
		return 1;
	}

	int CGJK::SimplexLine(u32 a, u32 b, Math::SIMDVector* pDir, Math::SIMDVector* pPoint)
	{
		Math::SIMDVector AO = -m_simplex.vecs[a];
		Math::SIMDVector AB = m_simplex.vecs[b] - m_simplex.vecs[a];
		if(DotTest(AB, AO))
		{
			Math::SIMDVector BO = -m_simplex.vecs[b];
			Math::SIMDVector BA = m_simplex.vecs[a] - m_simplex.vecs[b];
			if(DotTest(BA, BO))
			{
				// On edge plane.
				if(pDir) *pDir = Math::SIMDVector::Cross(AB, Math::SIMDVector::Cross(AO, AB));
				if(pPoint) *pPoint = ClosestPoint(m_simplex.vecs[a], m_simplex.vecs[b]);
				SetSimplex(m_simplex.vecs[a], m_simplex.vecs[b]);
				return 2;
			}
			else
			{
				// Behind edge plane B.
				return 0;
			}
		}
		else
		{
			// Behind edge plane A.
			SimplexPoint(a, pDir, pPoint);
			return 1;
		}
	}

	int CGJK::SimplexTriangle(u32 a, u32 b, u32 c, Math::SIMDVector* pDir, Math::SIMDVector* pPoint)
	{
		const Math::SIMDVector AO = -m_simplex.vecs[a];
		const Math::SIMDVector AB = m_simplex.vecs[b] - m_simplex.vecs[a];
		const Math::SIMDVector AC = m_simplex.vecs[c] - m_simplex.vecs[a];
		const Math::SIMDVector ABC = Math::SIMDVector::Cross(AB, AC);

		if(DotTest(-ABC, AO))
		{ // AO is below the triangle.
			SetSimplex(m_simplex.vecs[a], m_simplex.vecs[c], m_simplex.vecs[b]);
			return SimplexTriangle(a, b, c, pDir, pPoint);
		}
		else
		{
			const Math::SIMDVector BO = -m_simplex.vecs[b];
			const Math::SIMDVector CO = -m_simplex.vecs[c];

			if(DotTest(Math::SIMDVector::Cross(ABC, AC), AO))
			{ // Check if AO is outside triangle from edge AC.
				if(DotTest(AC, AO))
				{
					if(DotTest(-AC, CO))
					{
						return SimplexLine(a, c, pDir, pPoint);
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return SimplexLine(a, b, pDir, pPoint);
				}
			}
			else
			{
				if(DotTest(Math::SIMDVector::Cross(AB, ABC), AO))
				{
					if(DotTest(-AB, BO))
					{
						return SimplexLine(a, b, pDir, pPoint);
					}
					else
					{
						return 0;
						SimplexLine(b, c, pDir, pPoint);
					}
				}
				else
				{ // AO is within the triangle ABC.
					if(pDir) *pDir = ABC;
					if(pPoint) *pPoint = ClosestPoint(m_simplex.vecs[a], m_simplex.vecs[b], m_simplex.vecs[c]);
					SetSimplex(m_simplex.vecs[a], m_simplex.vecs[b], m_simplex.vecs[c]);
					return 3;
				}
			}
		}
	}

	int CGJK::SimplexTetrahedron(u32 a, u32 b, u32 c, u32 d, Math::SIMDVector* pDir, Math::SIMDVector* pPoint)
	{
		const Math::SIMDVector AO = -m_simplex.vecs[a];
		const Math::SIMDVector BO = -m_simplex.vecs[b];
		const Math::SIMDVector CO = -m_simplex.vecs[c];
		const Math::SIMDVector AB = m_simplex.vecs[b] - m_simplex.vecs[a];
		const Math::SIMDVector AC = m_simplex.vecs[c] - m_simplex.vecs[a];
		const Math::SIMDVector AD = m_simplex.vecs[d] - m_simplex.vecs[a];
		const Math::SIMDVector BC = m_simplex.vecs[c] - m_simplex.vecs[b];
		const Math::SIMDVector BD = m_simplex.vecs[d] - m_simplex.vecs[b];
		const Math::SIMDVector CD = m_simplex.vecs[d] - m_simplex.vecs[c];
		const Math::SIMDVector ABC = Math::SIMDVector::Cross(AC, AB);
		const Math::SIMDVector ADB = Math::SIMDVector::Cross(AB, AD);
		const Math::SIMDVector ACD = Math::SIMDVector::Cross(AD, AC);
		const Math::SIMDVector BDC = Math::SIMDVector::Cross(BC, BD);

		if(DotTest(-ACD, AO) && DotTest(Math::SIMDVector::Cross(ACD, AD), AO) && DotTest(Math::SIMDVector::Cross(AC, ACD), AO) && DotTest(Math::SIMDVector::Cross(CD, ACD), CO))
		{
			SetSimplex(m_simplex.vecs[a], m_simplex.vecs[c], m_simplex.vecs[d]);
		}
		else if(DotTest(-ADB, AO) && DotTest(Math::SIMDVector::Cross(ADB, AB), AO) && DotTest(Math::SIMDVector::Cross(ADB, BD), BO))
		{
			SetSimplex(m_simplex.vecs[a], m_simplex.vecs[d], m_simplex.vecs[b]);
		}
		else if(DotTest(-ABC, AO) && DotTest(Math::SIMDVector::Cross(BC, ABC), BO))
		{
			SetSimplex(m_simplex.vecs[a], m_simplex.vecs[b], m_simplex.vecs[c]);
		}
		else if(DotTest(-BDC, BO))
		{
			return 0;
		}
		else
		{
			if(pPoint) *pPoint = 0.0f;
			return 4;
		}
		
		return SimplexTriangle(2, 1, 0, pDir, pPoint);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	Math::SIMDVector CGJK::ClosestPoint(const Math::SIMDVector& A, const Math::SIMDVector& B)
	{
		Math::SIMDVector t = B - A;
		return A + t * _mm_cvtss_f32(Math::SIMDVector::Dot(-A, t)) / _mm_cvtss_f32(Math::SIMDVector::Dot(t, t));
	}

	Math::SIMDVector CGJK::ClosestPoint(const Math::SIMDVector& A, const Math::SIMDVector& B, const Math::SIMDVector& C)
	{
		Math::SIMDVector n = (B - A).Cross(C - A);
		return n * (_mm_cvtss_f32(Math::SIMDVector::Dot(A, n)) / _mm_cvtss_f32(Math::SIMDVector::Dot(n, n)));
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	Math::SIMDVector CGJK::Support(const Math::SIMDVector& dir, float insetA, float insetB)
	{
		return m_pVolumeA->SupportPoint(dir, m_pVolumeB, insetA) - m_pVolumeB->SupportPoint(-dir, m_pVolumeB, insetB);
	}
};
