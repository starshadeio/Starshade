//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12HeapManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDX12HEAPMANAGER_H
#define CDX12HEAPMANAGER_H

#include <Globals/CGlobals.h>
#include <d3d12.h>
#include <cassert>

namespace Graphics
{
	class CDX12HeapManager
	{
	public:
		CDX12HeapManager();
		~CDX12HeapManager();
		CDX12HeapManager(const CDX12HeapManager&) = delete;
		CDX12HeapManager(CDX12HeapManager&&) = delete;
		CDX12HeapManager& operator = (const CDX12HeapManager&) = delete;
		CDX12HeapManager& operator = (CDX12HeapManager&&) = delete;

		void Initialize();
		void BindCompute();
		void BindGraphics();
		void Release();

		D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandleCPU(u32 offset);
		D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandleCPU(u32 offset);
		D3D12_CPU_DESCRIPTOR_HANDLE GetCSUHandleCPU(u32 offset);

		D3D12_GPU_DESCRIPTOR_HANDLE GetRTVHandleGPU(u32 offset);
		D3D12_GPU_DESCRIPTOR_HANDLE GetDSVHandleGPU(u32 offset);
		D3D12_GPU_DESCRIPTOR_HANDLE GetCSUHandleGPU(u32 offset);

		inline void IncRTVCounter() { m_rtvCounter++; }
		inline void IncDSVCounter() { m_dsvCounter++; }
		inline void IncCSUCounter() { m_csuCounter++; }

		inline u32 GetNextRTVIndex() { assert(m_rtvCounter < m_rtvCount); return m_rtvCounter++; }
		inline u32 GetNextDSVIndex() { assert(m_dsvCounter < m_dsvCount); return m_dsvCounter++; }
		inline u32 GetNextCSUIndex() { assert(m_csuCounter < m_csuCount); return m_csuCounter++; }

	private:
		u32 m_rtvCounter;
		u32 m_dsvCounter;
		u32 m_csuCounter;
		u32 m_rtvCount;
		u32 m_dsvCount;
		u32 m_csuCount;

		u32 m_rtvDescriptorSize;
		u32 m_dsvDescriptorSize;
		u32 m_csuDescriptorSize;

		ID3D12DescriptorHeap* m_pRTVHeap;
		ID3D12DescriptorHeap* m_pDSVHeap;
		ID3D12DescriptorHeap* m_pCSUHeap;

		class CDX12Graphics* m_pDX12Graphics;
	};
};

#endif
