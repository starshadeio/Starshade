//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12Graphics.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDX12GRAPHICS_H
#define CDX12GRAPHICS_H

#include "CGraphicsAPI.h"
#include "CDX12HeapManager.h"
#include "CDX12Worker.h"
#include "../Utilities/CJobSystem.h"
#include <Math/CMathVector4.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <stack>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

namespace Graphics
{
	class CDX12Graphics : public CGraphicsAPI
	{
	private:
		struct ConstantBuffer
		{
			Math::Vector4 offset;
			float padding[60];
		};

	public:
		CDX12Graphics();
		~CDX12Graphics();
		CDX12Graphics(const CDX12Graphics&) = delete;
		CDX12Graphics(CDX12Graphics&&) = delete;
		CDX12Graphics& operator = (const CDX12Graphics&) = delete;
		CDX12Graphics& operator = (CDX12Graphics&&) = delete;

	private:
		void Initialize() final;
		void Render() final;
		void Sync() final;
		void Release() final;

		void ForceClear(CLEAR_MODE clearMode, const Math::Color* clearColorList, const DepthStencil& clearDepthStencil, u32 numRects, const Math::RectLTRB* pRects) final;
		void SetDefaultRenderTarget();
		void SetRenderTargetsNoStack(const RenderTargetData& rtData);
		void SetRenderTargets(const RenderTargetData& rtData) final;
		void ResetRenderTargets() final;

		void RenderEmptyBuffer(PRIMITIVE_TOPOLOGY topology, u32 vertexCount, u32 instanceCount) final;

		void AdjustToResize() final;
		void FinalizeResize() final;

	public:
		ID3D12GraphicsCommandList* CreateBundle(const class CMaterial* pMaterial);

		// Accessors.
		inline u32 GetFrameIndex() const final { return m_frameIndex; }
		inline u64 GetFrame() const final { return m_frame; }
		inline float GetAspectRatio() const final { return m_aspectRatio; }
		inline float GetWidth() const { return m_viewport.Width; }
		inline float GetHeight() const { return m_viewport.Height; }

		inline ID3D12Device* GetDevice() const { return m_pDevice; }
		inline ID3D12GraphicsCommandList* GetAssetCommandList() const 
			{ return dynamic_cast<const CDX12Worker*>(Util::CJobSystem::Instance().GetWorker().GetGraphicsWorker())->GetCommandList(); }
		inline ID3D12GraphicsCommandList* GetRealtimeCommandList() const { return m_pCommandRealtimeList; }
		inline ID3D12GraphicsCommandList* GetComputeCommandList() const { return m_pComputeCommandList; }
		inline ID3D12CommandQueue* GetCommandQueue() const { return m_pCommandQueue; }
		inline CDX12HeapManager& GetHeapManager() { return m_heapManager; }

	private:
		ADAPTER_ORDER PopulateAdapterList(IDXGIFactory* pFactory, DXGI_GPU_PREFERENCE preference);

		void WaitForGpu();
		void MoveToNextFrame();

		bool Screenshot(const wchar_t* filename, IMAGE_FILETYPE type) final;

	private:
		D3D_FEATURE_LEVEL m_featureLevel;

		float m_aspectRatio;
		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_scissor;
		DXGI_SWAP_CHAIN_DESC1 m_swapChainDesc;

		u32 m_rtvDescriptorSize;
		u32 m_frameIndex;
		u64 m_frame;

		CDX12HeapManager m_heapManager;

		std::stack<RenderTargetData> m_rtStack;

		IDXGISwapChain3* m_pSwapChain;
		ID3D12Device* m_pDevice;
		ID3D12Resource** m_pRenderTargetList;
		ID3D12CommandAllocator** m_pCommandAllocatorList;
		ID3D12CommandAllocator** m_pComputeCommandAllocatorList;
		//ID3D12CommandAllocator* m_pBundleAllocator;
		ID3D12CommandQueue* m_pCommandQueue;
		ID3D12CommandQueue* m_pComputeCommandQueue;
		ID3D12DescriptorHeap* m_pRTVHeap;

		ID3D12GraphicsCommandList* m_pCommandRealtimeList;
		ID3D12GraphicsCommandList* m_pComputeCommandList;

		ID3D12Fence* m_pFence;
		ID3D12Fence* m_pComputeFence;
		u64* m_pFenceValueList;
		HANDLE m_fenceEvent;
	};
};

#endif
