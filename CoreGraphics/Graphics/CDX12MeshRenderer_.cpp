//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12MeshRenderer_.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef GRAPHICS_DX12

#include "CDX12MeshRenderer_.h"
#include "CDX12Graphics.h"
#include "CDX12Data.h"
#include "CMeshData_.h"
#include "CMaterial.h"
#include "CShader.h"
#include "CDX12RootSignature.h"
#include "d3dx12.h"
#include "../Factory/CFactory.h"
#include "../Application/CSceneManager.h"
#include "../Utilities/CGarbage.h"
#include <Utilities/CDebugError.h>
#include <Utilities/CMemoryFree.h>

namespace Graphics
{
	CDX12MeshRenderer_::CDX12MeshRenderer_(const CVObject* pObject) :
		CMeshRenderer_(pObject),
		m_pInstanceData(nullptr),
		m_pVertexBuffer(nullptr),
		m_pVertexBufferUpload(nullptr),
		m_pInstanceBuffer(nullptr),
		m_pInstanceBufferUpload(nullptr),
		m_pIndexBuffer(nullptr),
		m_pIndexBufferUpload(nullptr),
		m_pBundleList(nullptr),
		m_pDX12Graphics(nullptr) {
	}

	CDX12MeshRenderer_::~CDX12MeshRenderer_() { }

