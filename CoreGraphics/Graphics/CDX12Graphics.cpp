//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12Graphics.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CDX12Graphics.h"
#include "CDX12Data.h"
#include "CShaderCompiler.h"
#include "CMaterial.h"
#include "CDX12Shader.h"
#include "CDX12RootSignature.h"
#include "d3dx12.h"
#include "../Application/CWinPanel.h"
#include "../Factory/CFactory.h"
#include <Utilities/CDebugError.h>
#include <Utilities/CMemoryFree.h>
#include <Math/CMathVector2.h>
#include <Math/CMathColor.h>
#include <wrl.h>

#include <ScreenGrab.h>
#include <wincodec.h>

#pragma comment(lib, "Windowscodecs.lib")

namespace Graphics
{
	using Microsoft::WRL::ComPtr;

	CDX12Graphics::CDX12Graphics() :
		m_featureLevel(D3D_FEATURE_LEVEL_12_0),
		m_rtvDescriptorSize(0),
		m_frameIndex(0),
		m_frame(0),
		m_pSwapChain(nullptr),
		m_pDevice(nullptr),
		m_pRenderTargetList(nullptr),
		m_pCommandAllocatorList(nullptr),
		m_pComputeCommandAllocatorList(nullptr),
		m_pCommandQueue(nullptr),
		m_pComputeCommandQueue(nullptr),
		m_pRTVHeap(nullptr),
		m_pCommandRealtimeList(nullptr),
		m_pComputeCommandList(nullptr),
		m_pFence(nullptr),
		m_pComputeFence(nullptr),
		m_pFenceValueList(nullptr),
		m_fenceEvent(nullptr) {
	}

	CDX12Graphics::~CDX12Graphics() { }

	void CDX12Graphics::Initialize()
	{
		u32 factoryFlags = 0;

#ifdef _DEBUG
		{
			ComPtr<ID3D12Debug> debugController;
			if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
				factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			}
		}
#endif

