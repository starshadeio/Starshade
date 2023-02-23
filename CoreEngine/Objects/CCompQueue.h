//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CCompQueue.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCOMPQUEUE_H
#define CCOMPQUEUE_H

#include "../Globals/CGlobals.h"
#include <algorithm>
#include <vector>
#include <queue>
#include <mutex>

template<typename T>
class CCompQueue
{
public:
	CCompQueue();
	~CCompQueue();
	CCompQueue(const CCompQueue&) = delete;
	CCompQueue(CCompQueue&&) = delete;
	CCompQueue& operator = (const CCompQueue&) = delete;
	CCompQueue& operator = (CCompQueue&&) = delete;

	void Update();
	void Push(T* pComp, u32 layer);

private:
	mutable std::mutex m_mutex;

	u32 m_maxLayer;
	std::vector<std::queue<T*>> m_queueList;
};

#include "CCompQueue.hpp"

#endif