//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CVolumeCapsule.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CVolumeCapsule.h"
#include "../Math/CMathVector3.h"

namespace Physics
{
	CVolumeCapsule::CVolumeCapsule(const CVObject* pObject) : CVolume(pObject) { }
	CVolumeCapsule::~CVolumeCapsule() { }

	void CVolumeCapsule::UpdateBounds()
	{
		Math::SIMDVector d1 = GetRotation() * Math::SIMD_VEC_UP;
		Math::Vector3 p1 = *(Math::Vector3*)((-d1 * m_data.height * (0.5f - m_data.offset)).ToFloat());
		Math::Vector3 q1 = *(Math::Vector3*)((d1 * m_data.height * (0.5f + m_data.offset)).ToFloat());

		for(int i = 0; i < 3; ++i)
		{
			if(p1[i] <= q1[i])
			{
				m_minExtents[i] = p1[i] - m_data.radius;
				m_maxExtents[i] = q1[i] + m_data.radius;
			}
			else
			{
				m_maxExtents[i] = p1[i] + m_data.radius;
				m_minExtents[i] = q1[i] - m_data.radius;
			}
		}
	}
	
	bool CVolumeCapsule::RayTest(const QueryRay& query, RaycastInfo& info) const
	{
		Math::SIMDVector d1 = GetRotation() * Math::SIMD_VEC_UP;
		Math::SIMDVector p1 = GetPosition() - d1 * m_data.height * (0.5f - m_data.offset);
		Math::SIMDVector q1 = GetPosition() + d1 * m_data.height * (0.5f + m_data.offset);
		Math::SIMDVector d2 = query.ray.GetDirection() * query.ray.GetDistance();
		Math::SIMDVector p2 = query.ray.GetOrigin();
		Math::SIMDVector q2 = p2 + d2;

		Math::SIMDVector r = p1 - p1;
		float a = _mm_cvtss_f32(Math::SIMDVector::Dot(d1, d1));
		float e = _mm_cvtss_f32(Math::SIMDVector::Dot(d2, d2));
		float f = _mm_cvtss_f32(Math::SIMDVector::Dot(d2, r));

		float s, t;
		Math::SIMDVector c1, c2;

		if(a <= 1e-5f && e <= 1e-5f)
		{
			s = t = 0.0f;
			c1 = p1;
			c2 = p2;
		}
		else if(a <= 1e-5f)
		{
			s = 0.0f;
			t = Math::Saturate(f / e);
		}
		else
		{
			float c = _mm_cvtss_f32(Math::SIMDVector::Dot(d1, r));
			if(e <= 1e-5f)
			{
				t = 0.0f;
				s = Math::Saturate(-c / a);
			}
			else
			{
				float b = _mm_cvtss_f32(Math::SIMDVector::Dot(d1, d2));
				float denom = a * e - b * b;

				if(denom != 0.0f)
				{
					s = Math::Saturate((b * f - c * e) / denom);
				}
				else
				{
					s = 0.0f;
				}

				t = (b * s + f) / e;

				if(t < 0.0f) {
					t = 0.0f;
					s = Math::Saturate(-c / a);
				}
				else if(t > 1.0f)
				{
					t = 1.0f;
					s = Math::Saturate((b - c) / a);
				}
			}
		}

		c1 = p1 + d1 * s;
		c2 = p2 + d2 * t;

		Math::SIMDVector diff = c2 - c1;
		float sqLen = _mm_cvtss_f32(diff.LengthSq());
		if(sqLen < m_data.radius * m_data.radius)
		{
			return false;
		}

		float len = sqrtf(sqLen);
		info.distance = len - m_data.radius;
		info.normal = diff / len;
		info.point = query.ray.GetOrigin() + query.ray.GetDirection() * info.distance;
		info.index.U64 = { 0, 0 };
		info.pVolume = this;

		return false;
	}

	Math::SIMDVector CVolumeCapsule::SupportPoint(const Math::SIMDVector& dir, const CVolume* pVolumeA, float inset) const
	{
		Math::SIMDVector position = GetSolverPosition() - pVolumeA->GetSolverPosition();

		Math::SIMDVector d1 = GetSolverRotation() * Math::SIMD_VEC_UP;
		Math::SIMDVector p1 = -d1 * m_data.height * (0.5f - m_data.offset);
		Math::SIMDVector q1 =  d1 * m_data.height * (0.5f + m_data.offset);
		
		if(_mm_cvtss_f32(dir.LengthSq()) < Math::g_EpsilonTol * Math::g_EpsilonTol)
		{
			return position;
		}

		float d = _mm_cvtss_f32(Math::SIMDVector::Dot(d1, dir));
		if(d < 0.0f)
		{
			return position + p1 + dir.Normalized() * (m_data.radius - inset);
		}
		else
		{
			return position + q1 + dir.Normalized() * (m_data.radius - inset);
		}
	}
};
