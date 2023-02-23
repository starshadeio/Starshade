//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12HeapManager.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef GRAPHICS_DX12

#include "CDX12HeapManager.h"
#include "CDX12Data.h"
#include "CDX12Graphics.h"
#include "d3dx12.h"
#include "../Factory/CFactory.h"
#include <Utilities/CMemoryFree.h>
#include <Utilities/CDebugError.h>

namespace Graphics
{
	CDX12HeapManager::CDX12HeapManager() :
		m_rtvCounter(0),
		m_dsvCounter(0),
		m_csuCounter(0),
		m_rtvCount(UINT_MAX),
		m_dsvCount(UINT_MAX),
		m_csuCount(UINT_MAX),
		m_rtvDescriptorSize(0),
		m_dsvDescriptorSize(0),
		m_csuDescriptorSize(0),
		m_pRTVHeap(nullptr),
		m_pDSVHeap(nullptr),
		m_pCSUHeap(nullptr),
		m_pDX12Graphics(nullptr) {
	}

	CDX12HeapManager::~CDX12HeapManager() { }

	void CDX12HeapManager::Initialize()
	{
		m_pDX12Graphics = reinterpret_cast<CDX12Graphics*>(CFactory::Instance().GetGraphicsAPI());

		{ // Create descriptor heaps.

			// Describe and create a RTV descriptor heap.
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc { };
			rtvHeapDesc.NumDescriptors = m_rtvCount = m_rtvCounter;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pRTVHeap)));
			NAME_D3D12_OBJECT(m_pRTVHeap);

			// Describe and create a DSV descriptor heap.
			D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc { };
			dsvHeapDesc.NumDescriptors = m_dsvCount = m_dsvCounter;
			dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_pDSVHeap)));
			NAME_D3D12_OBJECT(m_pDSVHeap);

			// Descrive and create a CVB/SRV/UAV descriptor heap.
			D3D12_DESCRIPTOR_HEAP_DESC csuHeapDesc { };
			csuHeapDesc.NumDescriptors = m_csuCount = m_csuCounter;
			csuHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			csuHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateDescriptorHeap(&csuHeapDesc, IID_PPV_ARGS(&m_pCSUHeap)));
			NAME_D3D12_OBJECT(m_pCSUHeap);

			m_rtvDescriptorSize = m_pDX12Graphics->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			m_dsvDescriptorSize = m_pDX12Graphics->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			m_csuDescriptorSize = m_pDX12Graphics->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		// Reset counters.
		m_rtvCounter = 0;
		m_dsvCounter = 0;
		m_csuCounter = 0;
	}

	void CDX12HeapManager::BindCompute()
	{
		ID3D12DescriptorHeap* ppDescriptorHeaps[] = { m_pCSUHeap };
		m_pDX12Graphics->GetComputeCommandList()->SetDescriptorHeaps(1, ppDescriptorHeaps);
	}

	void CDX12HeapManager::BindGraphics()
	{
		ID3D12DescriptorHeap* ppDescriptorHeaps[] = { m_pCSUHeap };
		m_pDX12Graphics->GetRealtimeCommandList()->SetDescriptorHeaps(1, ppDescriptorHeaps);
	}

	void CDX12HeapManager::Release()
	{
		SAFE_RELEASE(m_pCSUHeap);
		SAFE_RELEASE(m_pDSVHeap);
		SAFE_RELEASE(m_pRTVHeap);
	}

	//-----------------------------------------------------------------------------------------------
	// CPU incrementation methods.
	//-----------------------------------------------------------------------------------------------

	D3D12_CPU_DESCRIPTOR_HANDLE CDX12HeapManager::GetRTVHandleCPU(u32 offset)
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pRTVHeap->GetCPUDescriptorHandleForHeapStart(), offset, m_rtvDescriptorSize);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE CDX12HeapManager::GetDSVHandleCPU(u32 offset)
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pDSVHeap->GetCPUDescriptorHandleForHeapStart(), offset, m_dsvDescriptorSize);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE CDX12HeapManager::GetCSUHandleCPU(u32 offset)
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_pCSUHeap->GetCPUDescriptorHandleForHeapStart(), offset, m_csuDescriptorSize);
	}

	//-----------------------------------------------------------------------------------------------
	// GPU incrementation methods.
	//-----------------------------------------------------------------------------------------------

	D3D12_GPU_DESCRIPTOR_HANDLE CDX12HeapManager::GetRTVHandleGPU(u32 offset)
	{
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pRTVHeap->GetGPUDescriptorHandleForHeapStart(), offset, m_rtvDescriptorSize);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE CDX12HeapManager::GetDSVHandleGPU(u32 offset)
	{
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pDSVHeap->GetGPUDescriptorHandleForHeapStart(), offset, m_dsvDescriptorSize);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE CDX12HeapManager::GetCSUHandleGPU(u32 offset)
	{
		return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_pCSUHeap->GetGPUDescriptorHandleForHeapStart(), offset, m_csuDescriptorSize);
	}
};

#endif
