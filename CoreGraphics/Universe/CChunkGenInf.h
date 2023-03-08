//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunkGenInf.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCHUNKGENINF_H
#define CCHUNKGENINF_H

#include "CChunkGen.h"

namespace Universe
{
	class CChunkGenInf : public CChunkGen
	{
	public:
		CChunkGenInf() { }
		~CChunkGenInf() { }
		CChunkGenInf(const CChunkGenInf&) = delete;
		CChunkGenInf(CChunkGenInf&&) = delete;
		CChunkGenInf& operator = (const CChunkGenInf&) = delete;
		CChunkGenInf& operator = (CChunkGenInf&&) = delete;

		Block Generate(const Math::Vector3& coord) final;

	private:
	};
};

#endif
