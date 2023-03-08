//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunkGenNull.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCHUNKGENNULL_H
#define CCHUNKGENNULL_H

#include "CChunkGen.h"

namespace Universe
{
	class CChunkGenNull : public CChunkGen
	{
	public:
		CChunkGenNull() { }
		~CChunkGenNull() { }
		CChunkGenNull(const CChunkGenNull&) = delete;
		CChunkGenNull(CChunkGenNull&&) = delete;
		CChunkGenNull& operator = (const CChunkGenNull&) = delete;
		CChunkGenNull& operator = (CChunkGenNull&&) = delete;

		Block Generate(const Math::Vector3& coord)  final { return Block(0, false); }

	private:
	};
};

#endif