	void CDX12MeshRenderer_::Initialize()
	{
		CMeshRenderer_::Initialize();

		m_pDX12Graphics = reinterpret_cast<CDX12Graphics*>(CFactory::Instance().GetGraphicsAPI());

		CD3DX12_RESOURCE_BARRIER barrierList[3];
		u32 barrierCount = 0;

		if(m_data.pMeshData->GetVertexSize())
		{
			// Create the vertex buffer's resource.
			CreateResource(&m_pVertexBuffer, &m_pVertexBufferUpload, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
				m_data.pMeshData->GetVertexSize(), m_data.pMeshData->GetVertexStride(), m_data.pMeshData->GetVertexList(), nullptr);

			// Create the vertex buffer's view.
			m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
			m_vertexBufferView.StrideInBytes = m_data.pMeshData->GetVertexStride();
			m_vertexBufferView.SizeInBytes = m_data.pMeshData->GetVertexSize();

			// Make sure to transition the vertex buffer to a state that can be feed through the pipeline for rendering.
			barrierList[barrierCount++] = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}

		if(m_data.pMeshData->GetInstanceSizeMax())
		{
			// Create the instance buffer's resource.
			CreateResource(&m_pInstanceBuffer, m_data.bDynamicInstances ? nullptr : &m_pInstanceBufferUpload, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
				m_data.pMeshData->GetInstanceSizeMax(), m_data.pMeshData->GetInstanceStride(), m_data.pMeshData->GetInstanceList(), m_data.bDynamicInstances ? &m_pInstanceData : nullptr);

			// Create the instance buffer's view.
			m_instanceBufferView.BufferLocation = m_pInstanceBuffer->GetGPUVirtualAddress();
			m_instanceBufferView.StrideInBytes = m_data.pMeshData->GetInstanceStride();
			m_instanceBufferView.SizeInBytes = m_data.pMeshData->GetInstanceSize();

			if(!m_data.bDynamicInstances)
			{
				// Make sure to transition the instance buffer to a state that can be feed through the pipeline for rendering.
				barrierList[barrierCount++] = CD3DX12_RESOURCE_BARRIER::Transition(m_pInstanceBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			}
			else if(!m_data.bDynamicInstanceCount)
			{
				m_data.pMeshData->SetInstanceCount(m_data.pMeshData->GetInstanceMax());
				m_data.pMeshData->ReplaceInstanceList(m_pInstanceData);
			}
		}

		if(m_data.pMeshData->GetIndexSize())
		{
			// Create the index buffer's resource.
			CreateResource(&m_pIndexBuffer, &m_pIndexBufferUpload, D3D12_RESOURCE_STATE_INDEX_BUFFER,
				m_data.pMeshData->GetIndexSize(), m_data.pMeshData->GetIndexStride(), m_data.pMeshData->GetIndexList(), nullptr);

			// Create the index buffer's view.
			m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
			m_indexBufferView.Format = m_data.pMeshData->GetIndexStride() == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			m_indexBufferView.SizeInBytes = m_data.pMeshData->GetIndexSize();

			// Make sure to transition the index buffer to a state that can be feed through the pipeline for rendering.
			barrierList[barrierCount++] = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		}

		if(barrierCount)
		{
			m_pDX12Graphics->GetAssetCommandList()->ResourceBarrier(barrierCount, barrierList);
		}

		// Create bundle for rendering.
		m_pBundleList = new ID3D12GraphicsCommandList*[m_pMaterialList.size()];
		for(size_t i = 0; i < m_pMaterialList.size(); ++i)
		{
			m_pBundleList[i] = m_pDX12Graphics->CreateBundle(m_pMaterialList[i]);
			m_pBundleList[i]->IASetPrimitiveTopology(ConvertPrimitiveTopologyToD3D12(m_data.pMeshData->GetTopology()));

			// Index buffer and draw setup.
			if(m_data.pMeshData->GetIndexSize())
			{
				m_pBundleList[i]->IASetIndexBuffer(&m_indexBufferView);
			}

			if(!m_data.bDynamicInstances || !m_data.bDynamicInstanceCount)
			{ // Bundle based drawing is only supported for renderers without a dynamically counted instance buffer.
				SetupDraw(m_pBundleList[i]);
			}

			ASSERT_HR_R(m_pBundleList[i]->Close());
		}

		auto pIndexBufferUpload = m_pIndexBufferUpload;
		auto pInstanceBufferUpload = m_pInstanceBufferUpload;
		auto pVertexBufferUpload = m_pVertexBufferUpload;

		App::CSceneManager::Instance().Garbage().Dispose([pIndexBufferUpload, pInstanceBufferUpload, pVertexBufferUpload](){
			if(pIndexBufferUpload) pIndexBufferUpload->Release();
			if(pInstanceBufferUpload) pInstanceBufferUpload->Release();
			if(pVertexBufferUpload) pVertexBufferUpload->Release();
		});

		m_pIndexBufferUpload = m_pInstanceBufferUpload = m_pVertexBufferUpload = nullptr;
	}

	void CDX12MeshRenderer_::RenderWithMaterial(size_t materialIndex)
	{
		const bool bPreFrameUpdate = m_data.bDynamicInstances && m_data.bDynamicInstanceCount;

		if(bPreFrameUpdate)
		{
			if(m_data.pMeshData->ResetInstanceDirty())
			{ // If instance data has been updated, make sure to copy its contents over to the mapped instance buffer.
				memcpy(m_pInstanceData, m_data.pMeshData->GetInstanceList(), m_data.pMeshData->GetInstanceSize());
			}
		}

		CMeshRenderer_::RenderWithMaterial(materialIndex);

		m_pDX12Graphics->GetRealtimeCommandList()->ExecuteBundle(m_pBundleList[materialIndex]);

		if(bPreFrameUpdate)
		{ // Dynamic instance buffers require pre-frame draw configuration.
			SetupDraw(m_pDX12Graphics->GetRealtimeCommandList());
		}
	}

	void CDX12MeshRenderer_::Release()
	{
		CMeshRenderer_::Release();

		for(size_t i = 0; i < m_pMaterialList.size(); ++i)
		{
			SAFE_RELEASE(m_pBundleList[i]);
		} SAFE_DELETE_ARRAY(m_pBundleList);

		SAFE_RELEASE(m_pIndexBuffer);
		SAFE_RELEASE(m_pInstanceBuffer);
		SAFE_RELEASE(m_pVertexBuffer);

		/*SAFE_RELEASE(m_pIndexBufferUpload);
		SAFE_RELEASE(m_pInstanceBufferUpload);
		SAFE_RELEASE(m_pVertexBufferUpload);*/
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CDX12MeshRenderer_::CreateResource(ID3D12Resource** ppBuffer, ID3D12Resource** ppBufferUpload, D3D12_RESOURCE_STATES nextState, u32 size, u32 stride, const u8* pSrcData, u8** pDstData)
	{
		{ // Create the buffer.
			CD3DX12_HEAP_PROPERTIES heapProps(ppBufferUpload ? D3D12_HEAP_TYPE_DEFAULT : D3D12_HEAP_TYPE_UPLOAD);
			CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

			ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				ppBufferUpload ? D3D12_RESOURCE_STATE_COPY_DEST : (pDstData ? D3D12_RESOURCE_STATE_GENERIC_READ : nextState),
				nullptr,
				IID_PPV_ARGS(ppBuffer)
			));

			NAME_D3D12_OBJECT(*ppBuffer);
		}

		if(ppBufferUpload)
		{
			{ // Create the upload buffer.
				const u64 uploadBufferSize = GetRequiredIntermediateSize(*ppBuffer, 0, 1);
				CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
				CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

				ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(ppBufferUpload)
				));
			}

