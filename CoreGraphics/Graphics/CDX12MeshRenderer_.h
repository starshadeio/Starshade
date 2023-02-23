//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12MeshRenderer_.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDX12MESHRENDERER__H
#define CDX12MESHRENDERER__H

#include "CMeshRenderer_.h"
#include <d3d12.h>

namespace Graphics
{
	// This is legacy to CDX12MeshRenderer, and will eventually be replaced by it.
	class CDX12MeshRenderer_ : public CMeshRenderer_
	{
	public:
		CDX12MeshRenderer_(const CVObject* pObject);
		~CDX12MeshRenderer_();
		CDX12MeshRenderer_(const CDX12MeshRenderer_&) = delete;
		CDX12MeshRenderer_(CDX12MeshRenderer_&&) = delete;
		CDX12MeshRenderer_& operator = (const CDX12MeshRenderer_&) = delete;
		CDX12MeshRenderer_& operator = (CDX12MeshRenderer_&&) = delete;

	private:
		void Initialize() final;
		void PostInitialize() final;
		void Render() final;
		void Release() final;

		void CreateResource(ID3D12Resource** ppBuffer, ID3D12Resource** ppBufferUpload, D3D12_RESOURCE_STATES nextState, u32 size, u32 stride, const u8* pSrcData, u8** pDstData);
		void SetupDraw(ID3D12GraphicsCommandList* pCommandList);

	private:
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
		D3D12_VERTEX_BUFFER_VIEW m_instanceBufferView;
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

		u8* m_pInstanceData;

		ID3D12Resource* m_pVertexBuffer;
		ID3D12Resource* m_pVertexBufferUpload;
		ID3D12Resource* m_pInstanceBuffer;
		ID3D12Resource* m_pInstanceBufferUpload;
		ID3D12Resource* m_pIndexBuffer;
		ID3D12Resource* m_pIndexBufferUpload;

		ID3D12GraphicsCommandList* m_pBundle;

		class CDX12Graphics* m_pDX12Graphics;
	};
};

#endif
