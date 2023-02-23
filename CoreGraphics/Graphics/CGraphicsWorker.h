//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CGraphicsWorker.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGRAPHICSWORKER_H
#define CGRAPHICSWORKER_H

#include <functional>

namespace Graphics
{
	class CGraphicsWorker
	{
	public:
		CGraphicsWorker() { }
		virtual ~CGraphicsWorker() { }
		CGraphicsWorker(const CGraphicsWorker&) = delete;
		CGraphicsWorker(CGraphicsWorker&&) = delete;
		CGraphicsWorker& operator = (const CGraphicsWorker&) = delete;
		CGraphicsWorker& operator = (CGraphicsWorker&&) = delete;

		virtual void Initialize(bool bRecord) = 0;
		virtual void Release() = 0;
		
		virtual void Record() = 0;
		virtual void Execute() = 0;

		virtual void LoadAssets(std::function<void()> func) = 0;
		virtual void Sync() = 0;
	};
};

#endif
