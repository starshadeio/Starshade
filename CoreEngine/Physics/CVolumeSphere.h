//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CVolumeSphere.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CVOLUMESPHERE_H
#define CVOLUMESPHERE_H

#include "CVolume.h"

namespace Physics
{
	class CVolumeSphere : public CVolume
	{
	public:
		struct Data : mData
		{
			float radius;
		};

	public:
		CVolumeSphere(const CVObject* pObject);
		~CVolumeSphere();

		CVolumeSphere(const CVolumeSphere&) = delete;
		CVolumeSphere(CVolumeSphere&&) = delete;
		CVolumeSphere& operator = (const CVolumeSphere&) = delete;
		CVolumeSphere& operator = (CVolumeSphere&&) = delete;
		
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
