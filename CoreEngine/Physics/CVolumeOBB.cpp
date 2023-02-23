//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CVolumeOBB.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CVolumeOBB.h"

namespace Physics
{
	CVolumeOBB::CVolumeOBB(const CVObject* pObject) : CVolume(pObject) { }
	CVolumeOBB::~CVolumeOBB() { }
	
	void CVolumeOBB::UpdateBounds()
	{
		m_minExtents = m_maxExtents = 0.0f;
		const static Math::SIMDVector UNIT_VERTICES[] = {
			{ -1.0f, -1.0f, -1.0f },
			{  1.0f, -1.0f, -1.0f },
			{ -1.0f, -1.0f,  1.0f },
			{  1.0f, -1.0f,  1.0f },
			{ -1.0f,  1.0f, -1.0f },
			{  1.0f,  1.0f, -1.0f },
			{ -1.0f,  1.0f,  1.0f },
			{  1.0f,  1.0f,  1.0f }
		};

		for(int i = 0; i < 8; ++i)
		{
			Math::Vector3 vertex = *(Math::Vector3*)(GetRotation().Conjugate() * UNIT_VERTICES[i].PointwiseProduct(m_data.halfSize)).ToFloat();
			
			for(int i = 0; i < 3; ++i)
			{
				if(vertex[i] < m_minExtents[i])
				{
					m_minExtents[i] = vertex[i];
				}

				if(vertex[i] > m_maxExtents[i])
				{
					m_maxExtents[i] = vertex[i];
				}
			}
		}
	}

	bool CVolumeOBB::RayTest(const QueryRay& query, RaycastInfo& info) const
	{
		Math::Vector3 localDir = *(Math::Vector3*)(GetRotation().Conjugate() * query.ray.GetDirection()).ToFloat();
		Math::Vector3 localSrc = *(Math::Vector3*)(GetRotation().Conjugate() * query.ray.GetOrigin() - GetPosition()).ToFloat();
		Math::Vector3 mn = -m_data.halfSize;
		Math::Vector3 mx =  m_data.halfSize;

		float tmin = 0.0f;
		float tmax = query.ray.GetDistance();

		// Test for ray intersection with the current AABB.
		for(u32 i = 0; i < 3; ++i)
		{
			float invD = 1.0f / localDir[i];
			float t0 = (mn[i] - localSrc[i]) * invD;
			float t1 = (mx[i] - localSrc[i]) * invD;

			if(invD < 0.0f)
			{
				std::swap(t0, t1);
			}

			tmin = t0 > tmin ? t0 : tmin;
			tmax = t1 < tmax ? t1 : tmax;

			if(tmax <= tmin) { return false; }
		}

		Math::Vector3 pt = localSrc + localDir * tmin;
		Math::Vector3 norm;
		if(pt.x <= mn.x + 1e-5f) norm = Math::VEC3_LEFT;
		if(pt.x >= mx.x - 1e-5f) norm = Math::VEC3_RIGHT;
		if(pt.y <= mn.y + 1e-5f) norm = Math::VEC3_DOWN;
		if(pt.y >= mx.y - 1e-5f) norm = Math::VEC3_UP;
		if(pt.z <= mn.z + 1e-5f) norm = Math::VEC3_BACKWARD;
		if(pt.z >= mx.z - 1e-5f) norm = Math::VEC3_FORWARD;
		
		info.index.U64 = { 0, 0 };
		info.distance = tmin;
		info.normal = GetRotation() * norm;
		info.point = query.ray.GetOrigin() + query.ray.GetDirection() * info.distance;
		info.pVolume = this;


		return true;
	}
	
	Math::SIMDVector CVolumeOBB::SupportPoint(const Math::SIMDVector& dir, const CVolume* pVolumeA, float inset) const
	{
		Math::SIMDVector position = GetSolverPosition() - pVolumeA->GetSolverPosition();

		Math::SIMDVector localDir = GetSolverRotation().Conjugate() * dir;
		Math::SIMDVector maxCorner(_mm_or_ps(_mm_and_ps(localDir.m_xmm, _mm_set_ps1(-0.0f)),
			_mm_setr_ps(m_data.halfSize.x - inset, m_data.halfSize.y - inset, m_data.halfSize.z - inset, 0.0f)));
		return position + GetSolverRotation() * maxCorner;
	}
};
