//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Utilities/CWorker.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CWORKER_H
#define CWORKER_H

#include <functional>

namespace Graphics
{
	class CGraphicsWorker;
};

namespace Util
{
	class CWorker
	{
	public:
		CWorker();
		~CWorker();
		CWorker(const CWorker&) = delete;
		CWorker(CWorker&&) = delete;
		CWorker& operator = (const CWorker&) = delete;
		CWorker& operator = (CWorker&&) = delete;

		void Initialize(bool bRecord);
		void ExecuteCPU(std::function<void()> func);
		void ExecuteGraphics(std::function<void()> func);
		void ExecuteCompute(std::function<void()> func);
		void Release();

		// Accessors.
		inline Graphics::CGraphicsWorker* GetGraphicsWorker() const { return m_pGraphicsWorker; }

	private:
		Graphics::CGraphicsWorker* m_pGraphicsWorker;
	};
};

#endif
