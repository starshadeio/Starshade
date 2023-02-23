//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRootSigRegistrar.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CRootSigRegistrar.h"

#ifdef GRAPHICS_DX12
#include "../Graphics/CDX12RootSignature.h"
#endif
#ifdef GRAPHICS_VULKAN
#endif

#include <Utilities/CMemoryFree.h>

namespace Graphics
{
	CRootSigRegistrar::CRootSigRegistrar()
	{
		m_lookUpTree.SetOnDelete(std::bind(&CRootSigRegistrar::OnDeleteElement, this, std::placeholders::_1));
	}

	CRootSigRegistrar::~CRootSigRegistrar() { }

	void CRootSigRegistrar::Release()
	{
		m_lookUpTree.Clear();
	}

	CRootSignature* CRootSigRegistrar::Get(const CRootSignature::Data& data)
	{
		union FloatToUInt
		{
			float f;
			u32 i;
		};

		auto ToUInt = [](float f) {
			FloatToUInt cvt;
			cvt.f = f;
			return cvt.i;
		};

		auto pNode = m_lookUpTree.GetOrCreateHead();
		auto PushElem = [&](u32 i) {
			auto pNext = m_lookUpTree.Step(i, pNode);
			if(pNext == nullptr)
			{
				pNext = m_lookUpTree.Insert(i, pNode);
			}

			pNode = pNext;
		};

		// Build the sequence to be used to attempt to look up a root signature with.
		PushElem(static_cast<u32>(data.bCompute));
		PushElem(static_cast<u32>(data.flags));
		for(const DescriptorRange& range : data.rangeList)
		{
			PushElem(static_cast<u32>(range.rangeType));
			PushElem(range.numDescriptors);
			PushElem(range.baseShaderRegister);
			PushElem(range.registerSpace);
			PushElem(static_cast<u32>(range.flags));
			PushElem(range.offsetInDescriptorsFromTableStart);
		}

		for(const RootParameter param : data.paramList)
		{
			PushElem(static_cast<u32>(param.parameterType));
			PushElem(static_cast<u32>(param.shaderVisibility));

			switch(param.parameterType)
			{
				case ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
					PushElem(param.descriptorTable.numDescriptorRanges);
					PushElem(param.descriptorTable.rangeOffset);
					break;
				case ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
					PushElem(param.constants.shaderRegister);
					PushElem(param.constants.registerSpace);
					PushElem(param.constants.num32BitValues);
					break;
				case ROOT_PARAMETER_TYPE_CBV:
				case ROOT_PARAMETER_TYPE_SRV:
				case ROOT_PARAMETER_TYPE_UAV:
				default:
					PushElem(param.descriptor.shaderRegister);
					PushElem(param.descriptor.registerSpace);
					PushElem(static_cast<u32>(param.descriptor.flags));
					break;
			}
		}

		for(const StaticSamplerDesc sampler : data.samplerList)
		{
			PushElem(static_cast<u32>(sampler.filter));
			PushElem(static_cast<u32>(sampler.addressU));
			PushElem(static_cast<u32>(sampler.addressV));
			PushElem(static_cast<u32>(sampler.addressW));
			PushElem(ToUInt(sampler.mipLODBias));
			PushElem(sampler.maxAnisotropy);
			PushElem(static_cast<u32>(sampler.comparisonFunc));
			PushElem(static_cast<u32>(sampler.borderColor));
			PushElem(ToUInt(sampler.minLOD));
			PushElem(ToUInt(sampler.maxLOD));
			PushElem(sampler.shaderRegister);
			PushElem(sampler.registerSpace);
			PushElem(static_cast<u32>(sampler.shaderVisibility));
		}

		assert(pNode);

		// Check if lookup was successful.
		if(pNode->bTerminal) { }
		else
		{ // If no match was found, create a new root signature and insert it into the tree.
			CRootSignature* rootSig = CreateRootSignature();
			rootSig->SetData(data);
			rootSig->Initialize();

			pNode->bTerminal = true;
			pNode->value = rootSig;
		}

		return pNode->value;
	}

	CRootSignature* CRootSigRegistrar::CreateRootSignature()
	{
#ifdef GRAPHICS_DX12
		return new CDX12RootSignature();
#endif
#ifdef GRAPHICS_VULKAN
		return new CVKRootSignature();
#endif

		return nullptr;
	}

	void CRootSigRegistrar::OnDeleteElement(CRootSignature** ppRootSig)
	{
		SAFE_RELEASE_DELETE(*ppRootSig);
	}
};
