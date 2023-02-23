//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDebug3D.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDEBUG3D_H
#define CDEBUG3D_H

#include <Math/CSIMDMatrix.h>
#include <Math/CMathVector3.h>
#include <Math/CMathColor.h>

namespace Graphics
{
	struct DebugVertex3D
	{
		Math::Vector3 position;
	};
			
	struct DebugInstance3D
	{
		Math::Color color;
		Math::SIMDMatrix world;
	};
};

#endif
