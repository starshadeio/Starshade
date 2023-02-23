//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CTSDeque.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CTSQUEUE_H
#define CTSQUEUE_H

#include "CDeque.h"
#include <mutex>

namespace Util
{
	// This creates a contiguous thread-safe double ended queue.
	template<typename T>
	class CTSDeque
	{
	public:
		explicit CTSDeque(size_t allocSize = 4) :
			m_deque(allocSize) { }

		~CTSDeque() { }
		CTSDeque(const CTSDeque&) = delete;
		CTSDeque(CTSDeque&&) = delete;
		CTSDeque& operator = (const CTSDeque&) = delete;
		CTSDeque& operator = (CTSDeque&&) = delete;

		// Push methods.
		void PushBack(T& data)
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			m_deque.PushBack(data);
		}

		void PushFront(T& data)
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			m_deque.PushFront(data);
		}

		// Peek to reference methods.
		bool TryPeekBack(T& data)
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_deque.TryPeekBack(data);
		}

		bool TryPeekFront(T& data)
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_deque.TryPeekFront(data);
		}
		
		// Pop to reference methods.
		bool TryPopBack(T& data)
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_deque.TryPopBack(data);
		}

		bool TryPopFront(T& data)
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_deque.TryPopFront(data);
		}

		// Peek to shared_ptr methods.
		std::shared_ptr<T> TryPeekBack()
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_deque.TryPeekBack();
		}

		std::shared_ptr<T> TryPeekFront()
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_deque.TryPeekFront();
		}
		
		// Pop to shared_ptr methods.
		std::shared_ptr<T> TryPopBack()
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_deque.TryPopBack();
		}

		std::shared_ptr<T> TryPopFront()
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_deque.TryPopFront();
		}

		// Size.
		size_t Clear()
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_deque.Clear();
		}
		
		// Size.
		size_t Size() const
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_deque.Size();
		}

		// Empty?
		bool Empty() const
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			return m_deque.Empty();
		}

		// Quicksort.
		void Sort(std::function<bool(const T&, const T&)> lessFunc, std::function<bool(const T&, const T&)> greaterFunc)
		{
			std::lock_guard<std::mutex> lk(m_mutex);
			m_deque.Sort(lessFunc, greaterFunc);
		}

	private:
		mutable std::mutex m_mutex;
		CDeque<T> m_deque;
	};
};

#endif
