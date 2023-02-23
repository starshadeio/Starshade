//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12UAVBuffer.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef GRAPHICS_DX12

#include "CDX12UAVBuffer.h"
#include "d3dx12.h"
#include "../Factory/CFactory.h"
#include <Utilities/CMemoryFree.h>
#include <Utilities/CDebugError.h>
#include "CDX12Graphics.h"

namespace Graphics
{
	CDX12UAVBuffer::CDX12UAVBuffer() :
		m_heapIndex(0),
		m_pUAVBuffer(nullptr),
		m_pUAVBufferUpload(nullptr),
		m_pDX12Graphics(nullptr) {
	}

	CDX12UAVBuffer::~CDX12UAVBuffer() {
	}

	void CDX12UAVBuffer::Initialize()
	{
		m_pDX12Graphics = dynamic_cast<CDX12Graphics*>(CFactory::Instance().GetGraphicsAPI());

		{ // Create the unordered access buffer.
			CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
			CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(m_data.bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
			ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&m_pUAVBuffer)
			));
		}

		{ // Create the unordered access upload buffer.
			CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
			CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(m_pUAVBuffer, 0, 1));
			ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_pUAVBufferUpload)
			));
		}

		// Prepare buffer for upload view the command list.
		D3D12_SUBRESOURCE_DATA uavData { };
		uavData.pData = m_data.pBufferData;
		uavData.RowPitch = m_data.bufferSize;
		uavData.SlicePitch = uavData.RowPitch;

		UpdateSubresources<1>(m_pDX12Graphics->GetAssetCommandList(), m_pUAVBuffer, m_pUAVBufferUpload, 0, 0, 1, &uavData);

		CD3DX12_RESOURCE_BARRIER resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pUAVBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		m_pDX12Graphics->GetAssetCommandList()->ResourceBarrier(1, &resBarrier);

		m_pDX12Graphics->GetHeapManager().IncCSUCounter();
	}

	void CDX12UAVBuffer::PostInitialize()
	{
		SAFE_RELEASE(m_pUAVBufferUpload);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc { };
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = m_data.bufferSize / m_data.bufferStride;
		uavDesc.Buffer.StructureByteStride = m_data.bufferStride;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		m_heapIndex = m_pDX12Graphics->GetHeapManager().GetNextCSUIndex();
		m_pDX12Graphics->GetDevice()->CreateUnorderedAccessView(m_pUAVBuffer, m_pUAVBufferUpload, &uavDesc, m_pDX12Graphics->GetHeapManager().GetCSUHandleCPU(m_heapIndex));
	}

	void CDX12UAVBuffer::Bind(u32& index, bool bCompute)
	{
		if(bCompute)
		{
			m_pDX12Graphics->GetComputeCommandList()->SetComputeRootDescriptorTable(index++, m_pDX12Graphics->GetHeapManager().GetCSUHandleGPU(m_heapIndex));
		}
		else
		{
			m_pDX12Graphics->GetRealtimeCommandList()->SetGraphicsRootDescriptorTable(index++, m_pDX12Graphics->GetHeapManager().GetCSUHandleGPU(m_heapIndex));
		}
	}

	void CDX12UAVBuffer::Release()
	{
		SAFE_RELEASE(m_pUAVBufferUpload);
		SAFE_RELEASE(m_pUAVBuffer);
	}
};

#endif
