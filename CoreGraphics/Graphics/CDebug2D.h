//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDebug2D.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDEBUG2D_H
#define CDEBUG2D_H

#include <Math/CMathMatrix3x2.h>
#include <Math/CMathVector2.h>
#include <Math/CMathColor.h>

namespace Graphics
{
	struct DebugVertex2D
	{
		Math::Vector2 position;
	};
			
	struct DebugInstance2D
	{
		Math::Color color;
		Math::Matrix3x2 world;
	};

	struct DebugDraw2D
	{
		DebugInstance2D instance;
		float time;
	};
};

#endif
