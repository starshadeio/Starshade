//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CVolumeOBB.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CVOLUMEOBB_H
#define CVOLUMEOBB_H

#include "CVolume.h"
#include "../Math/CMathVector3.h"

namespace Physics
{
	class CVolumeOBB : public CVolume
	{
	public:
		struct Data : mData
		{
			Math::Vector3 halfSize;
		};

	public:
		CVolumeOBB(const CVObject* pObject);
		~CVolumeOBB();

		CVolumeOBB(const CVolumeOBB&) = delete;
		CVolumeOBB(CVolumeOBB&&) = delete;
		CVolumeOBB& operator = (const CVolumeOBB&) = delete;
		CVolumeOBB& operator = (CVolumeOBB&&) = delete;
		
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
