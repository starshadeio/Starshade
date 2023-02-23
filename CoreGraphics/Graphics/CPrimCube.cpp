//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPrimCube.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPrimCube.h"
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
	CPrimCube::CPrimCube(const wchar_t* pName, u32 viewHash) :
		CVObject(pName, viewHash),
		m_transform(this),
		m_meshData(this),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr),
		m_pTexture(nullptr)
	{
	}

	CPrimCube::~CPrimCube()
	{
	}
	
	void CPrimCube::Initialize()
	{
		{ // Create the mesh data.
			struct Vertex
			{
				Math::Vector3 position;
				Math::Vector3 normal;
			};

			struct VertexTex
			{
				Math::Vector3 position;
				Math::Vector3 normal;
				Math::Vector2 texCoord;
			};

			typedef u16 Index;

			CMeshData_::Data data { };
			data.topology = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.vertexCount = 24;
			data.indexCount = 36;
			data.vertexStride = m_data.bTextured ? sizeof(VertexTex) : sizeof(Vertex);
			data.indexStride = sizeof(Index);

			m_meshData.SetData(data);
			m_meshData.Initialize();

			const Math::Vector3 r = Math::VEC3_RIGHT * m_data.halfSize.x;
			const Math::Vector3 u = Math::VEC3_UP * m_data.halfSize.y;
			const Math::Vector3 f = Math::VEC3_FORWARD * m_data.halfSize.z;
			
			u32 index = 0;
			u32 vertex = 0;
			
			auto AddIndices = [&](){
				*(Index*)m_meshData.GetIndexAt(index++) = vertex;
				*(Index*)m_meshData.GetIndexAt(index++) = vertex + 1;
				*(Index*)m_meshData.GetIndexAt(index++) = vertex + 3;
				*(Index*)m_meshData.GetIndexAt(index++) = vertex + 1;
				*(Index*)m_meshData.GetIndexAt(index++) = vertex + 2;
				*(Index*)m_meshData.GetIndexAt(index++) = vertex + 3;
			};

			auto AddVertex = [&](const Math::Vector3& position, const Math::Vector3& normal, const Math::Vector2& texCoord){
				if(m_data.bTextured)
				{
					*(VertexTex*)m_meshData.GetVertexAt(vertex++) = { position, normal, texCoord };
				}
				else
				{
					*(Vertex*)m_meshData.GetVertexAt(vertex++) = { position, normal };
				}
			};

			AddIndices();
			AddVertex((-r + f - u), Math::VEC3_LEFT, Math::Vector2(0.0f, 0.0f));
			AddVertex((-r - f - u), Math::VEC3_LEFT, Math::Vector2(1.0f, 0.0f));
			AddVertex((-r - f + u), Math::VEC3_LEFT, Math::Vector2(1.0f, 1.0f));
			AddVertex((-r + f + u), Math::VEC3_LEFT, Math::Vector2(0.0f, 1.0f));
			
			AddIndices();
			AddVertex(( r - f - u), Math::VEC3_RIGHT, Math::Vector2(0.0f, 0.0f));
			AddVertex(( r + f - u), Math::VEC3_RIGHT, Math::Vector2(1.0f, 0.0f));
			AddVertex(( r + f + u), Math::VEC3_RIGHT, Math::Vector2(1.0f, 1.0f));
			AddVertex(( r - f + u), Math::VEC3_RIGHT, Math::Vector2(0.0f, 1.0f));
			
			AddIndices();
			AddVertex((-u - r + f), Math::VEC3_DOWN, Math::Vector2(0.0f, 0.0f));
			AddVertex((-u + r + f), Math::VEC3_DOWN, Math::Vector2(1.0f, 0.0f));
			AddVertex((-u + r - f), Math::VEC3_DOWN, Math::Vector2(1.0f, 1.0f));
			AddVertex((-u - r - f), Math::VEC3_DOWN, Math::Vector2(0.0f, 1.0f));
			
			AddIndices();
			AddVertex(( u - r - f), Math::VEC3_UP, Math::Vector2(0.0f, 0.0f));
			AddVertex(( u + r - f), Math::VEC3_UP, Math::Vector2(1.0f, 0.0f));
			AddVertex(( u + r + f), Math::VEC3_UP, Math::Vector2(1.0f, 1.0f));
			AddVertex(( u - r + f), Math::VEC3_UP, Math::Vector2(0.0f, 1.0f));
			
			AddIndices();
			AddVertex((-f - r - u), Math::VEC3_BACKWARD, Math::Vector2(0.0f, 0.0f));
			AddVertex((-f + r - u), Math::VEC3_BACKWARD, Math::Vector2(1.0f, 0.0f));
			AddVertex((-f + r + u), Math::VEC3_BACKWARD, Math::Vector2(1.0f, 1.0f));
			AddVertex((-f - r + u), Math::VEC3_BACKWARD, Math::Vector2(0.0f, 1.0f));
			
			AddIndices();
			AddVertex(( f + r - u), Math::VEC3_FORWARD, Math::Vector2(0.0f, 0.0f));
			AddVertex(( f - r - u), Math::VEC3_FORWARD, Math::Vector2(1.0f, 0.0f));
			AddVertex(( f - r + u), Math::VEC3_FORWARD, Math::Vector2(1.0f, 1.0f));
			AddVertex(( f + r + u), Math::VEC3_FORWARD, Math::Vector2(0.0f, 1.0f));
		}

		{ // Create the mesh renderer.
			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(this);

			CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CPrimCube::PreRender, this);
			data.pMaterial = m_pMaterial = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matHash));
			data.pMeshData = &m_meshData;
			m_pMeshRenderer->SetData(data);

			m_pMeshRenderer->Initialize();
		}
	}
	
	void CPrimCube::PreRender()
	{
		static const u32 matrixBufferHash = Math::FNV1a_32("MatrixBuffer");
		static const u32 worldHash = Math::FNV1a_32("World");
		static const u32 vpHash = Math::FNV1a_32("VP");

		Math::SIMDMatrix mtx = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix();

		m_pMaterial->SetFloat(matrixBufferHash, worldHash, m_transform.GetWorldMatrix().f32, 16);
		m_pMaterial->SetFloat(matrixBufferHash, vpHash, mtx.f32, 16);
	}
	
	void CPrimCube::Release()
	{
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}

	void CPrimCube::SetActive(bool bActive)
	{
		m_pMeshRenderer->SetActive(bActive);
	}
};
