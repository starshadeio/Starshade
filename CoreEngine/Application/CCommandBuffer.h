//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CCommandBuffer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCOMMANDBUFFER_H
#define CCOMMANDBUFFER_H

#include "../Globals/CGlobals.h"
#include <functional>

namespace App
{
	class CCommandBuffer
	{
	public:
		CCommandBuffer(size_t allocSize, size_t maxScale, u16 maxAlign = 256);
		~CCommandBuffer();
		CCommandBuffer(const CCommandBuffer&) = delete;
		CCommandBuffer(CCommandBuffer&&) = delete;
		CCommandBuffer& operator = (const CCommandBuffer&) = delete;
		CCommandBuffer& operator = (CCommandBuffer&&) = delete;

		size_t Push(const u8* pData, size_t sz, u16 align);
		bool Pop(std::function<void(const u8*, size_t, u16)> func);
		void Clear();

		// Accessors.
		const size_t Size() const { return m_size; }
		const bool Empty() const { return m_size == 0; }

	private:
		void Allocate(size_t allocSize);
		void MoveData(u8* pNewData, size_t oldOffset, size_t& newOffset);

	private:
		u16 m_maxAlign;
		size_t m_maxSize;
		size_t m_allocSize;
		size_t m_size;

		size_t m_head;
		size_t m_tail;
		u8* m_pData;
	};
};

#endif
