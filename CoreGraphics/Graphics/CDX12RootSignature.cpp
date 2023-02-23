//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12RootSignature.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef GRAPHICS_DX12

#include "CDX12RootSignature.h"
#include "CDX12Graphics.h"
#include "CDX12Data.h"
#include "d3dx12.h"
#include "../Factory/CFactory.h"
#include <Utilities/CDebugError.h>
#include <Utilities/CMemoryFree.h>
#include <wrl.h>
#include <vector>

namespace Graphics
{
	using Microsoft::WRL::ComPtr;

	CDX12RootSignature::CDX12RootSignature() :
		m_pRootSignature(nullptr),
		m_pDX12Graphics(nullptr) {
	}

	CDX12RootSignature::~CDX12RootSignature() { }

	void CDX12RootSignature::Initialize()
	{
		m_pDX12Graphics = reinterpret_cast<CDX12Graphics*>(CFactory::Instance().GetGraphicsAPI());

		// Get the highest supported root signature version.
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData { };
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if(FAILED(m_pDX12Graphics->GetDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		// Construct dx12 root signature description data from m_data.
		std::vector<CD3DX12_DESCRIPTOR_RANGE1> rangeList(m_data.rangeList.size());
		std::vector<CD3DX12_ROOT_PARAMETER1> paramList(m_data.paramList.size());
		std::vector<D3D12_STATIC_SAMPLER_DESC> samplerList(m_data.samplerList.size());

		// Descriptor range list.
		for(size_t i = 0; i < m_data.rangeList.size(); ++i)
		{
			rangeList[i].RangeType = ConvertGFXDescriptorRangeType(m_data.rangeList[i].rangeType);
			rangeList[i].NumDescriptors = m_data.rangeList[i].numDescriptors;
			rangeList[i].BaseShaderRegister = m_data.rangeList[i].baseShaderRegister;
			rangeList[i].RegisterSpace = m_data.rangeList[i].registerSpace;
			rangeList[i].Flags = ConvertGFXDescriptorRangeFlagsToD3D12(m_data.rangeList[i].flags);
			rangeList[i].OffsetInDescriptorsFromTableStart = m_data.rangeList[i].offsetInDescriptorsFromTableStart;
		}

		// Root parameter list.
		for(size_t i = 0; i < m_data.paramList.size(); ++i)
		{
			paramList[i].ParameterType = ConvertGFXRootParameterTypeToD3D12(m_data.paramList[i].parameterType);
			paramList[i].ShaderVisibility = ConvertGFXShaderVisibilityToD3D12(m_data.paramList[i].shaderVisibility);

			switch(m_data.paramList[i].parameterType)
			{
				case ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
					paramList[i].DescriptorTable.NumDescriptorRanges = m_data.paramList[i].descriptorTable.numDescriptorRanges;
					paramList[i].DescriptorTable.pDescriptorRanges = &rangeList[m_data.paramList[i].descriptorTable.rangeOffset];
					break;
				case ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
					paramList[i].Constants.ShaderRegister = m_data.paramList[i].constants.shaderRegister;
					paramList[i].Constants.RegisterSpace = m_data.paramList[i].constants.registerSpace;
					paramList[i].Constants.Num32BitValues = m_data.paramList[i].constants.num32BitValues;
					break;
				case ROOT_PARAMETER_TYPE_CBV:
				case ROOT_PARAMETER_TYPE_SRV:
				case ROOT_PARAMETER_TYPE_UAV:
				default:
					paramList[i].Descriptor.ShaderRegister = m_data.paramList[i].descriptor.shaderRegister;
					paramList[i].Descriptor.RegisterSpace = m_data.paramList[i].descriptor.registerSpace;
					paramList[i].Descriptor.Flags = ConvertGFXRootDescriptorFlagsToD3D12(m_data.paramList[i].descriptor.flags);
					break;
			}
		}

		// Static sampler list.
		for(size_t i = 0; i < m_data.samplerList.size(); ++i)
		{
			samplerList[i].Filter = ConvertGFXFilterToD3D12(m_data.samplerList[i].filter);
			samplerList[i].AddressU = ConvertGFXTextureAddessModeToD3D12(m_data.samplerList[i].addressU);
			samplerList[i].AddressV = ConvertGFXTextureAddessModeToD3D12(m_data.samplerList[i].addressV);
			samplerList[i].AddressW = ConvertGFXTextureAddessModeToD3D12(m_data.samplerList[i].addressW);
			samplerList[i].MipLODBias = m_data.samplerList[i].mipLODBias;
			samplerList[i].MaxAnisotropy = m_data.samplerList[i].maxAnisotropy;
			samplerList[i].ComparisonFunc = ConvertGFXComparisionFuncToD3D12(m_data.samplerList[i].comparisonFunc);
			samplerList[i].BorderColor = ConvertGFXBorderColorToD3D12(m_data.samplerList[i].borderColor);
			samplerList[i].MinLOD = m_data.samplerList[i].minLOD;
			samplerList[i].MaxLOD = m_data.samplerList[i].maxLOD;
			samplerList[i].ShaderRegister = m_data.samplerList[i].shaderRegister;
			samplerList[i].RegisterSpace = m_data.samplerList[i].registerSpace;
			samplerList[i].ShaderVisibility = ConvertGFXShaderVisibilityToD3D12(m_data.samplerList[i].shaderVisibility);
		}

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(static_cast<u32>(paramList.size()), paramList.data(), static_cast<u32>(samplerList.size()), samplerList.data(), ConvertGFXRootSignatureFlagsToD3D12(m_data.flags));

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ASSERT_HR_R(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
		ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)));
		NAME_D3D12_OBJECT(m_pRootSignature);
	}

	void CDX12RootSignature::Bind()
	{
		if(m_data.bCompute)
		{
			m_pDX12Graphics->GetComputeCommandList()->SetComputeRootSignature(m_pRootSignature);
		}
		else
		{
			m_pDX12Graphics->GetRealtimeCommandList()->SetGraphicsRootSignature(m_pRootSignature);
		}
	}

	void CDX12RootSignature::Release()
	{
		SAFE_RELEASE(m_pRootSignature);
	}
};

#endif
