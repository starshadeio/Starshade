//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12Shader.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDX12SHADER_H
#define CDX12SHADER_H

#include "CShader.h"
#include <d3d12.h>

namespace Graphics
{
	class CDX12Shader : public CShader
	{
	public:
		CDX12Shader();
		~CDX12Shader();
		CDX12Shader(const CDX12Shader&) = delete;
		CDX12Shader(CDX12Shader&&) = delete;
		CDX12Shader& operator = (const CDX12Shader&) = delete;
		CDX12Shader& operator = (CDX12Shader&&) = delete;

		void Initialize(class CShaderFile* pShaderFile) final;
		void Bind() final;
		void Release() final;

		// Accessors.
		inline ID3D12PipelineState* GetPipelineState() const { return m_pPipelineState; }

	private:
		D3D12_INPUT_ELEMENT_DESC* m_pInputLayout;

		ID3D12PipelineState* m_pPipelineState;
		ID3D12GraphicsCommandList* m_pCommandList;
		class CDX12Graphics* m_pDX12Graphics;
	};
};

#endif
