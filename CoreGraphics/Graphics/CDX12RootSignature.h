//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12RootSignature.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDX12ROOTSIGNATURE_H
#define CDX12ROOTSIGNATURE_H

#include "CRootSignature.h"
#include <d3d12.h>

namespace Graphics
{
	class CDX12RootSignature : public CRootSignature
	{
	public:
		CDX12RootSignature();
		~CDX12RootSignature();
		CDX12RootSignature(const CDX12RootSignature&) = delete;
		CDX12RootSignature(CDX12RootSignature&&) = delete;
		CDX12RootSignature& operator = (const CDX12RootSignature&) = delete;
		CDX12RootSignature& operator = (CDX12RootSignature&&) = delete;

		void Initialize() final;
		void Bind() final;
		void Release() final;

		// Accessors.
		inline ID3D12RootSignature* GetRootSignature() const { return m_pRootSignature; }

	private:
		ID3D12RootSignature* m_pRootSignature;
		class CDX12Graphics* m_pDX12Graphics;
	};
};

#endif
