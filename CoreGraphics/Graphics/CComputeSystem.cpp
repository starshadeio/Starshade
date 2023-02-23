//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CComputeSystem.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CComputeSystem.h"
#include "CCompute.h"
#include "CMaterial.h"
#include "CShader.h"
#include "CRootSignature.h"

namespace Graphics
{
	CComputeSystem::CComputeSystem() { }
	CComputeSystem::~CComputeSystem() { }

	void CComputeSystem::PostInitialize()
	{
		for(CCompute* pCompute : m_computeList)
		{
			pCompute->PostInitialize();
		};
	}

	void CComputeSystem::Dispatch()
	{
		CRootSignature* rootSig = nullptr;

		for(CCompute* pCompute : m_computeList)
		{
			if(rootSig != pCompute->GetData().pMaterial->GetShader()->GetRootSignature())
			{
				rootSig = pCompute->GetData().pMaterial->GetShader()->GetRootSignature();
				rootSig->Bind();
			}

			pCompute->Dispatch();
		};
	}

	// (De)registration methods.
	void CComputeSystem::Register(CCompute* pCompute)
	{
		m_computeList.push_back(pCompute);

		// Bubble the newly pushed mesh render to the proper position.
		for(s64 i = static_cast<s64>(m_computeList.size()) - 1; i > 0; --i)
		{
			if(reinterpret_cast<uintptr_t>(m_computeList[i - 1]->GetData().pMaterial->GetShader()->GetRootSignature()) >
				reinterpret_cast<uintptr_t>(m_computeList[i]->GetData().pMaterial->GetShader()->GetRootSignature()))
			{
				// Swap the two adjacent values.
				CCompute* tmp = m_computeList[i - 1];
				m_computeList[i - 1] = m_computeList[i];
				m_computeList[i] = tmp;
			}
		}
	}

	void CComputeSystem::Deregister(CCompute* pCompute)
	{
		size_t i, j;
		for(i = 0, j = 0; i < m_computeList.size(); ++i)
		{
			if(m_computeList[j] != pCompute)
			{
				m_computeList[j++] = m_computeList[i];
			}
		}

		m_computeList.resize(j);
	}
};
