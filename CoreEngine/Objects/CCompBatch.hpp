//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CCompBatch.hpp
//
//-------------------------------------------------------------------------------------------------

#include "CCompBatch.h"

template<typename T>
CCompBatch<T>::CCompBatch(u32 allocSize, u32 defragSteps) :
	m_frame(0),
	m_allocSize(0),
	m_size(0),
	m_defragSteps(defragSteps),
	m_defragIndex(0),
	m_pSlotList(nullptr)
{
	Allocate(allocSize);
}

template<typename T>
CCompBatch<T>::~CCompBatch() { }

template<typename T>
void CCompBatch<T>::Update()
{
	std::lock_guard<std::mutex> lk(m_mutex);

	Defragment(m_defragSteps);

	UpdateElements([this](T& elem) { elem.TryUpdate(m_frame); });
	m_frame ^= 0x1;
}

template<typename T>
void CCompBatch<T>::UpdateElements(std::function<void(T&)> func)
{
	for(u32 i = 0; i < m_size; ++i)
	{
		if(m_pSlotList[i].pSlotRef == nullptr) { continue; }
		func(m_pSlotList[i].elem);
	}
}

template<typename T>
void CCompBatch<T>::Release()
{
	std::lock_guard<std::mutex> lk(m_mutex);

	delete[] m_pSlotList;
	m_pSlotList = nullptr;
	m_size = m_allocSize = 0;
	m_freeSet.clear();
	m_frame = 0;
}

template<typename T>
void CCompBatch<T>::Clear()
{
	std::lock_guard<std::mutex> lk(m_mutex);

	m_size = 0;
	m_freeSet.clear();
	m_frame = 0;
}

template<typename T>
void CCompBatch<T>::Pull(CRef* pRef, const class CVObject* pObject)
{
	std::lock_guard<std::mutex> lk(m_mutex);

	if(m_freeSet.empty())
	{
		Allocate(m_allocSize << 1);
	}

	{
		u32 index = *m_freeSet.begin();
		m_freeSet.erase(index);

		m_size = std::max(m_size, index + 1);
		m_pSlotList[index].UpdateRef(index, pRef);
		pRef->pRef->Reset();
		pRef->pRef->SetObject(pObject, pRef);
	}
}

template<typename T>
void CCompBatch<T>::Free(CRef* pRef)
{
	if(pRef->pRef == nullptr) { return; }

	std::lock_guard<std::mutex> lk(m_mutex);

	m_freeSet.insert(pRef->index);
	pRef->pRef->SetObject(nullptr, nullptr);
	pRef->pRef = nullptr;
	m_defragIndex = std::min(m_defragIndex, pRef->index);

	m_pSlotList[pRef->index].Reset();
}

template<typename T>
void CCompBatch<T>::Allocate(u32 allocSize)
{
	Slot* pSlotList = new Slot[allocSize];

	if(m_pSlotList != nullptr)
	{
		u32 j = 0;
		for(u32 i = 0; i < m_size; ++i)
		{
			if(m_pSlotList[i].pSlotRef == nullptr) { continue; }
			pSlotList[j].elem = std::move(m_pSlotList[i].elem);
			pSlotList[j].UpdateRef(j, m_pSlotList[i].pSlotRef);
			++j;
		}

		m_size = j;
		delete[] m_pSlotList;
		m_freeSet.clear();
	}
	else
	{
		m_size = 0;
	}

	m_pSlotList = pSlotList;
	m_allocSize = allocSize;

	for(u32 i = m_size; i < m_allocSize; ++i)
	{
		m_freeSet.insert(i);
	}
}

template<typename T>
void CCompBatch<T>::Defragment(u32 steps)
{
	for(u32 i = m_defragIndex; i < m_size; ++i)
	{
		if(m_pSlotList[i].pSlotRef == nullptr) { continue; }
		if(m_defragIndex < i)
		{
			m_freeSet.erase(m_defragIndex);
			m_freeSet.insert(i);

			m_pSlotList[m_defragIndex].elem = std::move(m_pSlotList[i].elem);
			m_pSlotList[m_defragIndex].UpdateRef(m_defragIndex, m_pSlotList[i].pSlotRef);
			m_pSlotList[i].Reset();

			while(m_defragIndex < i && m_pSlotList[m_defragIndex].pSlotRef) 
			{
				++m_defragIndex;
			}

			if(steps != 0 && --steps == 0)
			{
				return;
			}
		}
		else
		{
			++m_defragIndex;
		}
	}

	m_size = m_defragIndex;
}
