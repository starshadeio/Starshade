//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Utilities/CJobSystem.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CJOBSYSTEM_H
#define CJOBSYSTEM_H

#include "CWorker.h"
#include <Globals/CGlobals.h>
#include <Utilities/CTSDeque.h>
#include <Utilities/CDeque.h>
#include <functional>
#include <future>

namespace Util
{
	class CJobSystem
	{
	private:
		enum class JobType
		{
			CPU,
			Graphics,
			Compute,
		};

	public:
		static CJobSystem& Instance()
		{
			static CJobSystem instance;
			return instance;
		}

	private:
		CJobSystem();
		~CJobSystem();
		CJobSystem(const CJobSystem&) = delete;
		CJobSystem(CJobSystem&&) = delete;
		CJobSystem& operator = (const CJobSystem&) = delete;
		CJobSystem& operator = (CJobSystem&&) = delete;

	public:
		void Initialize();
		void Open();

		void Execute();
		void Sync();
		void Process();

		void Close();
		void Release();

		std::future<void> JobCPU(std::function<void()> func, bool bAsync);
		std::future<void> JobGraphics(std::function<void()> func, bool bAsync);
		std::future<void> JobCompute(std::function<void()> func, bool bAsync);

		// Accessors.
		static CWorker& GetWorker() { return m_worker; }

	private:
		void JobThread(std::promise<void> p);

	private:
		Abool m_exitFlag;
		u32 m_threadCount;

		Util::CDeque<std::packaged_task<void()>> m_syncDeque;
		Util::CTSDeque<std::packaged_task<void()>> m_asyncDeque;
		Util::CDeque<std::future<void>> m_threadDeque;

		static thread_local CWorker m_worker;
	};
};

#endif
