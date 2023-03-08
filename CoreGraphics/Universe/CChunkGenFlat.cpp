//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunkGenFlat.h
//
//-------------------------------------------------------------------------------------------------

#include "CChunkGenFlat.h"

namespace Universe
{
	Block CChunkGenFlat::Generate(const Math::Vector3& coord)
	{
		if(coord.y <= 0.0f)
		{
			return Block(coord.y <= -1.0f ? 8 : 0, true);
		}
		else
		{
			return Block(0, false);
		}
	}
};
