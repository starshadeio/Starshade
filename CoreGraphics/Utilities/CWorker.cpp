//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Utilities/CWorker.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CWorker.h"
#include "../Graphics/CGraphicsWorker.h"
#include "../Factory/CFactory.h"
#include <Utilities/CMemoryFree.h>

namespace Util
{
	CWorker::CWorker() : m_pGraphicsWorker(nullptr)
	{
	}

	CWorker::~CWorker()
	{
	}
	
	void CWorker::Initialize(bool bRecord)
	{
		m_pGraphicsWorker = CFactory::Instance().CreateGraphicsWorker();
		m_pGraphicsWorker->Initialize(bRecord);
	}
	
	void CWorker::ExecuteCPU(std::function<void()> func)
	{
		func();
	}
	
	void CWorker::ExecuteGraphics(std::function<void()> func)
	{
		m_pGraphicsWorker->LoadAssets(func);
	}
	
	void CWorker::ExecuteCompute(std::function<void()> func)
	{
		func();
	}
	
	void CWorker::Release()
	{
		SAFE_RELEASE_DELETE(m_pGraphicsWorker);
	}
};