		ComPtr<IDXGIFactory4> pFactory;
		ASSERT_HR_R(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&pFactory)));

		{ // Try to discover best hardware adapter, and create a device out of it.
			const DXGI_GPU_PREFERENCE preference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
			ADAPTER_ORDER order = PopulateAdapterList(pFactory.Get(), preference);
			ASSERT_R(order != ADAPTER_ORDER_EMPTY);

			ComPtr<IDXGIAdapter1> pAdapter;
			if(order == ADAPTER_ORDER_SORTED)
			{
				ComPtr<IDXGIFactory6> pFactory6;
				ASSERT_HR_R(pFactory->QueryInterface(IID_PPV_ARGS(&pFactory6)));
				ASSERT_HR_R(pFactory6->EnumAdapterByGpuPreference(m_adapterDataList[0].index, preference, IID_PPV_ARGS(&pAdapter)));
			}
			else /* order == ADAPTER_ORDER_UNSORTED */
			{
				ASSERT_R(pFactory->EnumAdapters1(m_adapterDataList[0].index, &pAdapter) != DXGI_ERROR_NOT_FOUND);
			}

			ASSERT_HR_R(D3D12CreateDevice(pAdapter.Get(), m_featureLevel, IID_PPV_ARGS(&m_pDevice)));
		}

		{ // Describe and create the command queues.
			D3D12_COMMAND_QUEUE_DESC queueDesc { };
			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

			ASSERT_HR_R(m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue)));
			NAME_D3D12_OBJECT(m_pCommandQueue);

			D3D12_COMMAND_QUEUE_DESC computeQueueDesc { };
			computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

			ASSERT_HR_R(m_pDevice->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&m_pComputeCommandQueue)));
			NAME_D3D12_OBJECT(m_pComputeCommandQueue);
		}

		{ // Describe and create the swap chain.
			memset(&m_swapChainDesc, 0, sizeof(m_swapChainDesc));
			m_swapChainDesc.BufferCount = m_data.bufferCount;
			m_swapChainDesc.Width = static_cast<UINT>(m_data.pPanel->GetRect().w);
			m_swapChainDesc.Height = static_cast<UINT>(m_data.pPanel->GetRect().h);
			m_swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			m_swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			m_swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			m_swapChainDesc.Scaling = DXGI_SCALING_NONE;
			m_swapChainDesc.SampleDesc.Count = 1;

			HWND hWnd = dynamic_cast<App::CWinPanel*>(m_data.pPanel)->GetWindowHandle();

			ComPtr<IDXGISwapChain1> pSwapChain;
			ASSERT_HR_R(pFactory->CreateSwapChainForHwnd(
				m_pCommandQueue,
				hWnd,
				&m_swapChainDesc,
				nullptr,
				nullptr,
				&pSwapChain
			));

			ASSERT_HR_R(pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

			ASSERT_HR_R(pSwapChain->QueryInterface(IID_PPV_ARGS(&m_pSwapChain)));
			m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

			{ // Create descriptor heaps.

				// Describe and create a RTV descriptor heap.
				D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc { };
				rtvHeapDesc.NumDescriptors = m_data.bufferCount;
				rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
				rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				ASSERT_HR_R(m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pRTVHeap)));
				NAME_D3D12_OBJECT(m_pRTVHeap);

				m_rtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			}

			{ // Create frame resources.
				CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRTVHeap->GetCPUDescriptorHandleForHeapStart());

				m_pRenderTargetList = new ID3D12Resource * [m_data.bufferCount];
				m_pCommandAllocatorList = new ID3D12CommandAllocator * [m_data.bufferCount];
				m_pComputeCommandAllocatorList = new ID3D12CommandAllocator * [m_data.bufferCount];

				// Create a RTV and command allocators for each frame.
				for(u32 i = 0; i < m_data.bufferCount; ++i)
				{
					ASSERT_HR_R(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargetList[i])));
					m_pDevice->CreateRenderTargetView(m_pRenderTargetList[i], nullptr, rtvHandle);
					rtvHandle.Offset(1, m_rtvDescriptorSize);

					NAME_D3D12_OBJECT_INDEXED(m_pRenderTargetList, i);

					ASSERT_HR_R(m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocatorList[i])));
					ASSERT_HR_R(m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&m_pComputeCommandAllocatorList[i])));

					NAME_D3D12_OBJECT_INDEXED(m_pCommandAllocatorList, i);
					NAME_D3D12_OBJECT_INDEXED(m_pComputeCommandAllocatorList, i);
				}
			}

			// Create the command lists.
			ASSERT_HR_R(m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocatorList[m_frameIndex], nullptr, IID_PPV_ARGS(&m_pCommandRealtimeList)));
			ASSERT_HR_R(m_pCommandRealtimeList->Close());
			ASSERT_HR_R(m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, m_pComputeCommandAllocatorList[m_frameIndex], nullptr, IID_PPV_ARGS(&m_pComputeCommandList)));
			ASSERT_HR_R(m_pComputeCommandList->Close());

			NAME_D3D12_OBJECT(m_pCommandRealtimeList);
			NAME_D3D12_OBJECT(m_pComputeCommandList);
			
			// Setup the view.
			m_viewport.TopLeftX = 0.0f;
			m_viewport.TopLeftY = 0.0f;
			m_viewport.Width = static_cast<FLOAT>(m_data.pPanel->GetRect().w);
			m_viewport.Height = static_cast<FLOAT>(m_data.pPanel->GetRect().h);
			m_viewport.MinDepth = 0.0f;
			m_viewport.MaxDepth = 1.0f;

			m_aspectRatio = m_viewport.Width / m_viewport.Height;

			m_scissor.left = 0;
			m_scissor.top = 0;
			m_scissor.right = static_cast<LONG>(m_data.pPanel->GetRect().w);
			m_scissor.bottom = static_cast<LONG>(m_data.pPanel->GetRect().h);

			{ // Worker.
				if(m_data.onInit) { m_data.onInit(); }
				m_heapManager.Initialize();
			}

			{ // Create synchronization objects and wait until assets have been uploaded to the GPU.
				m_pFenceValueList = new u64[m_data.bufferCount];
				memset(m_pFenceValueList, 0, sizeof(u64) * m_data.bufferCount);

				ASSERT_HR_R(m_pDevice->CreateFence(m_pFenceValueList[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)));
				ASSERT_HR_R(m_pDevice->CreateFence(m_pFenceValueList[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pComputeFence)));
				++m_pFenceValueList[m_frameIndex];

				// Create an event handle to use for frame synchronization.
				m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
				if(m_fenceEvent == nullptr)
				{
					ASSERT_HR_R(HRESULT_FROM_WIN32(GetLastError()));
				}

				WaitForGpu();
			}
		}
	}
	
	void CDX12Graphics::Render()
	{
		{ // Populate the command lists.

			// Make sure that GPU work is completed before resetting allocators.
			ASSERT_HR_R(m_pComputeCommandAllocatorList[m_frameIndex]->Reset());
			ASSERT_HR_R(m_pCommandAllocatorList[m_frameIndex]->Reset());

			ASSERT_HR_R(m_pComputeCommandList->Reset(m_pComputeCommandAllocatorList[m_frameIndex], nullptr));
			ASSERT_HR_R(m_pCommandRealtimeList->Reset(m_pCommandAllocatorList[m_frameIndex], nullptr));

			{ // Populate compute command list.
				m_heapManager.BindCompute();

				if(m_data.onCompute) { m_data.onCompute(); }

				ASSERT_HR_R(m_pComputeCommandList->Close());
			}

			{ // Populate graphics command list.
				m_heapManager.BindGraphics();

				D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pRenderTargetList[m_frameIndex],
					D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
				m_pCommandRealtimeList->ResourceBarrier(1, &barrier);

				SetDefaultRenderTarget();
				ForceClear(CLEAR_COLOR, &m_data.clearValue.color, m_data.clearValue.depthStencil, 0, nullptr);

				if(m_data.onRender) { m_data.onRender(); }

				// Record rendering commands.
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
				m_pCommandRealtimeList->ResourceBarrier(1, &barrier);

				ASSERT_HR_R(m_pCommandRealtimeList->Close());
			}
		}

		// Execuate the compute work.
		ID3D12CommandList* ppComputeCommandLists[] = { m_pComputeCommandList };
		m_pComputeCommandQueue->ExecuteCommandLists(_countof(ppComputeCommandLists), ppComputeCommandLists);

		m_pComputeCommandQueue->Signal(m_pComputeFence, m_pFenceValueList[m_frameIndex]);

		// Execute the rendering work only when the compute work is complete.
		m_pCommandQueue->Wait(m_pComputeFence, m_pFenceValueList[m_frameIndex]);

		// Execute the rendering work.
		ID3D12CommandList* ppCommandLists[] = { m_pCommandRealtimeList };
		m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// Present the frame.
		ASSERT_HR_R(m_pSwapChain->Present(m_data.vBlanks, 0));

		MoveToNextFrame();
	}

	void CDX12Graphics::Sync()
	{
		WaitForGpu();
	}

	void CDX12Graphics::Release()
	{
		CloseHandle(m_fenceEvent);
		SAFE_RELEASE(m_pComputeFence);
		SAFE_RELEASE(m_pFence);
		SAFE_DELETE_ARRAY(m_pFenceValueList);

		m_heapManager.Release();

		SAFE_RELEASE(m_pComputeCommandList);
		SAFE_RELEASE(m_pCommandRealtimeList);

		//SAFE_RELEASE(m_pBundleAllocator);
		for(u32 i = 0; i < m_data.bufferCount; ++i)
		{
			SAFE_RELEASE(m_pComputeCommandAllocatorList[i]);
			SAFE_RELEASE(m_pCommandAllocatorList[i]);
			SAFE_RELEASE(m_pRenderTargetList[i]);
		}

		SAFE_DELETE_ARRAY(m_pComputeCommandAllocatorList);
		SAFE_DELETE_ARRAY(m_pCommandAllocatorList);
		SAFE_DELETE_ARRAY(m_pRenderTargetList);

		SAFE_RELEASE(m_pRTVHeap);
		SAFE_RELEASE(m_pComputeCommandQueue);
		SAFE_RELEASE(m_pCommandQueue);
		SAFE_RELEASE(m_pSwapChain);
		SAFE_RELEASE(m_pDevice);
	}

	//-----------------------------------------------------------------------------------------------
	// Render target methods.
	//-----------------------------------------------------------------------------------------------

	void CDX12Graphics::ForceClear(CLEAR_MODE clearMode, const Math::Color* clearColorList, const DepthStencil& clearDepthStencil, u32 numRects, const Math::RectLTRB* pRects)
	{
		assert(!m_rtStack.empty());
		const RenderTargetData& rtData = m_rtStack.top();

		if(clearMode & CLEAR_COLOR)
		{
			for(u32 i = 0; i < rtData.targetCount; ++i)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = { reinterpret_cast<SIZE_T>(rtData.pRtvHandle[i]) };

				// Clear render target view.
				m_pCommandRealtimeList->ClearRenderTargetView(rtvHandle, reinterpret_cast<const float*>(&clearColorList[i]),
					numRects, reinterpret_cast<const D3D12_RECT*>(pRects));
			}
		}

		if(rtData.pDsvHandle)
		{
			u32 flag = 0;
			if(clearMode & CLEAR_DEPTH) { flag |= D3D12_CLEAR_FLAG_DEPTH; }
			if(clearMode & CLEAR_STENCIL) { flag |= D3D12_CLEAR_FLAG_STENCIL; }
			if(flag)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = { reinterpret_cast<SIZE_T>(rtData.pDsvHandle) };
				m_pCommandRealtimeList->ClearDepthStencilView(dsvHandle, static_cast<D3D12_CLEAR_FLAGS>(flag), clearDepthStencil.depth, clearDepthStencil.stencil,
					numRects, reinterpret_cast<const D3D12_RECT*>(pRects));
			}
		}
	}

	void CDX12Graphics::SetDefaultRenderTarget()
	{
		while(m_rtStack.size())
		{
			m_rtStack.pop();
		}

		// Set frame render target and depth stencil view.
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRTVHeap->GetCPUDescriptorHandleForHeapStart();

		rtvHandle.ptr += static_cast<size_t>(m_frameIndex * m_rtvDescriptorSize);
		m_pCommandRealtimeList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		// Set viewports and scissor rects.
		m_pCommandRealtimeList->RSSetViewports(1, &m_viewport);
		m_pCommandRealtimeList->RSSetScissorRects(1, &m_scissor);

		RenderTargetData rtData { };
		rtData.targetCount = 1;
		rtData.pRtvHandle[0] = reinterpret_cast<void*>(rtvHandle.ptr);

		rtData.viewport.topLeftX = m_viewport.TopLeftX;
		rtData.viewport.topLeftY = m_viewport.TopLeftY;
		rtData.viewport.width = m_viewport.Width;
		rtData.viewport.height = m_viewport.Height;
		rtData.viewport.minDepth = m_viewport.MinDepth;
		rtData.viewport.maxDepth = m_viewport.MaxDepth;

		rtData.scissorRect.left = m_scissor.left;
		rtData.scissorRect.top = m_scissor.top;
		rtData.scissorRect.right = m_scissor.right;
		rtData.scissorRect.bottom = m_scissor.bottom;

		m_rtStack.push(rtData);
	}

	// Method for setting a new (set of) render target(s) without push data to the start. For internal use only!
	void CDX12Graphics::SetRenderTargetsNoStack(const RenderTargetData& rtData)
	{
		// Set frame render target and depth stencil view.
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[8];
		for(u32 i = 0; i < rtData.targetCount; ++i)
		{
			rtvHandle[i] = { reinterpret_cast<SIZE_T>(rtData.pRtvHandle[i]) };
		}

		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = { reinterpret_cast<SIZE_T>(rtData.pDsvHandle) };

		m_pCommandRealtimeList->OMSetRenderTargets(rtData.targetCount, rtData.targetCount ? rtvHandle : nullptr, FALSE, rtData.pDsvHandle ? &dsvHandle : nullptr);

		// Set viewports and scissor rects.
		D3D12_VIEWPORT viewport;
		viewport.TopLeftX = rtData.viewport.topLeftX;
		viewport.TopLeftY = rtData.viewport.topLeftY;
		viewport.Width = rtData.viewport.width;
		viewport.Height = rtData.viewport.height;
		viewport.MinDepth = rtData.viewport.minDepth;
		viewport.MaxDepth = rtData.viewport.maxDepth;

		D3D12_RECT rect;
		rect.left = rtData.scissorRect.left;
		rect.top = rtData.scissorRect.top;
		rect.right = rtData.scissorRect.right;
		rect.bottom = rtData.scissorRect.bottom;

		m_pCommandRealtimeList->RSSetViewports(1, &viewport);
		m_pCommandRealtimeList->RSSetScissorRects(1, &rect);
	}

	void CDX12Graphics::SetRenderTargets(const RenderTargetData& rtData)
	{
		m_rtStack.push(rtData);
		SetRenderTargetsNoStack(rtData);
	}

	void CDX12Graphics::ResetRenderTargets()
	{
		if(m_rtStack.size() == 1) return;
		m_rtStack.pop();
		SetRenderTargetsNoStack(m_rtStack.top());
	}

	//-----------------------------------------------------------------------------------------------
	// Rendering utility methods.
	//-----------------------------------------------------------------------------------------------

	void CDX12Graphics::RenderEmptyBuffer(PRIMITIVE_TOPOLOGY topology, u32 vertexCount, u32 instanceCount)
	{
		m_pCommandRealtimeList->IASetPrimitiveTopology(ConvertPrimitiveTopologyToD3D12(topology));
		m_pCommandRealtimeList->IASetVertexBuffers(0, 0, nullptr);
		m_pCommandRealtimeList->DrawInstanced(vertexCount, instanceCount, 0, 0);
	}

	//-----------------------------------------------------------------------------------------------
	// Viewport resizing and mode adjustment methods.
	//-----------------------------------------------------------------------------------------------

	void CDX12Graphics::AdjustToResize()
	{
		if(!m_pSwapChain) { return; }

		WaitForGpu();

		for(u8 i = 0; i < m_data.bufferCount; ++i)
		{
			SAFE_RELEASE(m_pRenderTargetList[i]);
		}

		auto pWinPanel = dynamic_cast<const App::CWinPanel*>(m_data.pPanel);

		// Update swapchain description.
		m_swapChainDesc.Width = UINT(pWinPanel->GetRect().w);
		m_swapChainDesc.Height = UINT(pWinPanel->GetRect().h);

		m_viewport.Width = FLOAT(pWinPanel->GetRect().w);
		m_viewport.Height = FLOAT(pWinPanel->GetRect().h);
		m_scissor.right = LONG(pWinPanel->GetRect().w);
		m_scissor.bottom = LONG(pWinPanel->GetRect().h);

		m_aspectRatio = m_viewport.Width / m_viewport.Height;

		ASSERT_HR_R(m_pSwapChain->ResizeBuffers(m_swapChainDesc.BufferCount, m_swapChainDesc.Width,
			m_swapChainDesc.Height, m_swapChainDesc.Format, m_swapChainDesc.Flags));

		{ // Create frame resources.
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_pRTVHeap->GetCPUDescriptorHandleForHeapStart());

			// Create a RTV and command allocators for each frame.
			for(u32 i = 0; i < m_data.bufferCount; ++i)
			{
				ASSERT_HR_R(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargetList[i])));
				m_pDevice->CreateRenderTargetView(m_pRenderTargetList[i], nullptr, rtvHandle);
				rtvHandle.Offset(1, m_rtvDescriptorSize);

				NAME_D3D12_OBJECT_INDEXED(m_pRenderTargetList, i);
			}
		}

		for(u8 i = 0; i < m_data.bufferCount; ++i)
		{
			m_pFenceValueList[i] = m_pFenceValueList[m_frameIndex];
		}

		// Make sure that GPU work is completed before resetting allocators.
		m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	}

	void CDX12Graphics::FinalizeResize()
	{
		if(!m_pSwapChain) { return; }
	}

	//-----------------------------------------------------------------------------------------------
	// DX12 specific public utility methods.
	//-----------------------------------------------------------------------------------------------

	ID3D12GraphicsCommandList* CDX12Graphics::CreateBundle(const CMaterial* pMaterial)
	{
		return dynamic_cast<const CDX12Worker*>(Util::CJobSystem::Instance().GetWorker().GetGraphicsWorker())->CreateBundle(pMaterial);
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	ADAPTER_ORDER CDX12Graphics::PopulateAdapterList(IDXGIFactory* pFactory, DXGI_GPU_PREFERENCE preference)
	{
		m_adapterDataList.clear();

		DXGI_ADAPTER_DESC1 adapterDesc;
		ComPtr<IDXGIAdapter1> pAdapter;
		ComPtr<IDXGIFactory6> pFactory6;

		// Lambda for generating and pushing adapter data from dxgi adapter description.
		auto PushAdapter = [this](u32 index, const DXGI_ADAPTER_DESC1& desc) {
			// Create adapter data for adapter data list.
			AdapterData data { };
			wcscpy_s(data.description, desc.Description);
			data.index = index;
			data.vendorId = desc.VendorId;
			data.deviceId = desc.DeviceId;
			data.subSysId = desc.SubSysId;
			data.revision = desc.Revision;
			data.dedicatedVideoMemory = desc.DedicatedVideoMemory;
			data.dedicatedSystemMemory = desc.DedicatedSystemMemory;
			data.sharedSystemMemory = desc.SharedSystemMemory;

			m_adapterDataList.push_back(data);
		};

		// Attempt to cycle through adapters in preferred order.
		if(SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&pFactory6))))
		{
			for(u32 adapterIndex = 0; SUCCEEDED(pFactory6->EnumAdapterByGpuPreference(adapterIndex, preference, IID_PPV_ARGS(&pAdapter))); ++adapterIndex)
			{
				pAdapter->GetDesc1(&adapterDesc);

				// Check if adapter is supported, and skip it if it's not.
				if(adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) { continue; }
				if(FAILED(D3D12CreateDevice(pAdapter.Get(), m_featureLevel, _uuidof(ID3D12Device), nullptr))) { continue; }

				PushAdapter(adapterIndex, adapterDesc);
			}
		}

		if(m_adapterDataList.size() > 0)
		{
			// Adapter list has been generated in a sorted order.
			return ADAPTER_ORDER_SORTED;
		}
		else
		{
			// If no adapters were discovered from the preferred order pass, look for adapters without a preference to the order.
			ComPtr<IDXGIFactory1> pFactory1;
			if(SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&pFactory1))))
			{
				for(u32 adapterIndex = 0; pFactory1->EnumAdapters1(adapterIndex, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++adapterIndex)
				{
					pAdapter->GetDesc1(&adapterDesc);

					// Check if adapter is supported, and skip it if it's not.
					if(adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) { continue; }
					if(FAILED(D3D12CreateDevice(pAdapter.Get(), m_featureLevel, _uuidof(ID3D12Device), nullptr))) { continue; }

					PushAdapter(adapterIndex, adapterDesc);
				}
			}

			if(m_adapterDataList.size() > 0)
			{
				return ADAPTER_ORDER_UNSORTED;
			}
			else
			{
				return ADAPTER_ORDER_EMPTY;
			}
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Synchronization methods.
	//-----------------------------------------------------------------------------------------------

	// Wait for pending GPU work to complete.
	void CDX12Graphics::WaitForGpu()
	{
		// Schedule a Signal command in the queue.
		ASSERT_HR_R(m_pCommandQueue->Signal(m_pFence, m_pFenceValueList[m_frameIndex]));

		// Wait until the fence has been processed.
		ASSERT_HR_R(m_pFence->SetEventOnCompletion(m_pFenceValueList[m_frameIndex], m_fenceEvent));
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

		// Increment the fence value for the current frame.
		++m_pFenceValueList[m_frameIndex];
	}

	// Prepare to render the next frame.
	void CDX12Graphics::MoveToNextFrame()
	{
		// Schedule a Signal command in the queue.
		const u64 currentFenceValue = m_pFenceValueList[m_frameIndex];
		ASSERT_HR_R(m_pCommandQueue->Signal(m_pFence, currentFenceValue));

		// Update the frame index.
		m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		// If the next frame is not ready to be rendered yet, wait until it is ready.
		if(m_pFence->GetCompletedValue() < m_pFenceValueList[m_frameIndex])
		{
			ASSERT_HR_R(m_pFence->SetEventOnCompletion(m_pFenceValueList[m_frameIndex], m_fenceEvent));
			WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
		}

		// Set the fence value for the next frame.
		m_pFenceValueList[m_frameIndex] = currentFenceValue + 1;

		++m_frame;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CDX12Graphics::Screenshot(const wchar_t* filename, IMAGE_FILETYPE type)
	{
		WaitForGpu();

		HRESULT hr;

		if(type == IMAGE_FILETYPE_DDS)
		{
			hr = DirectX::SaveDDSTextureToFile(m_pCommandQueue, m_pRenderTargetList[m_frameIndex], filename, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PRESENT);
		}
		else
		{
			GUID typeId;

			switch(type)
			{
				case Graphics::IMAGE_FILETYPE_BMP:
					typeId = GUID_ContainerFormatBmp;
					break;
				case Graphics::IMAGE_FILETYPE_ICO:
					typeId = GUID_ContainerFormatIco;
					break;
				case Graphics::IMAGE_FILETYPE_GIF:
					typeId = GUID_ContainerFormatGif;
					break;
				case Graphics::IMAGE_FILETYPE_JPEG:
					typeId = GUID_ContainerFormatJpeg;
					break;
				case Graphics::IMAGE_FILETYPE_PNG:
					typeId = GUID_ContainerFormatPng;
					break;
				case Graphics::IMAGE_FILETYPE_TIFF:
					typeId = GUID_ContainerFormatTiff;
					break;
				case Graphics::IMAGE_FILETYPE_WMP:
					typeId = GUID_ContainerFormatWmp;
					break;
				default:
					typeId = GUID_ContainerFormatBmp;
					break;
			}

			hr = DirectX::SaveWICTextureToFile(m_pCommandQueue, m_pRenderTargetList[m_frameIndex], typeId, filename, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_PRESENT, nullptr, nullptr, true);
		}

		ASSERT_HR(hr);

		if(SUCCEEDED(hr))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};
