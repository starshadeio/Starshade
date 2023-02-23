//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPrimTriangle.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPrimTriangle.h"
#include "CMeshRenderer_.h"
#include "CCompute.h"
#include "CMaterial.h"
#include "CUAVBuffer.h"
#include "CTexture.h"
#include "CGraphicsAPI.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include <Utilities/CMemoryFree.h>
#include <Math/CMathFNV.h>
#include <Math/CMathVector2.h>

namespace Graphics
{
	CPrimTriangle::CPrimTriangle(const wchar_t* pName, u32 viewHash) :
		CVObject(pName, viewHash),
		m_uavBufferCount(0),
		m_meshData(this),
		m_pMeshRenderer(nullptr),
		m_pCompute(nullptr),
		m_pMaterial(nullptr),
		m_pComputeMaterial(nullptr),
		m_pUAVBufferList(nullptr),
		m_pTexture(nullptr),
		m_pGraphicsAPI(nullptr) {
	}

	CPrimTriangle::~CPrimTriangle() { }

	void CPrimTriangle::Initialize()
	{
		m_pGraphicsAPI = CFactory::Instance().GetGraphicsAPI();

		{ // Create the mesh data.
			struct Vertex
			{
				Math::Vector3 position;
				Math::Vector2 color;
			};

			CMeshData_::Data data { };
			data.topology = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.vertexCount = 3;
			data.vertexStride = sizeof(Vertex);

			m_meshData.SetData(data);
			m_meshData.Initialize();

			*(Vertex*)m_meshData.GetVertexAt(0) = { { 0.0f, 0.25f, 0.0f }, { 0.5f, 0.0f } };
			*(Vertex*)m_meshData.GetVertexAt(1) = { { 0.25f, -0.25f, 0.0f }, { 1.0f, 1.0f } };
			*(Vertex*)m_meshData.GetVertexAt(2) = { { -0.25f, -0.25f, 0.0f }, { 0.0f, 1.0f } };
		}

		{ // Create the mesh renderer.
			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(this);

			CMeshRenderer_::Data data { };
			data.pMaterial = m_pMaterial = reinterpret_cast<class CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64("MATERIAL_TRIANGLE")));
			data.pMeshData = &m_meshData;
			m_pMeshRenderer->SetData(data);

			m_pMeshRenderer->Initialize();
		}

		{ // Create the compute.
			m_pCompute = CFactory::Instance().CreateCompute(this);

			CCompute::Data data { };
			data.threadX = 1;
			data.threadY = 1;
			data.threadZ = 1;
			data.pMaterial = m_pComputeMaterial = reinterpret_cast<class CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64("MATERIAL_COMPUTE")));
			m_pCompute->SetData(data);

			m_pCompute->Initialize();
		}

		{ // Create the UAV buffer.
			m_uavBufferCount = m_pGraphicsAPI->GetBufferCount();
			m_pUAVBufferList = new CUAVBuffer * [m_uavBufferCount];

			for(u8 i = 0; i < m_uavBufferCount; ++i)
			{
				m_pUAVBufferList[i] = CFactory::Instance().CreateUAVBuffer();
				CUAVBuffer::Data data { };
				Math::Vector4 offset(0.5f, 0.0f, 0.0f, 0.0f);
				data.bufferStride = sizeof(Math::Vector4);
				data.bufferSize = data.bufferStride;
				data.pBufferData = reinterpret_cast<u8*>(&offset);
				m_pUAVBufferList[i]->SetData(data);
				m_pUAVBufferList[i]->Initialize();
			}
		}

		{ // Create procedural texture.
			m_pTexture = CFactory::Instance().CreateTexture();

			CTexture::Data data { };
			data.width = 256;
			data.height = 256;
			data.depth = 1;
			data.textureFormat = GFX_FORMAT_R8G8B8A8_UNORM;
			data.textureType = CTexture::TTP_TEXTURE2D;
			data.stride = 4;

			// Create and copy data to the intermediate upload heap, and then schedule a copy from the upload heap to the Texture2D.
			auto GenerateTextureData = [](u32 width, u32 height, u32 pixelSize, std::vector<u8>& texture) {
				const u32 rowPitch = width * pixelSize;
				const u32 cellPitch = rowPitch >> 3;
				const u32 cellHeight = width >> 3;
				const u32 textureSize = rowPitch * height;

				texture.resize(textureSize);
				u8* pData = &texture[0];

				for(u32 n = 0; n < textureSize; n += pixelSize)
				{
					const u32 x = n % rowPitch;
					const u32 y = n / rowPitch;
					const u32 i = x / cellPitch;
					const u32 j = y / cellHeight;

					if((i & 1) == (j & 1))
					{
						pData[n] = 0x00;
						pData[n + 1] = 0x00;
						pData[n + 2] = 0x00;
						pData[n + 3] = 0xFF;
					}
					else
					{
						pData[n] = 0xFF;
						pData[n + 1] = 0xFF;
						pData[n + 2] = 0xFF;
						pData[n + 3] = 0xFF;
					}
				}
			};

			std::vector<u8> texture;
			GenerateTextureData(data.width, data.height, data.stride, texture);

			data.pTextureData = texture.data();
			m_pTexture->SetData(data);

			m_pTexture->Initialize();
		}
	}

	void CPrimTriangle::PostInitialize()
	{
		for(u8 i = 0; i < m_uavBufferCount; ++i)
		{
			m_pUAVBufferList[i]->PostInitialize();
		}

		m_pTexture->PostInitialize();
		m_pMaterial->SetTexture(Math::FNV1a_32("g_texture"), m_pTexture);
	}

	void CPrimTriangle::Update()
	{
		static float y = 0.0f;
		y += 0.01f;
		static Math::Vector4 v = Math::VEC4_ZERO;
		v.y = sinf(y) * 0.0f;

		m_pMaterial->SetFloat(Math::FNV1a_32("DataBuffer"), Math::FNV1a_32("Offset"), v.v, 4);

		m_pMaterial->SetUAV(Math::FNV1a_32("g_uav"), m_pUAVBufferList[m_pGraphicsAPI->GetFrameIndex()]);

		m_pComputeMaterial->SetUAV(Math::FNV1a_32("g_uav_in"), m_pUAVBufferList[(m_pGraphicsAPI->GetFrameIndex() + (m_uavBufferCount - 1)) % m_uavBufferCount]);
		m_pComputeMaterial->SetUAV(Math::FNV1a_32("g_uav_out"), m_pUAVBufferList[m_pGraphicsAPI->GetFrameIndex()]);
	}

	void CPrimTriangle::Release()
	{
		SAFE_RELEASE_DELETE(m_pTexture);
		for(u8 i = 0; i < m_uavBufferCount; ++i)
		{
			SAFE_RELEASE_DELETE(m_pUAVBufferList[i]);
		} SAFE_DELETE_ARRAY(m_pUAVBufferList);
		SAFE_RELEASE_DELETE(m_pCompute);
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}
};
