//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CComputeSystem.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCOMPUTESYSTEM_H
#define CCOMPUTESYSTEM_H

#include <vector>

namespace Graphics
{
	class CComputeSystem
	{
	public:
		CComputeSystem();
		~CComputeSystem();
		CComputeSystem(const CComputeSystem&) = delete;
		CComputeSystem(CComputeSystem&&) = delete;
		CComputeSystem& operator = (const CComputeSystem&) = delete;
		CComputeSystem& operator = (CComputeSystem&&) = delete;

		void PostInitialize();
		void Dispatch();

		// (De)registration methods.
		void Register(class CCompute* pCompute);
		void Deregister(class CCompute* pCompute);

	private:
		std::vector<class CCompute*> m_computeList;
	};
};

#endif
