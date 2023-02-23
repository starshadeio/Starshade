//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDX12Texture.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef GRAPHICS_DX12

#include "CDX12Texture.h"
#include "CDX12Graphics.h"
#include "CDX12Data.h"
#include "CRenderTexture.h"
#include "d3dx12.h"
#include "../Factory/CFactory.h"
#include <Utilities/CDebugError.h>
#include <Utilities/CMemoryFree.h>

#include <DDSTextureLoader.h>
#include <ScreenGrab.h>

namespace Graphics
{
	CDX12Texture::CDX12Texture() :
		m_bHeapIndexed(false),
		m_csuIndex(0),
		m_rtvIndex(0),
		m_resourceStateFrom(D3D12_RESOURCE_STATE_COPY_DEST),
		m_resourceStateTo(D3D12_RESOURCE_STATE_GENERIC_READ),
		m_resourceState(D3D12_RESOURCE_STATE_GENERIC_READ),
		m_pTexture(nullptr),
		m_pTextureUpload(nullptr),
		m_pDX12Graphics(nullptr) {
	}

	CDX12Texture::~CDX12Texture() { }

	void CDX12Texture::Initialize()
	{
		m_pDX12Graphics = dynamic_cast<CDX12Graphics*>(CFactory::Instance().GetGraphicsAPI());

		D3D12_RESOURCE_DESC textureDesc { };
		textureDesc.MipLevels = 1;
		textureDesc.Format = ConvertGFXFormatToDXGI(m_data.textureFormat);
		textureDesc.Width = m_data.width;
		textureDesc.Height = m_data.height;
		textureDesc.DepthOrArraySize = m_data.depth;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;

		m_resourceFormat = m_data.textureFormat;
		
		if(m_data.filename.size())
		{ // Default to texture2d if it's to be loaded by a filename.
			m_data.textureType = TTP_TEXTURE2D;
		}

		// Setup descriptions' target dimensions.
		switch(m_data.textureType)
		{
			case TTP_TEXTURE1D:
				textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
				if(m_data.depth > 1) m_resourceDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
				else m_resourceDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
				break;
			case TTP_TEXTURE2D:
			case TTP_RTV:
			case TTP_DSV:
				textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				if(m_data.depth > 1) m_resourceDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				else m_resourceDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				break;
			case TTP_TEXTURECUBE:
				textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				if(m_data.depth > 1) m_resourceDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
				else m_resourceDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				break;
			case TTP_TEXTURE3D:
				textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
				m_resourceDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
				break;
			default:
				break;
		}
		
		if(m_data.filename.size())
		{
			std::unique_ptr<u8[]> ddsData;
			std::vector<D3D12_SUBRESOURCE_DATA> subResDataVec;

			SAFE_RELEASE(m_pTexture);
			ASSERT_HR_R(DirectX::LoadDDSTextureFromFile(
				m_pDX12Graphics->GetDevice(),
				m_data.filename.c_str(),
				&m_pTexture,
				ddsData,
				subResDataVec,
				0,
				nullptr,
				&m_data.bCube
			));

			NAME_D3D12_OBJECT(m_pTexture);

			{ // Extract loaded texture description data.
				D3D12_RESOURCE_DESC desc = m_pTexture->GetDesc();

				m_data.width = u32(desc.Width);
				m_data.height = u32(desc.Height);
				
				if(m_data.bCube)
				{
					m_data.depth = u32(desc.DepthOrArraySize) / 6;
					if(m_data.depth > 1) m_resourceDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
					else m_resourceDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				}
				else
				{
					m_data.depth = u32(desc.DepthOrArraySize);
					
					if(desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE1D) {
						if(m_data.depth > 1) m_resourceDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
						else m_resourceDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
					} else if(desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D) {
						if(m_data.depth > 1) m_resourceDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
						else m_resourceDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					}
				}
				
				m_resourceFormat = m_data.textureFormat = ConvertDXGIFormatToGFX(desc.Format);
			}

			// Describe the texture heap properties.
			CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

			// Describe the texture upload resource.
			CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(m_pTexture, 0, static_cast<u32>(subResDataVec.size())));
		
			// Create the texture's GPU upload buffer.
			ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_pTextureUpload)
			));
			
			NAME_D3D12_OBJECT(m_pTextureUpload);

			UpdateSubresources(m_pDX12Graphics->GetAssetCommandList(), m_pTexture, m_pTextureUpload, 0, 0,
				static_cast<u32>(subResDataVec.size()), subResDataVec.data());
		}
		else if(m_data.pTextureData)
		{ // Create a texture from data.
			{ // Create the texture resource.
				CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
				ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&textureDesc,
					D3D12_RESOURCE_STATE_COPY_DEST,
					nullptr,
					IID_PPV_ARGS(&m_pTexture)
				));

				NAME_D3D12_OBJECT(m_pTexture);
			}

			{ // Create the upload buffer.
				CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
				CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(m_pTexture, 0, 1));
				ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
					&heapProps,
					D3D12_HEAP_FLAG_NONE,
					&resDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_pTextureUpload)
				));
			}

			D3D12_SUBRESOURCE_DATA textureData { };
			textureData.pData = m_data.pTextureData;
			textureData.RowPitch = m_data.width * m_data.stride;
			textureData.SlicePitch = textureData.RowPitch * m_data.height;

			UpdateSubresources(m_pDX12Graphics->GetAssetCommandList(), m_pTexture, m_pTextureUpload, 0, 0, 1, &textureData);

		}
		else if(m_data.textureType == TTP_RTV)
		{ // Dataless texture. Check if its a render target view.
			textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			m_resourceStateFrom = D3D12_RESOURCE_STATE_RENDER_TARGET;
			m_resourceStateTo = D3D12_RESOURCE_STATE_GENERIC_READ;

			const Math::Color color = m_data.pRenderTexture->GetClearColor(m_data.index);
			D3D12_CLEAR_VALUE clearValue = { };
			clearValue.Format = textureDesc.Format;
			memcpy(clearValue.Color, color, sizeof(clearValue.Color));

			// Create committed texture resource.
			CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
			ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&textureDesc,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				&clearValue,
				IID_PPV_ARGS(&m_pTexture)
			));

			NAME_D3D12_OBJECT(m_pTexture);
			m_pDX12Graphics->GetHeapManager().IncRTVCounter();

		}
		else if(m_data.textureType == TTP_DSV)
		{ // Dataless texture. Check if its a depth stencil view.
			textureDesc.Format = ConvertGFXFormatToDXGI(ConvertGFXDepthStencilFormatToTex(m_data.textureFormat));
			m_resourceFormat = ConvertGFXDepthStencilFormatToSRV(m_data.textureFormat);

			textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			m_resourceStateFrom = D3D12_RESOURCE_STATE_DEPTH_WRITE;
			m_resourceStateTo = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

			const DepthStencil depthStencil = m_data.pRenderTexture->GetClearDepthStencil();
			D3D12_CLEAR_VALUE clearValue = { };
			clearValue.Format = ConvertGFXFormatToDXGI(m_data.textureFormat);
			clearValue.DepthStencil.Depth = depthStencil.depth;
			clearValue.DepthStencil.Stencil = depthStencil.stencil;

			// Create committed texture resource.
			CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
			ASSERT_HR_R(m_pDX12Graphics->GetDevice()->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&textureDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&clearValue,
				IID_PPV_ARGS(&m_pTexture)
			));

			NAME_D3D12_OBJECT(m_pTexture);
			m_pDX12Graphics->GetHeapManager().IncDSVCounter();
		}
		
		CD3DX12_RESOURCE_BARRIER resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pTexture, m_resourceStateFrom, m_resourceStateTo);
		m_pDX12Graphics->GetAssetCommandList()->ResourceBarrier(1, &resBarrier);
		m_resourceState = m_resourceStateTo;

		m_pDX12Graphics->GetHeapManager().IncCSUCounter();
	}

	void CDX12Texture::PostInitialize()
	{
		SAFE_RELEASE(m_pTextureUpload);

		// Prep heap for this object when its use is specified.
		if(!m_bHeapIndexed)
		{
			m_csuIndex = m_pDX12Graphics->GetHeapManager().GetNextCSUIndex();

			if(m_data.textureType == TTP_RTV)
			{
				m_rtvIndex = m_pDX12Graphics->GetHeapManager().GetNextRTVIndex();
			}
			else if(m_data.textureType == TTP_DSV)
			{
				m_dsvIndex = m_pDX12Graphics->GetHeapManager().GetNextDSVIndex();
			}

			m_bHeapIndexed = true;
		}

		D3D12_RESOURCE_DESC desc = m_pTexture->GetDesc();

		switch(m_data.textureType)
		{
			case TTP_RTV:
			{ // Describe and create a RTV.
				D3D12_RENDER_TARGET_VIEW_DESC rtvDesc { };
				rtvDesc.Format = ConvertGFXFormatToDXGI(m_data.textureFormat);
				rtvDesc.ViewDimension = ConvertSRVToRTVDimensions(m_resourceDimension);

				switch(rtvDesc.ViewDimension)
				{
					case D3D12_RTV_DIMENSION_UNKNOWN:
						break;
					case D3D12_RTV_DIMENSION_BUFFER:
						rtvDesc.Buffer.FirstElement = 0;
						rtvDesc.Buffer.NumElements = 1;
						break;
					case D3D12_RTV_DIMENSION_TEXTURE1D:
						rtvDesc.Texture1D.MipSlice = 0;
						break;
					case D3D12_RTV_DIMENSION_TEXTURE1DARRAY:
						rtvDesc.Texture1DArray.FirstArraySlice = 0;
						rtvDesc.Texture1DArray.ArraySize = m_data.depth;
						rtvDesc.Texture1DArray.MipSlice = 0;
						break;
					case D3D12_RTV_DIMENSION_TEXTURE2D:
						rtvDesc.Texture2D.MipSlice = 0;
						rtvDesc.Texture2D.PlaneSlice = 0;
						break;
					case D3D12_RTV_DIMENSION_TEXTURE2DARRAY:
						rtvDesc.Texture2DArray.FirstArraySlice = 0;
						rtvDesc.Texture2DArray.ArraySize = m_data.depth;
						rtvDesc.Texture2DArray.MipSlice = 0;
						rtvDesc.Texture2DArray.PlaneSlice = 0;
						break;
					case D3D12_RTV_DIMENSION_TEXTURE2DMS:
						break;
					case D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY:
						rtvDesc.Texture2DMSArray.FirstArraySlice = 0;
						rtvDesc.Texture2DMSArray.ArraySize = m_data.depth;
						break;
					case D3D12_RTV_DIMENSION_TEXTURE3D:
						rtvDesc.Texture3D.FirstWSlice = 0;
						rtvDesc.Texture3D.MipSlice = 0;
						rtvDesc.Texture3D.WSize = m_data.depth;
						break;
					default:
						break;
				}

				m_pDX12Graphics->GetDevice()->CreateRenderTargetView(m_pTexture, &rtvDesc, m_pDX12Graphics->GetHeapManager().GetRTVHandleCPU(m_rtvIndex));
			} break;
			case TTP_DSV:
			{ // Describe and create a DSV.
				D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc { };
				dsvDesc.Format = ConvertGFXFormatToDXGI(m_data.textureFormat);
				dsvDesc.ViewDimension = ConvertSRVToDSVDimensions(m_resourceDimension);
				dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

				m_pDX12Graphics->GetDevice()->CreateDepthStencilView(m_pTexture, &dsvDesc, m_pDX12Graphics->GetHeapManager().GetDSVHandleCPU(m_dsvIndex));
			} break;
			default:
				break;
		}

		// Describe and create a SRV.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc { };
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = ConvertGFXFormatToDXGI(m_resourceFormat);
		srvDesc.ViewDimension = m_resourceDimension;

		switch(srvDesc.ViewDimension)
		{
			case D3D12_SRV_DIMENSION_TEXTURE1D:
				srvDesc.Texture1D.MipLevels = desc.MipLevels;
				break;
			case D3D12_SRV_DIMENSION_TEXTURE1DARRAY:
				srvDesc.Texture1DArray.MipLevels = desc.MipLevels;
				srvDesc.Texture1DArray.ArraySize = m_data.depth;
				srvDesc.Texture1DArray.FirstArraySlice = 0;
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2D:
				srvDesc.Texture2D.MipLevels = desc.MipLevels;
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2DARRAY:
				srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
				srvDesc.Texture2DArray.ArraySize = m_data.depth;
				srvDesc.Texture2DArray.FirstArraySlice = 0;
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2DMS:
				break;
			case D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY:
				srvDesc.Texture2DMSArray.ArraySize = m_data.depth;
				srvDesc.Texture2DMSArray.FirstArraySlice = 0;
				break;
			case D3D12_SRV_DIMENSION_TEXTURE3D:
				srvDesc.Texture3D.MipLevels = desc.MipLevels;
				break;
			case D3D12_SRV_DIMENSION_TEXTURECUBE:
				srvDesc.TextureCube.MipLevels = desc.MipLevels;
				break;
			case D3D12_SRV_DIMENSION_TEXTURECUBEARRAY:
				srvDesc.TextureCubeArray.MipLevels = desc.MipLevels;
				srvDesc.TextureCubeArray.NumCubes = m_data.depth;
				srvDesc.TextureCubeArray.First2DArrayFace = 0;
				break;
			default:
				break;
		}

		m_pDX12Graphics->GetDevice()->CreateShaderResourceView(m_pTexture, &srvDesc, m_pDX12Graphics->GetHeapManager().GetCSUHandleCPU(m_csuIndex));
	}

	void CDX12Texture::Bind(u32& index, bool bCompute)
	{
		if(bCompute)
		{
			m_pDX12Graphics->GetComputeCommandList()->SetComputeRootDescriptorTable(index++, m_pDX12Graphics->GetHeapManager().GetCSUHandleGPU(m_csuIndex));
		}
		else
		{
			m_pDX12Graphics->GetRealtimeCommandList()->SetGraphicsRootDescriptorTable(index++, m_pDX12Graphics->GetHeapManager().GetCSUHandleGPU(m_csuIndex));
		}
	}

	void CDX12Texture::Release()
	{
		SAFE_RELEASE(m_pTextureUpload);
		SAFE_RELEASE(m_pTexture);
	}

	//-----------------------------------------------------------------------------------------------
	// Utility method.
	//-----------------------------------------------------------------------------------------------

	void* CDX12Texture::GetCPUHandle(DESC_HEAP_TYPE tp)
	{
		switch(tp)
		{
			case DHT_RTV:
				return reinterpret_cast<void*>(m_pDX12Graphics->GetHeapManager().GetRTVHandleCPU(m_rtvIndex).ptr);
			case DHT_DSV:
				return reinterpret_cast<void*>(m_pDX12Graphics->GetHeapManager().GetDSVHandleCPU(m_dsvIndex).ptr);
			case DHT_CSU:
			default:
				return reinterpret_cast<void*>(m_pDX12Graphics->GetHeapManager().GetCSUHandleCPU(m_csuIndex).ptr);
		}
	}

	// Method for transition the texture resource to transition state.
	void CDX12Texture::TransitionToShader()
	{
		CD3DX12_RESOURCE_BARRIER resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pTexture, m_resourceStateFrom, m_resourceStateTo);
		m_pDX12Graphics->GetRealtimeCommandList()->ResourceBarrier(1, &resBarrier);
		m_resourceState = m_resourceStateTo;
	}

	// Method for transition the texture resource from transition state.
	void CDX12Texture::TransitionFromShader()
	{
		CD3DX12_RESOURCE_BARRIER resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_pTexture, m_resourceStateTo, m_resourceStateFrom);
		m_pDX12Graphics->GetRealtimeCommandList()->ResourceBarrier(1, &resBarrier);
		m_resourceState = m_resourceStateFrom;
	}

	void CDX12Texture::SaveAsTexture(const wchar_t* filename)
	{
		ASSERT_HR_R(DirectX::SaveDDSTextureToFile(m_pDX12Graphics->GetCommandQueue(), m_pTexture, filename, m_resourceState, m_resourceState));
	}
};

#endif
