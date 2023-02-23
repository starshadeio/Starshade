//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12Worker.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDX12WORKER_H
#define CDX12WORKER_H

#include "CGraphicsWorker.h"
#include <Globals/CGlobals.h>
#include <d3d12.h>

namespace Graphics
{
	class CDX12Worker : public CGraphicsWorker
	{
	public:
		CDX12Worker();
		~CDX12Worker();
		CDX12Worker(const CDX12Worker&) = delete;
		CDX12Worker(CDX12Worker&&) = delete;
		CDX12Worker& operator = (const CDX12Worker&) = delete;
		CDX12Worker& operator = (CDX12Worker&&) = delete;
		
		void Initialize(bool bRecord) final;
		void Release() final;
		
		void Record() final;
		void Execute() final;

		void LoadAssets(std::function<void()> func) final;
		void Sync() final;
		
		ID3D12GraphicsCommandList* CreateBundle(const class CMaterial* pMaterial) const;

		// Accessors.
		inline ID3D12GraphicsCommandList* GetCommandList() const { return m_pCommandList; }

	private:
		bool m_bDirty;

		ID3D12Fence* m_pFence;
		u64 m_fenceValue;
		HANDLE m_fenceEvent;

		ID3D12CommandAllocator* m_pCommandAllocator;
		ID3D12CommandAllocator* m_pBundleAllocator;
		ID3D12GraphicsCommandList* m_pCommandList;
		class CDX12Graphics* m_pDX12Graphics;
	};
};

#endif
