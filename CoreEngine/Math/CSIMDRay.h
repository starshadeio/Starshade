//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CSIMDRay.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSIMDRAY_H
#define CSIMDRAY_H

#include "CSIMDVector.h"
#include "CMathFloat.h"

namespace Math
{
	class CSIMDRay
	{
	public:
		CSIMDRay() { }
		CSIMDRay(const SIMDVector& origin, const SIMDVector& direction, float distance = 1.0f) :
			m_origin(origin), m_direction(direction), m_distance(distance) { }

		// Accessors.
		inline SIMDVector GetPoint(float t) const { return m_origin + m_direction * t; }
		inline SIMDVector GetPointClamped(float t) const { return m_origin + m_direction * Math::Clamp(t, 0.0f, m_distance); }

		inline const SIMDVector& GetOrigin() const { return m_origin; }
		inline SIMDVector GetEndPoint() const { return m_origin + m_direction * m_distance; }
		inline const SIMDVector& GetDirection() const { return m_direction; }
		inline float GetDistance() const { return m_distance; }

		// Modifiers.
		inline void SetOrigin(const SIMDVector& origin) { m_origin = origin; }
		inline void SetEndPoint(const SIMDVector& endPoint) { m_distance = _mm_cvtss_f32((endPoint - m_origin).Length()); }
		inline void SetDirection(const SIMDVector& direction) { m_direction = direction; }
		inline void SetDistance(float distance) { m_distance = distance; }

	private:
		SIMDVector m_origin;
		SIMDVector m_direction;
		float m_distance;
	};
};

#endif
