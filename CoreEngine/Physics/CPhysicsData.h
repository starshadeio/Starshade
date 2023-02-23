//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CPhysicsData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPHYSICSDATA_H
#define CPHYSICSDATA_H

#include "../Math/CSIMDRay.h"
#include "../Globals/CGlobals.h"
#include <functional>
#include <vector>

namespace Physics
{
	enum class ColliderType
	{
		None,
		Trigger,
		Collider,
	};

	struct ContactData
	{
		Math::SIMDVector hitPoint;
		Math::SIMDVector hitNormal;
		float interval;
		class CVolume* pVolume;
		int index;
	};

	struct RaycastInfo
	{
		union Index
		{
			struct U8
			{
				u8 lo[8];
				u8 hi[8];
			} U8;
			
			struct U16
			{
				u16 lo[4];
				u16 hi[4];
			} U16;
			
			struct U32
			{
				u32 lo[2];
				u32 hi[2];
			} U32;

			struct U64
			{
				u64 lo;
				u64 hi;
			} U64;
		} index, alt;

		//int index;
		float distance;
		const class CVolume* pVolume;
		Math::SIMDVector normal;
		Math::SIMDVector point;
	};

	struct QueryRay
	{
		Math::CSIMDRay ray;
		std::function<void(const std::vector<RaycastInfo>&)> callback;
	};
};

#endif
