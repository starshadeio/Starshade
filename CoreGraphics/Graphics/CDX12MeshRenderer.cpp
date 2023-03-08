//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12MeshRenderer.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef GRAPHICS_DX12

#include "CDX12MeshRenderer.h"
#include "CDX12Graphics.h"
#include "CDX12Data.h"
#include "CMeshFilter.h"
#include "CMesh.h"
#include "CMaterial.h"
#include "CShader.h"
#include "CDX12RootSignature.h"
#include "d3dx12.h"
#include "../Factory/CFactory.h"
#include <Application/CCoreManager.h>
#include <Utilities/CDebugError.h>
#include <Utilities/CMemoryFree.h>
#include <Objects/CNodeObject.h>

namespace Graphics
{
	//-----------------------------------------------------------------------------------------------
	// CDX12MeshRenderer::Data
	//-----------------------------------------------------------------------------------------------

	CDX12MeshRenderer::DX12Data::DX12Data(u64 thisHash) : 
		Data(thisHash),
		m_pInstanceData(nullptr),
		m_pVertexBuffer(nullptr),
		m_pVertexBufferUpload(nullptr),
		m_pInstanceBuffer(nullptr),
		m_pInstanceBufferUpload(nullptr),
		m_pIndexBuffer(nullptr),
		m_pIndexBufferUpload(nullptr),
		m_pBundleList(nullptr),
		m_pDX12Graphics(nullptr)
	{
	}

	CDX12MeshRenderer::DX12Data::~DX12Data()
	{
	}

