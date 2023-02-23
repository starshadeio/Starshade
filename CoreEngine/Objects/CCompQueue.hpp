//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CCompQueue.hpp
//
//-------------------------------------------------------------------------------------------------

#include "CCompQueue.h"

template<typename T>
CCompQueue<T>::CCompQueue() : 
	m_maxLayer(0)
{
}

template<typename T>
CCompQueue<T>::~CCompQueue()
{
}

template<typename T>
void CCompQueue<T>::Update()
{
	std::lock_guard<std::mutex> lk(m_mutex);

	for(size_t i = 0; i < m_maxLayer; ++i)
	{
		while(!m_queueList[i].empty())
		{
			m_queueList[i].front()->Update();
			m_queueList[i].pop();
		}
	}

	m_maxLayer = 0;
}

template<typename T>
void CCompQueue<T>::Push(T* pComp, u32 layer)
{
	std::lock_guard<std::mutex> lk(m_mutex);

	m_maxLayer = std::max(m_maxLayer, layer + 1);

	while(m_queueList.size() <= m_maxLayer - 1)
	{
		m_queueList.push_back(std::queue<T*>());
	}

	m_queueList[layer].push(pComp);
}
