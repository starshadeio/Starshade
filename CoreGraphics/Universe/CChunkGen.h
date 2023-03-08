//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunkGen.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCHUNKGEN_H
#define CCHUNKGEN_H

#include "CChunkData.h"

namespace Universe
{
	class CChunkGen
	{
	public:
		CChunkGen();
		virtual ~CChunkGen();
		CChunkGen(const CChunkGen&) = delete;
		CChunkGen(CChunkGen&&) = delete;
		CChunkGen& operator = (const CChunkGen&) = delete;
		CChunkGen& operator = (CChunkGen&&) = delete;

		virtual void Initialize() { }
		virtual void Release() { }

		virtual Block Generate(const Math::Vector3& coord) = 0;

	private:
	};
};

#endif
