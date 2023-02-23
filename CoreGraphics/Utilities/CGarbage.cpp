//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Utilities/CGarbage.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGarbage.h"
#include "../Graphics/CGraphicsAPI.h"
#include "../Factory/CFactory.h"
#include <future>

namespace Util
{
	CGarbage::CGarbage()
	{
	}
	
	CGarbage::~CGarbage()
	{
	}
	
	void CGarbage::Process()
	{
		const u64 frame = CFactory::Instance().GetGraphicsAPI()->GetFrame();

		Garbage g;
		while(m_garbageDeque.TryPeekFront(g) && (frame - g.frame) > CFactory::Instance().GetGraphicsAPI()->GetBufferCount())
		{
			m_garbageDeque.PopFront();
			m_disposalDeque.PushBack(g);
		}

		if(m_disposalDeque.Size())
		{
			std::future<void> f = std::async([this](){
				Garbage g;
				while(m_disposalDeque.TryPopFront(g))
				{
					g.func();
				}
			});
		}
	}
	
	void CGarbage::Dispose(std::function<void()> func)
	{
		Garbage g = { CFactory::Instance().GetGraphicsAPI()->GetFrame(), func };
		m_garbageDeque.PushBack(g);
	}
	
	void CGarbage::Release()
	{
		Garbage g;
		while(m_garbageDeque.TryPopFront(g))
		{
			g.func();
		}
	}
};
