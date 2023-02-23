//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CDeque.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDEQUE_H
#define CDEQUE_H

#include <memory>
#include <functional>

namespace Util
{
	// This creates a contiguous double ended queue.
	template<typename T>
	class CDeque
	{
	public:
		explicit CDeque(size_t allocSize = 4) : m_size(0), m_allocSize(0),
			m_head(0), m_tail(0), m_pDeque(nullptr)
		{
			Allocate(allocSize);
		}

		~CDeque()
		{
			m_size = m_allocSize = 0;
			m_head = m_tail = 0;
			delete[] m_pDeque;
			m_pDeque = nullptr;
		}

		CDeque(const CDeque&) = delete;
		CDeque(CDeque&&) = delete;
		CDeque& operator = (const CDeque&) = delete;
		CDeque& operator = (CDeque&&) = delete;

		// Method for pushing data to the queue.
		void PushBack(T& data)
		{
			size_t tail = (m_tail + 1) % m_allocSize;
			if(tail == m_head)
			{
				// Overflow, reallocate.
				Allocate(m_allocSize << 1);
				tail = m_tail + 1;
			}

			m_pDeque[m_tail] = std::move(data);
			m_tail = tail;
			++m_size;
		}

		// Method for pushing data to the front of the queue.
		void PushFront(T& data)
		{
			size_t head = m_head;
			if(head == 0) head = m_allocSize - 1;
			else --head;
			if(head == m_tail)
			{
				// Overflow, reallocate.
				Allocate(m_allocSize << 1);
				head = m_allocSize - 1;
			}

			m_pDeque[head] = std::move(data);
			m_head = head;
			++m_size;
		}

		// Method for trying to discard data from the front of the queue.
		bool TryDiscardFront()
		{
			if(m_size == 0) return false;
			m_head = (m_head + 1) % m_allocSize;
			--m_size;
			return true;
		}

		// Method for trying to discard data from the back of the queue.
		bool TryDiscardBack()
		{
			if(m_size == 0) return false;
			if(m_tail == 0) m_tail = m_allocSize - 1;
			else --m_tail;
			--m_size;
			return true;
		}

		// Method for popping data from the front of the queue (to reference).
		void PopFront()
		{
			m_head = (m_head + 1) % m_allocSize;
			--m_size;
		}

		// Method for popping data from the back of the queue (to reference).
		void PopBack()
		{
			if(m_tail == 0) m_tail = m_allocSize - 1;
			else --m_tail;
			--m_size;
		}
		
		// Method for trying to pop data from the front of the queue (to reference).
		bool TryPopFront(T& data)
		{
			if(m_size == 0) return false;
			data = std::move(m_pDeque[m_head]);
			m_head = (m_head + 1) % m_allocSize;
			--m_size;
			return true;
		}

		// Method for trying to pop data from the back of the queue (to reference).
		bool TryPopBack(T& data)
		{
			if(m_size == 0) return false;
			if(m_tail == 0) m_tail = m_allocSize - 1;
			else --m_tail;
			--m_size;

			data = std::move(m_pDeque[m_tail]);
			return true;
		}

		// Method for trying to pop data from the front of the queue (with shared pointer).
		std::shared_ptr<T> TryPopFront()
		{
			if(m_size == 0) return std::shared_ptr<T>();
			std::shared_ptr<T> res = std::make_shared<T>(m_pDeque[m_head]);
			m_head = (m_head + 1) % m_allocSize;
			--m_size;
			return res;
		}

		// Method for trying to pop data from the back of the queue (with shared pointer).
		std::shared_ptr<T> TryPopBack()
		{
			if(m_size == 0) return std::shared_ptr<T>();
			if(m_tail == 0) m_tail = m_allocSize - 1;
			else --m_tail;
			--m_size;

			std::shared_ptr<T> res = std::make_shared<T>(m_pDeque[m_tail]);
			return res;
		}