			// Prepare for the upload of data.
			D3D12_SUBRESOURCE_DATA subResData { };
			subResData.pData = pSrcData;
			subResData.RowPitch = static_cast<LONG_PTR>(size);
			subResData.SlicePitch = subResData.RowPitch;

			UpdateSubresources<1>(m_pDX12Graphics->GetAssetCommandList(), *ppBuffer, *ppBufferUpload, 0, 0, 1, &subResData);
		}
		else
		{
			// Copy data to buffer.
			D3D12_RANGE readRange = { 0, 0 };
			ASSERT_HR_R((*ppBuffer)->Map(0, &readRange, reinterpret_cast<void**>(pDstData)));
			memcpy(*pDstData, pSrcData, size);
		}
	}

	// Method for setting up draw in a commandlist, which can also be a bundle.
	void CDX12MeshRenderer_::SetupDraw(ID3D12GraphicsCommandList* pCommandList)
	{
		// Vertex buffer setup.
		if(m_data.pMeshData->GetVertexSize())
		{
			if(m_data.pMeshData->GetInstanceSizeMax())
			{
				m_instanceBufferView.SizeInBytes = m_data.pMeshData->GetInstanceSize();
				if(m_instanceBufferView.SizeInBytes)
				{
					D3D12_VERTEX_BUFFER_VIEW views[] = { m_vertexBufferView, m_instanceBufferView };
					pCommandList->IASetVertexBuffers(0, 2, views);
				}
			}
			else
			{
				pCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
			}
		}

		// Index buffer and draw setup.
		if(m_data.pMeshData->GetIndexSize())
		{ // Setup up vertex/index draw.
			pCommandList->IASetIndexBuffer(&m_indexBufferView);
			
			if(m_data.pMeshData->GetInstanceSizeMax())
			{
				if(m_instanceBufferView.SizeInBytes)
				{
					pCommandList->DrawIndexedInstanced(m_data.pMeshData->GetIndexCount(), m_data.pMeshData->GetInstanceCount(), 0, 0, 0);
				}
			}
			else
			{
				pCommandList->DrawIndexedInstanced(m_data.pMeshData->GetIndexCount(), 1, 0, 0, 0);
			}
		}
		else
		{ // Setup up vertex only draw.
			if(m_data.pMeshData->GetInstanceSizeMax())
			{
				if(m_instanceBufferView.SizeInBytes)
				{
					pCommandList->DrawInstanced(m_data.pMeshData->GetVertexCount(), m_data.pMeshData->GetInstanceCount(), 0, 0);
				}
			}
			else
			{
				pCommandList->DrawInstanced(m_data.pMeshData->GetVertexCount(), 1, 0, 0);
			}
		}
	}
};

#endif
