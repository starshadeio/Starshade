//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12Shader.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef GRAPHICS_DX12

#include "CDX12Shader.h"
#include "CDX12Graphics.h"
#include "CDX12Data.h"
#include "CDX12RootSignature.h"
#include "d3dx12.h"
#include "../Factory/CFactory.h"
#include <Utilities/CDebugError.h>
#include <Utilities/CMemoryFree.h>

namespace Graphics
{
	CDX12Shader::CDX12Shader() :
		m_pInputLayout(nullptr),
		m_pPipelineState(nullptr),
		m_pCommandList(nullptr),
		m_pDX12Graphics(nullptr) {
	}

	CDX12Shader::~CDX12Shader() { }

	void CDX12Shader::Initialize(class CShaderFile* pShaderFile)
	{
		CShader::Initialize(pShaderFile);

		m_pDX12Graphics = reinterpret_cast<CDX12Graphics*>(CFactory::Instance().GetGraphicsAPI());

		auto FillByteCodeField = [this](SHADER_TYPE type, D3D12_SHADER_BYTECODE& byteCode) {
			if(m_byteCode[type].pData == nullptr) { return; }
			byteCode = CD3DX12_SHADER_BYTECODE(m_byteCode[type].pData, m_byteCode[type].tSize);
		};

		if(m_shaderInfo[SHADER_TYPE_CS].entryPoint.size() > 0)
		{
			// Describe and create the compute pipeline state object (PSO).
			D3D12_COMPUTE_PIPELINE_STATE_DESC computePsoDesc { };
			computePsoDesc.pRootSignature = dynamic_cast<CDX12RootSignature*>(m_pRootSignature)->GetRootSignature();
			FillByteCodeField(SHADER_TYPE_CS, computePsoDesc.CS);

			ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateComputePipelineState(&computePsoDesc, IID_PPV_ARGS(&m_pPipelineState)));
			NAME_D3D12_OBJECT(m_pPipelineState);

			m_pCommandList = m_pDX12Graphics->GetComputeCommandList();
		}
		else
		{
			// Define the vertex input layout.
			m_pInputLayout = new D3D12_INPUT_ELEMENT_DESC[m_inputData.size()];
			for(size_t i = 0; i < m_inputData.size(); ++i)
			{
				m_pInputLayout[i].SemanticName = m_inputData[i].semanticName.c_str();
				m_pInputLayout[i].SemanticIndex = m_inputData[i].semanticIndex;
				m_pInputLayout[i].Format = ConvertGFXFormatToDXGI(m_inputData[i].format);
				m_pInputLayout[i].InputSlot = m_inputData[i].inputSlot;
				m_pInputLayout[i].AlignedByteOffset = m_inputData[i].alignedByteOffset;
				m_pInputLayout[i].InputSlotClass = ConvertInputClassToD3D12(m_inputData[i].inputSlotClass);
				m_pInputLayout[i].InstanceDataStepRate = m_inputData[i].instanceDataStepRate;
			}

			// Describe and create the graphics pipeline state object (PSO).
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc { };
			psoDesc.InputLayout = { m_pInputLayout, static_cast<u32>(m_inputData.size()) };
			psoDesc.pRootSignature = dynamic_cast<CDX12RootSignature*>(m_pRootSignature)->GetRootSignature();
			FillByteCodeField(SHADER_TYPE_VS, psoDesc.VS);
			FillByteCodeField(SHADER_TYPE_HS, psoDesc.HS);
			FillByteCodeField(SHADER_TYPE_DS, psoDesc.DS);
			FillByteCodeField(SHADER_TYPE_GS, psoDesc.GS);
			FillByteCodeField(SHADER_TYPE_PS, psoDesc.PS);
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
			psoDesc.RasterizerState.FillMode = ConvertFillModeToD3D12(m_pipelineInfo.fillMode);
			psoDesc.RasterizerState.AntialiasedLineEnable = m_pipelineInfo.bAntialiasedLines;
			psoDesc.RasterizerState.DepthBias = m_pipelineInfo.fillMode == FILL_MODE_WIREFRAME ? -500 : 0;
			psoDesc.RasterizerState.CullMode = ConvertCullModeToD3D12(m_pipelineInfo.cullMode);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = ConvertPrimitiveTopologyToD3D12Type(m_pipelineInfo.topologyType);
			psoDesc.NumRenderTargets = m_pipelineInfo.renderTargetCount;
			for(u32 i = 0; i < psoDesc.NumRenderTargets; ++i)
			{
				psoDesc.RTVFormats[i] = ConvertGFXFormatToDXGI(m_pipelineInfo.rtvFormats[i]);
			}
			psoDesc.DSVFormat = ConvertGFXFormatToDXGI(m_pipelineInfo.dsvFormat);
			psoDesc.SampleDesc.Count = 1;

			psoDesc.DepthStencilState.DepthEnable = psoDesc.DSVFormat != DXGI_FORMAT_UNKNOWN;
			psoDesc.DepthStencilState.DepthFunc = m_pipelineInfo.bDepthRead ? D3D12_COMPARISON_FUNC_LESS : D3D12_COMPARISON_FUNC_NEVER;
			psoDesc.DepthStencilState.DepthWriteMask = m_pipelineInfo.bDepthWrite ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
			psoDesc.DepthStencilState.StencilEnable = FALSE;

			// ( TRUE:FALSE:SRC_COLOR:INV_SRC_COLOR:ADD:SRC_ALPHA:INV_SRC_ALPHA:ADD:NOOP:R|G|B|A )
			psoDesc.BlendState.IndependentBlendEnable = m_blendData.size() > 1;
			for(size_t i = 0; i < m_blendData.size(); ++i)
			{
				psoDesc.BlendState.RenderTarget[i] = {
					m_blendData[i].bBlendEnable, m_blendData[i].bLogicOpEnable,
					ConvertGFXBlendToD3D12(m_blendData[i].srcBlend), ConvertGFXBlendToD3D12(m_blendData[i].destBlend), ConvertGFXBlendOpToD3D12(m_blendData[i].blendOp),
					ConvertGFXBlendToD3D12(m_blendData[i].srcBlendAlpha), ConvertGFXBlendToD3D12(m_blendData[i].destBlendAlpha), ConvertGFXBlendOpToD3D12(m_blendData[i].blendOpAlpha),
					ConvertGFXLogicOpToD3D12(m_blendData[i].logicOp),
					ConvertGFXColorWriteEnableToD3D12(m_blendData[i].renderTargetWriteMask)
				};
			}

			ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pPipelineState)));
			NAME_D3D12_OBJECT(m_pPipelineState);

			m_pCommandList = m_pDX12Graphics->GetRealtimeCommandList();
		}
	}

	void CDX12Shader::Bind()
	{
		m_pCommandList->SetPipelineState(m_pPipelineState);
	}

	void CDX12Shader::Release()
	{
		SAFE_RELEASE(m_pPipelineState);
		SAFE_DELETE_ARRAY(m_pInputLayout);
	}
};

#endif
