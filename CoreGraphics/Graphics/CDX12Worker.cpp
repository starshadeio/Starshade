//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12Worker.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef GRAPHICS_DX12

#include "CDX12Worker.h"
#include "CDX12Graphics.h"
#include "CDX12Data.h"
#include "CMaterial.h"
#include "CShader.h"
#include "CDX12Shader.h"
#include "CDX12RootSignature.h"
#include "../Factory/CFactory.h"
#include <Utilities/CDebugError.h>
#include <Utilities/CMemoryFree.h>

namespace Graphics
{
	CDX12Worker::CDX12Worker() :
		m_bDirty(false),
		m_pFence(nullptr),
		m_fenceValue(0),
		m_fenceEvent(nullptr),
		m_pBundleAllocator(nullptr),
		m_pCommandAllocator(nullptr),
		m_pCommandList(nullptr),
		m_pDX12Graphics(nullptr) {
	}

	CDX12Worker::~CDX12Worker() { }

	void CDX12Worker::Initialize(bool bRecord)
	{
		m_pDX12Graphics = dynamic_cast<CDX12Graphics*>(CFactory::Instance().GetGraphicsAPI());

		{ // Create synchronization objects and wait until assets have been uploaded to the GPU.
			m_fenceValue = 0;
			ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)));
			++m_fenceValue;
			
			// Create an event handle to use for frame synchronization.
			m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if(m_fenceEvent == nullptr)
			{
				ASSERT_HR_R(HRESULT_FROM_WIN32(GetLastError()));
			}
		}
		
		ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&m_pBundleAllocator)));
		NAME_D3D12_OBJECT(m_pBundleAllocator);

		ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator)));
		NAME_D3D12_OBJECT(m_pCommandAllocator);
		
		ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator, nullptr, IID_PPV_ARGS(&m_pCommandList)));
		if(!bRecord) { ASSERT_HR_R(m_pCommandList->Close()); }
	}

	void CDX12Worker::Release()
	{
		Sync();

		CloseHandle(m_fenceEvent);
		SAFE_RELEASE(m_pFence);
		m_fenceValue = 0;

		SAFE_RELEASE(m_pCommandList);
		SAFE_RELEASE(m_pCommandAllocator);
		SAFE_RELEASE(m_pBundleAllocator);

		m_pDX12Graphics = nullptr;
	}

	void CDX12Worker::Record()
	{
		if(m_bDirty) { Sync(); }
		m_bDirty = true;

		ASSERT_HR_R(m_pCommandAllocator->Reset());
		ASSERT_HR_R(m_pCommandList->Reset(m_pCommandAllocator, nullptr));
	}

	void CDX12Worker::Execute()
	{
		ASSERT_HR_R(m_pCommandList->Close());
		ID3D12CommandList* ppCommandLists[] = { m_pCommandList };
		m_pDX12Graphics->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}
	
	void CDX12Worker::LoadAssets(std::function<void()> func)
	{
		Record();
		func();
		Execute();
	}

	void CDX12Worker::Sync()
	{
		if(!m_bDirty) { return; }

		// Schedule a Signal command in the queue.
		ASSERT_HR_R(m_pDX12Graphics->GetCommandQueue()->Signal(m_pFence, m_fenceValue));

		// Wait until the fence has been processed.
		ASSERT_HR_R(m_pFence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
		WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

		// Increment the fence value for the current frame.
		++m_fenceValue;
		
		m_bDirty = false;
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	ID3D12GraphicsCommandList* CDX12Worker::CreateBundle(const class CMaterial* pMaterial) const
	{
		const CDX12Shader* pShader = reinterpret_cast<const CDX12Shader*>(pMaterial->GetShader());

		ID3D12GraphicsCommandList* pBundle = nullptr;
		ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_BUNDLE, m_pBundleAllocator, pShader->GetPipelineState(), IID_PPV_ARGS(&pBundle)));
		NAME_D3D12_OBJECT(pBundle);

		ID3D12RootSignature* pRootSignature = dynamic_cast<CDX12RootSignature*>(pShader->GetRootSignature())->GetRootSignature();
		if(pShader->IsCompute())
		{
			pBundle->SetComputeRootSignature(pRootSignature);
		}
		else
		{
			pBundle->SetGraphicsRootSignature(pRootSignature);
		}

		return pBundle;
	}
};

#endif
