//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12Compute.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDX12COMPUTE_H
#define CDX12COMPUTE_H

#include "CCompute.h"

namespace Graphics
{
	class CDX12Compute : public CCompute
	{
	public:
		CDX12Compute(const CVObject* pObject);
		~CDX12Compute();
		CDX12Compute(const CDX12Compute&) = delete;
		CDX12Compute(CDX12Compute&&) = delete;
		CDX12Compute& operator = (const CDX12Compute&) = delete;
		CDX12Compute& operator = (CDX12Compute&&) = delete;

		void Initialize() final;
		void PostInitialize() final;
		void Dispatch() final;

	private:
		class CDX12Graphics* m_pDX12Graphics;
	};
};

#endif
