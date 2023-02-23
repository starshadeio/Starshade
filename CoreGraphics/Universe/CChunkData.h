//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunkData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCHUNKDATA_H
#define CCHUNKDATA_H

#include <Globals/CGlobals.h>
#include <Math/CMathVector3.h>

typedef u8 BlockId;
typedef Au8 AblockId;

namespace Universe
{
	enum SIDE : u8
	{
		SIDE_LEFT,
		SIDE_RIGHT,
		SIDE_BOTTOM,
		SIDE_TOP,
		SIDE_BACK,
		SIDE_FRONT,
	};
	
	enum SIDE_FLAG : u8
	{
		SIDE_FLAG_LEFT = 0x1 << SIDE_LEFT,
		SIDE_FLAG_RIGHT = 0x1 << SIDE_RIGHT,
		SIDE_FLAG_BOTTOM = 0x1 << SIDE_BOTTOM,
		SIDE_FLAG_TOP = 0x1 << SIDE_TOP,
		SIDE_FLAG_BACK = 0x1 << SIDE_BACK,
		SIDE_FLAG_FRONT = 0x1 << SIDE_FRONT,
	};

	const Math::Vector3 SIDE_NORMAL[] = {
		Math::VEC3_LEFT,
		Math::VEC3_RIGHT,
		Math::VEC3_DOWN,
		Math::VEC3_UP,
		Math::VEC3_BACKWARD,
		Math::VEC3_FORWARD,
	};

	struct Block
	{
		BlockId id;
		bool bEmpty : 1;
		u8 sideFlag : 7;
		u16 padding; // For future use.
	};
};

#endif