		// Method for peeking at the front of the queue.
		const T& PeekFront() const
		{
			return m_pDeque[m_head];
		}
		
		// Method for peeking at the front of the queue.
		const T& PeekBack() const
		{
			size_t tail = m_tail;
			if(tail == 0) tail = m_allocSize - 1;
			else --tail;

			return m_pDeque[tail];
		}
		
		// Method for getting a copy of the next element in the front of the queue.
		bool TryPeekFront(T& data) const
		{
			if(m_size == 0) return false;
			data = m_pDeque[m_head];
			return true;
		}

		// Method for getting a copy of the next element in the back of the queue.
		bool TryPeekBack(T& data) const
		{
			if(m_size == 0) return false;

			size_t tail = m_tail;
			if(tail == 0) tail = m_allocSize - 1;
			else --tail;

			data = m_pDeque[tail];
			return true;
		}

		// Method for getting a copy of the next element in the front of the queue.
		bool TryPeekFront(T** ppData)
		{
			if(m_size == 0) return false;
			*ppData = &m_pDeque[m_head];
			return true;
		}

		// Method for getting a copy of the next element in the back of the queue.
		bool TryPeekBack(T** ppData) const
		{
			if(m_size == 0) return false;

			size_t tail = m_tail;
			if(tail == 0) tail = m_allocSize - 1;
			else --tail;

			*ppData = &m_pDeque[tail];
			return true;
		}

		// Method for getting queue size.
		size_t Size() const
		{
			return m_size;
		}

		// Method for checking if the queue is empty.
		bool Empty() const
		{
			return m_size == 0;
		}

		void Clear()
		{
			m_size = m_head = m_tail = 0;
		}

		void Sort(std::function<bool(const T&, const T&)> lessFunc, std::function<bool(const T&, const T&)> greaterFunc)
		{
			QSort(0, int(m_size) - 1, lessFunc, greaterFunc);
		}

	private:
		// Basic implementation of random quicksort.
		void QSort(int l, int r, std::function<bool(const T&, const T&)> lessFunc, std::function<bool(const T&, const T&)> greaterFunc)
		{
			if(l >= r) return;
			int left = l;
			int right = r;

			const T& pivot = m_pDeque[(m_head + ((r + l) >> 1)) % m_size];
			T* pElemLeft = &m_pDeque[(m_head + left) % m_size];
			T* pElemRight = &m_pDeque[(m_head + right) % m_size];

			while(left <= right)
			{
				while(lessFunc(*pElemLeft, pivot))
				{
					pElemLeft = &m_pDeque[(m_head + ++left) % m_size];
				}

				while(greaterFunc(*pElemRight, pivot))
				{
					pElemRight = &m_pDeque[(m_head + --right) % m_size];
				}

				if(left <= right)
				{
					if(left != right)
					{
						T tmp = *pElemLeft;
						*pElemLeft = *pElemRight;
						*pElemRight = tmp;
					}

					++left; --right;
				}
			}

			QSort(l, right, lessFunc, greaterFunc);
			QSort(left, r, lessFunc, greaterFunc);
		}

	private:
		// Method for allocating, or reallocating space to the queue.
		void Allocate(size_t allocSize)
		{
			T* pQueue = new T[allocSize];
			if(m_pDeque)
			{ // Reallocating.
				size_t index = 0;

				while(m_head != m_tail)
				{ // Push data from past queue to new.
					pQueue[index++] = std::move(m_pDeque[m_head++]);
					if(m_head >= m_allocSize) m_head = 0;
				}

				// Update indices, and destroy old queue.
				m_head = 0;
				m_tail = index;
				delete[] m_pDeque;
			}

			m_pDeque = pQueue;
			m_allocSize = allocSize;
		}

	private:
		size_t m_size;
		size_t m_allocSize;

		size_t m_head;
		size_t m_tail;

		T* m_pDeque;
	};
};

#endif
