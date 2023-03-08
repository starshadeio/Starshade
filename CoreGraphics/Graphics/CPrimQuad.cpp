//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPrimQuad.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPrimQuad.h"
#include "CMeshRenderer_.h"
#include "CMaterial.h"
#include "CTexture.h"
#include "CGraphicsAPI.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include "../Application/CSceneManager.h"
#include <Utilities/CMemoryFree.h>
#include <Math/CMathFNV.h>

namespace Graphics
{
	CPrimQuad::CPrimQuad(const wchar_t* pName, u32 viewHash) :
		CVObject(pName, viewHash),
		m_transform(this),
		m_meshData(this),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr),
		m_pTexture(nullptr)
	{
	}

	CPrimQuad::~CPrimQuad()
	{
	}
	
	void CPrimQuad::Initialize()
	{
		{ // Create the mesh data.
			struct Vertex
			{
				Math::Vector3 position;
				Math::Vector2 texCoord;
			};

			typedef u16 Index;

			CMeshData_::Data data { };
			data.topology = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.vertexCount = 4;
			data.indexCount = 6;
			data.vertexStride = sizeof(Vertex);
			data.indexStride = sizeof(Index);

			m_meshData.SetData(data);
			m_meshData.Initialize();
			
			*(Vertex*)m_meshData.GetVertexAt(0) = { { m_data.right * -m_data.halfSize.x + m_data.up * -m_data.halfSize.y }, { 0.0f, 0.0f } };
			*(Vertex*)m_meshData.GetVertexAt(1) = { { m_data.right *  m_data.halfSize.x + m_data.up * -m_data.halfSize.y }, { 1.0f, 0.0f } };
			*(Vertex*)m_meshData.GetVertexAt(2) = { { m_data.right *  m_data.halfSize.x + m_data.up *  m_data.halfSize.y }, { 1.0f, 1.0f } };
			*(Vertex*)m_meshData.GetVertexAt(3) = { { m_data.right * -m_data.halfSize.x + m_data.up *  m_data.halfSize.y }, { 0.0f, 1.0f } };

			*(Index*)m_meshData.GetIndexAt(0) = 0;
			*(Index*)m_meshData.GetIndexAt(1) = 1;
			*(Index*)m_meshData.GetIndexAt(2) = 2;
			*(Index*)m_meshData.GetIndexAt(3) = 0;
			*(Index*)m_meshData.GetIndexAt(4) = 2;
			*(Index*)m_meshData.GetIndexAt(5) = 3;
		}

		{ // Create the mesh renderer.
			m_pMaterial = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matHash));

			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(this);

			CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CPrimQuad::PreRender, this);
			data.pMeshData = &m_meshData;
			m_pMeshRenderer->SetData(data);
			m_pMeshRenderer->AddMaterial(m_pMaterial);

			m_pMeshRenderer->Initialize();
		}
	}
	
	void CPrimQuad::PreRender()
	{
		static const u32 matrixBufferHash = Math::FNV1a_32("MatrixBuffer");
		static const u32 wvHash = Math::FNV1a_32("WV");
		static const u32 projHash = Math::FNV1a_32("Proj");

		Math::SIMDMatrix mtx = Math::SIMD_MTX4X4_IDENTITY;
		mtx *= m_transform.GetWorldMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix();
		Math::SIMDMatrix proj = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix();

		m_pMaterial->SetFloat(matrixBufferHash, wvHash, mtx.f32, 16);
		m_pMaterial->SetFloat(matrixBufferHash, projHash, proj.f32, 16);
	}
	
	void CPrimQuad::Release()
	{
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}

	void CPrimQuad::SetActive(bool bActive)
	{
		m_pMeshRenderer->SetActive(bActive);
	}
};
