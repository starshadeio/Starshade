//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CBlockMarker.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CBlockMarker.h"
#include <Application/CSceneManager.h>
#include <Graphics/CMeshRenderer_.h>
#include <Graphics/CMaterial.h>
#include <Graphics/CGraphicsAPI.h>
#include <Factory/CFactory.h>
#include <Resources/CResourceManager.h>
#include <Math/CMathVector2.h>
#include <Utilities/CMemoryFree.h>

namespace App
{
	CBlockMarker::CBlockMarker(const wchar_t* pName, u32 viewHash) : 
		CVObject(pName, viewHash),
		m_transform(this),
		m_meshData(this),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr)
	{
	}

	CBlockMarker::~CBlockMarker()
	{
	}
	
	void CBlockMarker::Initialize()
	{
		{ // Create the mesh data.
			struct Vertex
			{
				Math::Vector3 position;
				Math::Vector2 texCoord;
			};

			typedef u16 Index;

			Graphics::CMeshData_::Data data { };
			data.topology = Graphics::PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.vertexCount = 24;
			data.indexCount = 36;
			data.vertexStride = sizeof(Vertex);
			data.indexStride = sizeof(Index);

			m_meshData.SetData(data);
			m_meshData.Initialize();

			u16 vertex = 0;
			u16 index = 0;

			auto AddQuad = [&](const Math::Vector3& offset, const Math::Vector3& normal, const Math::Vector3& tangent, const Math::Vector3& bitangent){
				*(Index*)m_meshData.GetIndexAt(index++) = vertex;
				*(Index*)m_meshData.GetIndexAt(index++) = vertex + 1;
				*(Index*)m_meshData.GetIndexAt(index++) = vertex + 2;
				*(Index*)m_meshData.GetIndexAt(index++) = vertex;
				*(Index*)m_meshData.GetIndexAt(index++) = vertex + 2;
				*(Index*)m_meshData.GetIndexAt(index++) = vertex + 3;

				*(Vertex*)m_meshData.GetVertexAt(vertex++) = { { offset + (normal - tangent - bitangent) * 0.5f }, { 0.0f, 0.0f } };
				*(Vertex*)m_meshData.GetVertexAt(vertex++) = { { offset + (normal + tangent - bitangent) * 0.5f }, { 1.0f, 0.0f } };
				*(Vertex*)m_meshData.GetVertexAt(vertex++) = { { offset + (normal + tangent + bitangent) * 0.5f }, { 1.0f, 1.0f } };
				*(Vertex*)m_meshData.GetVertexAt(vertex++) = { { offset + (normal - tangent + bitangent) * 0.5f }, { 0.0f, 1.0f } };
			};

			AddQuad(0.0f, Math::VEC3_RIGHT, Math::VEC3_UP, Math::VEC3_BACKWARD);
			AddQuad(0.0f, Math::VEC3_LEFT, Math::VEC3_UP, Math::VEC3_FORWARD);

			AddQuad(0.0f, Math::VEC3_DOWN, Math::VEC3_RIGHT, Math::VEC3_BACKWARD);
			AddQuad(0.0f, Math::VEC3_UP, Math::VEC3_RIGHT, Math::VEC3_FORWARD);

			AddQuad(0.0f, Math::VEC3_FORWARD, Math::VEC3_UP, Math::VEC3_RIGHT);
			AddQuad(0.0f, Math::VEC3_BACKWARD, Math::VEC3_UP, Math::VEC3_LEFT);
		}

		{ // Create the mesh renderer.
			m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matHash));

			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(this);

			Graphics::CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CBlockMarker::PreRender, this);
			data.pMeshData = &m_meshData;
			m_pMeshRenderer->SetData(data);
			m_pMeshRenderer->AddMaterial(m_pMaterial);

			m_pMeshRenderer->Initialize();
		}
	}
	
	void CBlockMarker::PreRender()
	{
		static const u32 matrixBufferHash = Math::FNV1a_32("MatrixBuffer");
		static const u32 worldHash = Math::FNV1a_32("World");
		static const u32 viewHash = Math::FNV1a_32("View");
		static const u32 projHash = Math::FNV1a_32("Proj");

		m_pMaterial->SetFloat(matrixBufferHash, worldHash, m_transform.GetWorldMatrix().f32, 16);
		m_pMaterial->SetFloat(matrixBufferHash, viewHash, CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrixInv().f32, 16);
		m_pMaterial->SetFloat(matrixBufferHash, projHash, CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix().f32, 16);
	}
	
	void CBlockMarker::Release()
	{
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}

	void CBlockMarker::SetActive(bool bActive)
	{
		m_pMeshRenderer->SetActive(bActive);
	}
};
