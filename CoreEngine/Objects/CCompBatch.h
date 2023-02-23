//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CCompBatch.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCOMPBATCH_H
#define CCOMPBATCH_H

#include "../Globals/CGlobals.h"
#include <algorithm>
#include <functional>
#include <mutex>
#include <unordered_set>

template<typename T>
class CCompBatch
{
public:
	class CRef
	{
	public:
		friend CCompBatch;

	public:
		CRef() : index(0), pRef(nullptr) { }

	private:
		u32 index;

	public:
		T* pRef;
	};

	struct Slot
	{
		T elem;
		CRef* pSlotRef = nullptr;

		void UpdateRef(u32 index, CRef* pSlotRef)
		{
			this->pSlotRef = pSlotRef;
			this->pSlotRef->index = index;
			this->pSlotRef->pRef = &elem;
		}

		void Reset()
		{
			pSlotRef = nullptr;
		}
	};

public:
	CCompBatch(u32 allocSize, u32 defragSteps);
	~CCompBatch();
	CCompBatch(const CCompBatch&) = delete;
	CCompBatch(CCompBatch&&) = delete;
	CCompBatch& operator = (const CCompBatch&) = delete;
	CCompBatch& operator = (CCompBatch&&) = delete;

	void Update();
	void Release();
	void Clear();

	void Pull(CRef* pRef, const class CVObject* pObject);
	void Free(CRef* pRef);

private:
	void UpdateElements(std::function<void(T&)> func);

	void Allocate(u32 allocSize);
	void Defragment(u32 steps);

private:
	mutable std::mutex m_mutex;

	u8 m_frame;

	u32 m_allocSize;
	u32 m_size;
	u32 m_defragSteps;
	u32 m_defragIndex;

	Slot* m_pSlotList;
	std::unordered_set<u32> m_freeSet;
};

#include "CCompBatch.hpp"

#endif
