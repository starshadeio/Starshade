//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12MeshRenderer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDX12MESHRENDERER_H
#define CDX12MESHRENDERER_H

#include "CMeshRenderer.h"
#include <d3d12.h>

namespace Graphics
{
	class CDX12MeshRenderer : public CMeshRenderer
	{
	public:
		struct DX12Data : Data
		{
		private:
			friend class CDX12MeshRenderer;

		public:
			DX12Data(u64 thisHash);
			~DX12Data();
			DX12Data(const DX12Data&) = default;
			DX12Data(DX12Data&&) = default;
			DX12Data& operator = (const DX12Data&) = default;
			DX12Data& operator = (DX12Data&&) = default;

		private:
			void Initialize() final;
			void PostInitialize();
			void Render() final;
			void Release();

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

	public:
		CDX12MeshRenderer();
		~CDX12MeshRenderer();
		CDX12MeshRenderer(const CDX12MeshRenderer&) = delete;
		CDX12MeshRenderer(CDX12MeshRenderer&&) = delete;
		CDX12MeshRenderer& operator = (const CDX12MeshRenderer&) = delete;
		CDX12MeshRenderer& operator = (CDX12MeshRenderer&&) = delete;

	protected:
		void Register() final;

		void Initialize() final;
		void PostInitialize() final;
		void Render(u32 viewHash) final;
		void Release() final;

	private:
		void* AddToObject(CNodeObject* pObject) final;
		void* GetFromObject(const CNodeObject* pObject) final;
		bool TryToGetFromObject(const CNodeObject* pObject, void** ppData) final;
		void RemoveFromObject(CNodeObject* pObject) final;

		void* AddToObject(u64 objHash) final;
		void* GetFromObject(u64 objHash) final;
		bool TryToGetFromObject(u64 objHash, void** ppData) final;
		void RemoveFromObject(u64 objHash) final;

	private:
		std::unordered_map<u32, std::unordered_map<u64, DX12Data>> m_fullMap;
		std::unordered_map<u64, DX12Data>* m_pDataMap;
	};
};

#endif
