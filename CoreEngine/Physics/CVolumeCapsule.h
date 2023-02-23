//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CVolumeCapsule.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CVOLUMECAPSULE_H
#define CVOLUMECAPSULE_H

#include "CVolume.h"

namespace Physics
{
	class CVolumeCapsule : public CVolume
	{
	public:
		struct Data : mData
		{
			float radius;
			float height;
			float offset;
		};

	public:
		CVolumeCapsule(const CVObject* pObject);
		~CVolumeCapsule();

		CVolumeCapsule(const CVolumeCapsule&) = delete;
		CVolumeCapsule(CVolumeCapsule&&) = delete;
		CVolumeCapsule& operator = (const CVolumeCapsule&) = delete;
		CVolumeCapsule& operator = (CVolumeCapsule&&) = delete;
		
		void UpdateBounds() final;
		bool RayTest(const QueryRay& query, RaycastInfo& info) const final;
		Math::SIMDVector SupportPoint(const Math::SIMDVector& dir, const CVolume* pVolumeA, float inset = 0.0f) const final;

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	protected:
		// Accessors.
		virtual inline const mData& GetData() const final { return m_data; }

	private:
		Data m_data;
	};
};

#endif
