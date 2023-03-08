//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunkGenFlat.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCHUNKGENFLAT_H
#define CCHUNKGENFLAT_H

#include "CChunkGen.h"

namespace Universe
{
	class CChunkGenFlat : public CChunkGen
	{
	public:
		CChunkGenFlat() { }
		~CChunkGenFlat() { }
		CChunkGenFlat(const CChunkGenFlat&) = delete;
		CChunkGenFlat(CChunkGenFlat&&) = delete;
		CChunkGenFlat& operator = (const CChunkGenFlat&) = delete;
		CChunkGenFlat& operator = (CChunkGenFlat&&) = delete;

		Block Generate(const Math::Vector3& coord) final;

	private:
	};
};

#endif
