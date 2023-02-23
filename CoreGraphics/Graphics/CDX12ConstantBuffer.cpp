//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12ConstantBuffer.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef GRAPHICS_DX12

#include "CDX12ConstantBuffer.h"
#include "CDX12Graphics.h"
#include "CDX12Data.h"
#include "d3dx12.h"
#include "../Factory/CFactory.h"
#include <Utilities/CDebugError.h>
#include <Utilities/CMemoryFree.h>

namespace Graphics
{
	CDX12ConstantBuffer::CDX12ConstantBuffer() :
		m_32BitValueCount(0),
		m_heapIndex(0),
		m_pConstantBuffer(nullptr),
		m_pConstantBufferUpload(nullptr),
		m_pDX12Graphics(nullptr) {
	}

	CDX12ConstantBuffer::~CDX12ConstantBuffer() {
	}

	void CDX12ConstantBuffer::Initialize()
	{
		m_pDX12Graphics = dynamic_cast<CDX12Graphics*>(CFactory::Instance().GetGraphicsAPI());

		if(m_data.bRootConstant)
		{ // Calculate 32-bit value count for root constants.
			m_32BitValueCount = m_data.bufferSize >> 2;

		}
		else
		{ // Create constant buffer to be linked with the heap.

			{ // Create the constant buffer.
				CD3DX12_HEAP_PROPERTIES heapProps(m_data.bFixed ? D3D12_HEAP_TYPE_DEFAULT : D3D12_HEAP_TYPE_UPLOAD);
				CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(m_data.bufferSize);

				ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					m_data.bFixed ? D3D12_RESOURCE_STATE_COPY_DEST : D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_pConstantBuffer)
				));

				NAME_D3D12_OBJECT(m_pConstantBuffer);
			}

			if(m_data.bFixed)
			{ // Fixed constant buffer data.
				{ // Create the constant buffer data uploader.
					const u64 uploadBufferSize = GetRequiredIntermediateSize(m_pConstantBuffer, 0, 1);
					CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
					CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

					ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
						&heapProps,
						D3D12_HEAP_FLAG_NONE,
						&resDesc,
						D3D12_RESOURCE_STATE_GENERIC_READ,
						nullptr,
						IID_PPV_ARGS(&m_pConstantBufferUpload)
					));
				}

				// Prepare the buffer for upload via the command list.
				D3D12_SUBRESOURCE_DATA cbvData { };
				cbvData.pData = m_data.pBufferData;
				cbvData.RowPitch = m_data.bufferSize;
				cbvData.SlicePitch = cbvData.RowPitch;

				UpdateSubresources<1>(m_pDX12Graphics->GetAssetCommandList(), m_pConstantBuffer, m_pConstantBufferUpload, 0, 0, 1, &cbvData);
			}
			else
			{ // Dynamic constant buffer data.
			 // Map the constant buffer, and keep it mapped until destruction.
				CD3DX12_RANGE readRange(0, 0);
				ASSERT_HR_R(m_pConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegin)));
				UpdateData();//memcpy(m_pCbvDataBegin, m_data.pBufferData, m_data.bufferSize);
			}

			m_pDX12Graphics->GetHeapManager().IncCSUCounter();
		}
	}

	void CDX12ConstantBuffer::PostInitialize()
	{
		if(!m_data.bRootConstant)
		{
			SAFE_RELEASE(m_pConstantBufferUpload);

			// Describe and create a constant buffer view.
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc { };
			cbvDesc.BufferLocation = m_pConstantBuffer->GetGPUVirtualAddress();
			cbvDesc.SizeInBytes = m_data.bufferSize;

			m_heapIndex = m_pDX12Graphics->GetHeapManager().GetNextCSUIndex();
			m_pDX12Graphics->GetDevice()->CreateConstantBufferView(&cbvDesc, m_pDX12Graphics->GetHeapManager().GetCSUHandleCPU(m_heapIndex));
		}
	}

	void CDX12ConstantBuffer::Bind(u32& index, bool bCompute)
	{
		CConstantBuffer::Bind(index, bCompute);

		if(m_data.bRootConstant)
		{
			if(bCompute)
			{
				m_pDX12Graphics->GetComputeCommandList()->SetComputeRoot32BitConstants(index++, m_32BitValueCount, m_data.pBufferData, 0);
			}
			else
			{
				m_pDX12Graphics->GetRealtimeCommandList()->SetGraphicsRoot32BitConstants(index++, m_32BitValueCount, m_data.pBufferData, 0);
			}
		}
		else
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
	}

	void CDX12ConstantBuffer::Release()
	{
		SAFE_RELEASE(m_pConstantBufferUpload);
		SAFE_RELEASE(m_pConstantBuffer);
	}
};

#endif
