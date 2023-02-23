//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathBounds2D.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATHBOUNDS2D_H
#define CMATHBOUNDS2D_H

#include "CMathVector2.h"

namespace Math
{
	struct Bounds2D
	{
		Math::Vector2 minExtents;
		Math::Vector2 size;

		Bounds2D() { }
		Bounds2D(const Math::Vector2& minExtents, const Math::Vector2& size) :
			minExtents(minExtents), size(size) { }

		bool IsIntersecting(const Vector2& pt) const
		{
			if(pt.x < minExtents.x) return false;
			if(pt.x > minExtents.x + size.x) return false;
			if(pt.y < minExtents.y) return false;
			if(pt.y > minExtents.y + size.y) return false;
			return true;
		}

		bool HasInside(const Bounds2D& bounds) const
		{
			if(bounds.minExtents.x < minExtents.x) return false;
			if(bounds.minExtents.y < minExtents.y) return false;
			if(bounds.minExtents.x + bounds.size.x > minExtents.x + size.x) return false;
			if(bounds.minExtents.y + bounds.size.y > minExtents.y + size.y) return false;
			return true;
		}
	};
};

#endif
