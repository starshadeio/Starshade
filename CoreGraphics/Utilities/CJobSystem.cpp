//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Utilities/CJobSystem.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CJobSystem.h"
#include "CWorker.h"
#include "../Graphics/CGraphicsWorker.h"
#include "../Graphics/CGraphicsAPI.h"
#include "../Factory/CFactory.h"

namespace Util
{
	thread_local CWorker CJobSystem::m_worker;

	CJobSystem::CJobSystem() : 
		m_exitFlag(false),
		m_threadCount(4)
	{
	}

	CJobSystem::~CJobSystem() { }
	
	void CJobSystem::Initialize()
	{
		m_worker.Initialize(true);
	}
	
	void CJobSystem::Open()
	{
		for(u32 i = 0; i < m_threadCount; ++i)
		{
			std::promise<void> p;
			std::future<void> f = p.get_future();
			m_threadDeque.PushBack(f);
			std::thread t(&CJobSystem::JobThread, this, std::move(p));
			t.detach();
		}
	}
	
	void CJobSystem::Execute()
	{
		m_worker.GetGraphicsWorker()->Execute();
	}
	
	void CJobSystem::Sync()
	{
		m_worker.GetGraphicsWorker()->Sync();
	}

	void CJobSystem::Process()
	{
		std::packaged_task<void()> task;
		while(m_syncDeque.TryPopFront(task))
		{
			task();
		}
	}
	
	void CJobSystem::Close()
	{
		m_exitFlag = true;
		for(u32 i = 0; i < m_threadCount; ++i)
		{
			std::future<void> f;
			while(m_threadDeque.TryPopFront(f))
			{
				f.wait();
			}
		}
	}
	
	void CJobSystem::Release()
	{
		m_worker.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Method for posting work to jobs system.
	//-----------------------------------------------------------------------------------------------

	std::future<void> CJobSystem::JobCPU(std::function<void()> func, bool bAsync)
	{
		std::packaged_task<void()> task([=](){
			m_worker.ExecuteCPU(func);
		});

		std::future<void> f = task.get_future();

		if(bAsync)
		{
			m_asyncDeque.PushBack(task);
		}
		else
		{
			m_syncDeque.PushBack(task);
		}

		return f;
	}

	std::future<void> CJobSystem::JobGraphics(std::function<void()> func, bool bAsync)
	{
		std::packaged_task<void()> task([=](){
			m_worker.ExecuteGraphics(func);
		});

		std::future<void> f = task.get_future();
		
		if(bAsync)
		{
			m_asyncDeque.PushBack(task);
		}
		else
		{
			m_syncDeque.PushBack(task);
		}

		return f;
	}

	std::future<void> CJobSystem::JobCompute(std::function<void()> func, bool bAsync)
	{
		std::packaged_task<void()> task([=](){
			m_worker.ExecuteCompute(func);
		});

		std::future<void> f = task.get_future();
		
		if(bAsync)
		{
			m_asyncDeque.PushBack(task);
		}
		else
		{
			m_syncDeque.PushBack(task);
		}

		return f;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Jobs thread, each with a separate worker.
	//-----------------------------------------------------------------------------------------------

	void CJobSystem::JobThread(std::promise<void> p)
	{
		// Initialize per thread worker.
		m_worker.Initialize(false);
		std::packaged_task<void()> task;

		while(!m_exitFlag)
		{
			if(m_asyncDeque.TryPopFront(task))
			{
				task();
				std::this_thread::yield();
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			}
		}

		m_worker.Release();

		p.set_value();
	}
};
