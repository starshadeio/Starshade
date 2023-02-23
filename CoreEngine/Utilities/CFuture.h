//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CFuture.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CFUTURE_H
#define CFUTURE_H

#include "../Globals/CGlobals.h"
#include <future>

namespace Util
{
	template<typename T>
	class CFuture
	{
	public:
		CFuture() : m_bReady(false) { }
		CFuture(std::future<T>&& f) : m_bReady(false), m_future(std::move(f)) { }
		CFuture& operator = (std::future<T>&& f)
		{
			m_bReady = false;
			m_future = std::move(f);
			return *this;
		}

		// Unary operators.
		inline operator std::future<T>() { return std::move(m_future); }

		// Accessors.
		inline bool Ready()
		{
			if(m_bReady) return true;
			if(m_future.valid() && m_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
			{
				m_bReady = true;
				return true;
			}

			return !m_future.valid();
		}

	private:
		Abool m_bReady;
		std::future<T> m_future;
	};
};

#endif
