//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CVolumeSphere.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CVolumeSphere.h"

namespace Physics
{
	CVolumeSphere::CVolumeSphere(const CVObject* pObject) : CVolume(pObject) { }
	CVolumeSphere::~CVolumeSphere() { }
	
	void CVolumeSphere::UpdateBounds()
	{
		m_minExtents = -m_data.radius;
		m_maxExtents = m_data.radius;
	}
	
	bool CVolumeSphere::RayTest(const QueryRay& query, RaycastInfo& info) const
	{
		const Math::SIMDVector oc = query.ray.GetOrigin() - GetPosition();
		const float a = _mm_cvtss_f32(Math::SIMDVector::Dot(query.ray.GetDirection(), query.ray.GetDirection()));
		const float b = _mm_cvtss_f32(Math::SIMDVector::Dot(oc, query.ray.GetDirection()));
		const float c = _mm_cvtss_f32(Math::SIMDVector::Dot(oc, oc)) - powf(m_data.radius, 2.0f);
		float discriminant = b * b - a * c;
		if(discriminant > 0.0f)
		{
			const float sr = sqrtf(b * b - a * c);
			const float t = std::min((-b - sr) / a, (-b + sr) / a);
			if(t >= 0.0f && t <= query.ray.GetDistance()) {
				info.distance = t;
				info.point = query.ray.GetOrigin() + query.ray.GetDirection() * t;
				info.normal = Math::SIMDVector::Normalized(info.point - GetPosition());
				info.pVolume = this;

				return true;
			}
		}

		return false;
	}

	Math::SIMDVector CVolumeSphere::SupportPoint(const Math::SIMDVector& dir, const CVolume* pVolumeA, float inset) const
	{
		Math::SIMDVector position = GetSolverPosition() - pVolumeA->GetSolverPosition();

		if(_mm_cvtss_f32(dir.LengthSq()) < Math::g_EpsilonTol * Math::g_EpsilonTol)
		{
			return position;
		}

		return position + dir.Normalized() * (m_data.radius - inset);
	}
};
