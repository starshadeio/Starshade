//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Utilities/CGarbage.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGARBAGE_H
#define CGARBAGE_H

#include <Globals/CGlobals.h>
#include <Utilities/CTSDeque.h>
#include <functional>

namespace Util
{
	class CGarbage
	{
	private:
		struct Garbage
		{
			u64 frame;
			std::function<void()> func;
		};

	public:
		CGarbage();
		~CGarbage();
		CGarbage(const CGarbage&) = delete;
		CGarbage(CGarbage&&) = delete;
		CGarbage& operator = (const CGarbage&) = delete;
		CGarbage& operator = (CGarbage&&) = delete;

		void Process();
		void Dispose(std::function<void()> func);
		void Release();

	private:
		Util::CDeque<Garbage> m_garbageDeque;
		Util::CTSDeque<Garbage> m_disposalDeque;
	};
};

#endif