	void CDX12MeshRenderer::DX12Data::Initialize()
	{
		Data::Initialize();
		
		m_pDX12Graphics = reinterpret_cast<CDX12Graphics*>(CFactory::Instance().GetGraphicsAPI());
		auto& meshFilter = *reinterpret_cast<CMeshFilter::Data*>(App::CCoreManager::Instance().NodeRegistry().GetComponentFromObject(CMeshFilter::HASH, GetThis()));
		auto& mesh = *meshFilter.GetMesh();

		CD3DX12_RESOURCE_BARRIER barrierList[3];
		u32 barrierCount = 0;

		if(mesh.GetVertexSize())
		{
			// Create the vertex buffer's resource.
			CreateResource(&m_pVertexBuffer, &m_pVertexBufferUpload, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
				mesh.GetVertexSize(), mesh.GetVertexStride(), mesh.GetVertexList(), nullptr);

			// Create the vertex buffer's view.
			m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
			m_vertexBufferView.StrideInBytes = mesh.GetVertexStride();
			m_vertexBufferView.SizeInBytes = mesh.GetVertexSize();

			// Make sure to transition the vertex buffer to a state that can be feed through the pipeline for rendering.
			barrierList[barrierCount++] = CD3DX12_RESOURCE_BARRIER::Transition(m_pVertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}

		if(meshFilter.GetInstanceSizeMax())
		{
			// Create the instance buffer's resource.
			CreateResource(&m_pInstanceBuffer, IsUsingDynamicInstanceData() ? nullptr : &m_pInstanceBufferUpload, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
				meshFilter.GetInstanceSizeMax(), meshFilter.GetInstanceStride(), meshFilter.GetInstanceList(), IsUsingDynamicInstanceData() ? &m_pInstanceData : nullptr);

			// Create the instance buffer's view.
			m_instanceBufferView.BufferLocation = m_pInstanceBuffer->GetGPUVirtualAddress();
			m_instanceBufferView.StrideInBytes = meshFilter.GetInstanceStride();
			m_instanceBufferView.SizeInBytes = meshFilter.GetInstanceSize();

			if(!IsUsingDynamicInstanceData())
			{
				// Make sure to transition the instance buffer to a state that can be feed through the pipeline for rendering.
				barrierList[barrierCount++] = CD3DX12_RESOURCE_BARRIER::Transition(m_pInstanceBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
			}
			else if(!IsUsingDynamicInstanceCount())
			{
				meshFilter.SetInstanceCount(meshFilter.GetInstanceMax());
				meshFilter.ReplaceInstanceList(m_pInstanceData);
			}
		}

		if(mesh.GetIndexSize())
		{
			// Create the index buffer's resource.
			CreateResource(&m_pIndexBuffer, &m_pIndexBufferUpload, D3D12_RESOURCE_STATE_INDEX_BUFFER,
				mesh.GetIndexSize(), mesh.GetIndexStride(), mesh.GetIndexList(), nullptr);

			// Create the index buffer's view.
			m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
			m_indexBufferView.Format = mesh.GetIndexStride() == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			m_indexBufferView.SizeInBytes = mesh.GetIndexSize();

			// Make sure to transition the index buffer to a state that can be feed through the pipeline for rendering.
			barrierList[barrierCount++] = CD3DX12_RESOURCE_BARRIER::Transition(m_pIndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
		}

		if(barrierCount)
		{
			m_pDX12Graphics->GetAssetCommandList()->ResourceBarrier(barrierCount, barrierList);
		}
		
		// Create bundle for rendering.
		m_pBundleList = new ID3D12GraphicsCommandList*[GetMaterialCount()];
		for(size_t i = 0; i < GetMaterialCount(); ++i)
		{
			m_pBundleList[i] = m_pDX12Graphics->CreateBundle(GetMaterialAt(i));
			m_pBundleList[i]->IASetPrimitiveTopology(ConvertPrimitiveTopologyToD3D12(mesh.GetTopology()));

			// Index buffer and draw setup.
			if(mesh.GetIndexSize())
			{
				m_pBundleList[i]->IASetIndexBuffer(&m_indexBufferView);
			}

			if(!IsUsingDynamicInstanceData() || !IsUsingDynamicInstanceCount())
			{ // Bundle based drawing is only supported for renderers without a dynamically counted instance buffer.
				SetupDraw(m_pBundleList[i]);
			}

			ASSERT_HR_R(m_pBundleList[i]->Close());
		}
	}

	void CDX12MeshRenderer::DX12Data::PostInitialize()
	{
		SAFE_RELEASE(m_pIndexBufferUpload);
		SAFE_RELEASE(m_pInstanceBufferUpload);
		SAFE_RELEASE(m_pVertexBufferUpload);
	}

	void CDX12MeshRenderer::DX12Data::RenderWithMaterial(size_t materialIndex)
	{
		const bool bPreFrameUpdate = IsUsingDynamicInstanceData() && IsUsingDynamicInstanceCount();

		if(bPreFrameUpdate)
		{
			auto& meshFilter = *reinterpret_cast<CMeshFilter::Data*>(App::CCoreManager::Instance().NodeRegistry().GetComponentFromObject(CMeshFilter::HASH, GetThis()));

			if(meshFilter.ResetInstanceDirty())
			{ // If instance data has been updated, make sure to copy its contents over to the mapped instance buffer.
				memcpy(m_pInstanceData, meshFilter.GetInstanceList(), meshFilter.GetInstanceSize());
			}
		}
		
		Data::RenderWithMaterial(materialIndex);
		
		m_pDX12Graphics->GetRealtimeCommandList()->ExecuteBundle(m_pBundleList[materialIndex]);

		if(bPreFrameUpdate)
		{ // Dynamic instance buffers require pre-frame draw configuration.
			SetupDraw(m_pDX12Graphics->GetRealtimeCommandList());
		}
	}

	void CDX12MeshRenderer::DX12Data::Release()
	{
		for(size_t i = 0; i < GetMaterialCount(); ++i)
		{
			SAFE_RELEASE(m_pBundleList[i]);
		} SAFE_DELETE_ARRAY(m_pBundleList);

		SAFE_RELEASE(m_pIndexBuffer);
		SAFE_RELEASE(m_pInstanceBuffer);
		SAFE_RELEASE(m_pVertexBuffer);

		SAFE_RELEASE(m_pIndexBufferUpload);
		SAFE_RELEASE(m_pInstanceBufferUpload);
		SAFE_RELEASE(m_pVertexBufferUpload);
	}

	void CDX12MeshRenderer::DX12Data::CreateResource(ID3D12Resource** ppBuffer, ID3D12Resource** ppBufferUpload, D3D12_RESOURCE_STATES nextState, u32 size, u32 stride, const u8* pSrcData, u8** pDstData)
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

	void CDX12MeshRenderer::DX12Data::SetupDraw(ID3D12GraphicsCommandList* pCommandList)
	{
		auto& meshFilter = *reinterpret_cast<CMeshFilter::Data*>(App::CCoreManager::Instance().NodeRegistry().GetComponentFromObject(CMeshFilter::HASH, GetThis()));
		auto& mesh = *meshFilter.GetMesh();

		// Vertex buffer setup.
		if(mesh.GetVertexSize())
		{
			if(meshFilter.GetInstanceSizeMax())
			{
				m_instanceBufferView.SizeInBytes = meshFilter.GetInstanceSize();
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
		if(mesh.GetIndexSize())
		{ // Setup up vertex/index draw.
			pCommandList->IASetIndexBuffer(&m_indexBufferView);
			
			if(meshFilter.GetInstanceSizeMax())
			{
				if(m_instanceBufferView.SizeInBytes)
				{
					pCommandList->DrawIndexedInstanced(mesh.GetIndexCount(), meshFilter.GetInstanceCount(), 0, 0, 0);
				}
			}
			else
			{
				pCommandList->DrawIndexedInstanced(mesh.GetIndexCount(), 1, 0, 0, 0);
			}
		}
		else
		{ // Setup up vertex only draw.
			if(meshFilter.GetInstanceSizeMax())
			{
				if(m_instanceBufferView.SizeInBytes)
				{
					pCommandList->DrawInstanced(mesh.GetVertexCount(), meshFilter.GetInstanceCount(), 0, 0);
				}
			}
			else
			{
				pCommandList->DrawInstanced(mesh.GetVertexCount(), 1, 0, 0);
			}
		}
	}

	//-----------------------------------------------------------------------------------------------
	// CDX12MeshRenderer methods.
	//-----------------------------------------------------------------------------------------------
	
	CDX12MeshRenderer::CDX12MeshRenderer() :
		CMeshRenderer(HASH)
	{
	}

	CDX12MeshRenderer::~CDX12MeshRenderer()
	{
	}

	void CDX12MeshRenderer::Register()
	{
		App::CCoreManager::Instance().NodeRegistry().RegisterComponent<CDX12MeshRenderer>();
	}

	void CDX12MeshRenderer::Initialize()
	{
		for(auto& dataMap : m_fullMap)
		{
			for(auto& elem : dataMap.second)
			{
				elem.second.Initialize();
			}
		}
	}
	
	void CDX12MeshRenderer::PostInitialize()
	{
		for(auto& dataMap : m_fullMap)
		{
			for(auto& elem : dataMap.second)
			{
				elem.second.PostInitialize();
			}
		}
	}

	void CDX12MeshRenderer::Render(u32 viewHash)
	{
		auto dataMap = m_fullMap.find(viewHash);
		if(dataMap == m_fullMap.end()) return;

		for(auto& elem : dataMap->second)
		{
			elem.second.Render();
		}
	}

	void CDX12MeshRenderer::Release()
	{
		for(auto& dataMap : m_fullMap)
		{
			for(auto& elem : dataMap.second)
			{
				elem.second.Release();
			}
		}
	}

	//---------------------------------------------
	// Object methods.
	//---------------------------------------------

	void* CDX12MeshRenderer::AddToObject(CNodeObject* pObject)
	{
		m_pDataMap = &m_fullMap.insert({ pObject->GetViewHash(), { } }).first->second;
		return CNodeComponent::AddToObject(pObject);
	}

	void* CDX12MeshRenderer::GetFromObject(const CNodeObject* pObject)
	{
		m_pDataMap = &m_fullMap.find(pObject->GetViewHash())->second;
		return CNodeComponent::GetFromObject(pObject);
	}

	bool CDX12MeshRenderer::TryToGetFromObject(const CNodeObject* pObject, void** ppData)
	{
		m_pDataMap = &m_fullMap.find(pObject->GetViewHash())->second;
		return CNodeComponent::TryToGetFromObject(pObject, ppData);
	}

	void CDX12MeshRenderer::RemoveFromObject(CNodeObject* pObject)
	{
		m_pDataMap = &m_fullMap.find(pObject->GetViewHash())->second;
		CNodeComponent::RemoveFromObject(pObject);
	}
	

	void* CDX12MeshRenderer::AddToObject(u64 objHash)
	{
		return &m_pDataMap->insert({ objHash, DX12Data(objHash) }).first->second;
	}
	
	void* CDX12MeshRenderer::GetFromObject(u64 objHash)
	{
		return &m_pDataMap->find(objHash)->second;
	}

	bool CDX12MeshRenderer::TryToGetFromObject(u64 objHash, void** ppData)
	{
		auto elem = m_pDataMap->find(objHash);
		if(elem == m_pDataMap->end()) return false;
		*ppData = &elem->second;
		return true;
	}

	void CDX12MeshRenderer::RemoveFromObject(u64 objHash)
	{
		m_pDataMap->erase(objHash);
	}
};

#endif
