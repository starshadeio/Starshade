//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CCommandBuffer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CCommandBuffer.h"
#include <cassert>

namespace App
{
	static const size_t HEADER_SIZE = sizeof(size_t) * 2 + sizeof(u16);
	
	CCommandBuffer::CCommandBuffer(size_t allocSize, size_t maxScale, u16 maxAlign) :
		m_maxAlign(maxAlign),
		m_maxSize(allocSize * maxScale),
		m_allocSize(0),
		m_size(0),
		m_head(0),
		m_tail(0),
		m_pData(nullptr)
	{
		 // This should check if allocSize and maxScale are powers of two.
		assert(allocSize > 0 && (allocSize & (allocSize - 1)) == 0);
		assert(maxScale > 0 && (maxScale & (maxScale - 1)) == 0);
		Allocate(allocSize);
	}

	CCommandBuffer::~CCommandBuffer()
	{
	}

	size_t CCommandBuffer::Push(const u8* pData, size_t sz, u16 align)
	{
		assert(align <= m_maxAlign);
		assert(sz + HEADER_SIZE <= m_maxSize);
		
		uintptr_t ptr = reinterpret_cast<uintptr_t>(m_pData + m_tail);
		uintptr_t misalignment = ptr & (align - 1);
		uintptr_t totalSz = sz + misalignment + HEADER_SIZE;
		size_t rem = 0;

		if(m_tail + totalSz > m_allocSize)
		{
			if(m_head < totalSz && m_allocSize < m_maxSize)
			{
				do
				{
					Allocate(m_allocSize << 1);

					ptr = reinterpret_cast<uintptr_t>(m_pData + m_tail);
					misalignment = ptr & (align - 1);
					totalSz = sz + misalignment + HEADER_SIZE;

				} while(m_allocSize < totalSz);
			}
			
			// Check if there is still overflow.
			if(m_tail + totalSz > m_allocSize)
			{
				misalignment = 0; // Misalignment can't exist at the front of the buffer.
				totalSz = sz + HEADER_SIZE;
				rem = m_allocSize - m_tail;
				m_tail = 0;
				
				if(m_head < totalSz)
				{
					m_size -= totalSz - m_head;
					m_head = totalSz;
				}

				// Add in the remainder at the end of the data buffer.
				totalSz += rem;
			}
		}
		else if(m_size > 0 && m_head >= m_tail && m_head < m_tail + totalSz)
		{ // Adjust head so that it doesn't get overlapped by tail.
				m_size -= (m_tail + totalSz) - m_head;
				m_head = m_tail + totalSz;
		}

		{ // Copy data.
			u8* pPtr = m_pData + m_tail;
			
			uintptr_t ptr = reinterpret_cast<uintptr_t>(pPtr);

			pPtr += misalignment;
			memcpy(pPtr, pData, sz);

			pPtr += sz;
			memcpy(pPtr, &totalSz, sizeof(size_t));
			pPtr += sizeof(size_t);
			memcpy(pPtr, &sz, sizeof(size_t));
			pPtr += sizeof(size_t);
			memcpy(pPtr, &align, sizeof(u16));

			m_tail += totalSz - rem;
			m_size += totalSz;
		}

		return totalSz;
	}
	
	bool CCommandBuffer::Pop(std::function<void(const u8*, size_t, u16)> func)
	{
		if(m_size < HEADER_SIZE)
		{
			Clear();
			return false;
		}

		size_t index = m_tail;
		
		// Extract header data.
		const u16 alignment = *(u16*)(m_pData + (index -= sizeof(u16)));
		const size_t dataSz = *(size_t*)(m_pData + (index -= sizeof(size_t)));
		size_t totalSz = *(size_t*)(m_pData + (index -= sizeof(size_t)));
		
		// Check if data exists.
		if(m_size < HEADER_SIZE + dataSz)
		{
			Clear();
			return false;
		}

		// Process data with passed function.
		func(m_pData + (index -= dataSz), dataSz, alignment);
		
		// Adjust tail to reflect pop.
		if(m_size >= totalSz)
		{
			m_size -= totalSz;
			
			if(m_tail >= totalSz)
			{
				m_tail -= totalSz;
			}
			else
			{
				totalSz -= m_tail;
				m_tail = m_allocSize - totalSz;
			}
		}

		return true;
	}
	
	void CCommandBuffer::Clear()
	{
		m_head = 0;
		m_tail = 0;
		m_size = 0;
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CCommandBuffer::Allocate(size_t allocSize)
	{
		u8* pData = reinterpret_cast<u8*>(_aligned_malloc(allocSize, m_maxAlign));
		
		m_head = 0;
		m_tail = 0;
		if(m_pData)
		{
			MoveData(pData, m_size, m_tail);
		}

		m_pData = pData;
		m_allocSize = allocSize;
	}

	// Method for moving m_pData to a new data buffer using DFS seeing as the header is at the end of each block.
	void CCommandBuffer::MoveData(u8* pNewData, size_t oldOffset, size_t& newOffset)
	{
		// Check if header exists in old data..
		if(oldOffset < HEADER_SIZE)
		{
			return;
		}

		size_t index = (m_head + oldOffset) & (m_allocSize - 1);
		if(index == 0) index = m_allocSize;
		
		// Extract header data.
		const u16 alignment = *(u16*)(m_pData + (index -= sizeof(u16)));
		const size_t dataSz = *(size_t*)(m_pData + (index -= sizeof(size_t)));
		size_t totalSz = *(size_t*)(m_pData + (index -= sizeof(size_t)));
		
		// Confirm that all data exists.
		if(oldOffset < dataSz + HEADER_SIZE)
		{
			return;
		}

		// Recurse if additional data exists.
		if(totalSz <= oldOffset)
		{
			// Recurse through old data in a DFS manner.
			MoveData(pNewData, oldOffset - totalSz, newOffset);
		}

		{ // Copy data via DFS.
			u8* pPtr = pNewData + newOffset;
			
			uintptr_t ptr = reinterpret_cast<uintptr_t>(pPtr);
			uintptr_t misalignment = ptr & (alignment - 1);

			pPtr += misalignment;
			memcpy(pPtr, m_pData + (index - dataSz), dataSz);

			totalSz = dataSz + misalignment + HEADER_SIZE;
			
			pPtr += dataSz;
			memcpy(pPtr, &totalSz, sizeof(size_t));
			pPtr += sizeof(size_t);
			memcpy(pPtr, &dataSz, sizeof(size_t));
			pPtr += sizeof(size_t);
			memcpy(pPtr, &alignment, sizeof(u16));

			newOffset += totalSz;
		}
	}
};
