//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12Compute.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef GRAPHICS_DX12

#include "CDX12Compute.h"
#include "../Factory/CFactory.h"
#include "CDX12Graphics.h"

namespace Graphics
{
	CDX12Compute::CDX12Compute(const CVObject* pObject) :
		CCompute(pObject),
		m_pDX12Graphics(nullptr) {
	}

	CDX12Compute::~CDX12Compute() { }

	void CDX12Compute::Initialize()
	{
		CCompute::Initialize();
		m_pDX12Graphics = dynamic_cast<CDX12Graphics*>(CFactory::Instance().GetGraphicsAPI());
	}

	void CDX12Compute::PostInitialize() { }

	void CDX12Compute::Dispatch()
	{
		CCompute::Dispatch();
		m_pDX12Graphics->GetComputeCommandList()->Dispatch(m_data.threadX, m_data.threadY, m_data.threadZ);
	}
};

#endif
