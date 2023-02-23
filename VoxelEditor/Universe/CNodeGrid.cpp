//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CNodeGrid.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CNodeGrid.h"
#include "../Application/CEditor.h"
#include <Graphics/CMeshRenderer_.h>
#include <Graphics/CMaterial.h>
#include <Factory/CFactory.h>
#include <Resources/CResourceManager.h>
#include <Application/CSceneManager.h>
#include <Utilities/CMemoryFree.h>
#include <Math/CMathFNV.h>

namespace Universe
{
	CNodeGrid::CNodeGrid(const wchar_t* pName, u32 viewHash) :
		CVObject(pName, viewHash),
		m_transform(this),
		m_meshData(this),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr) {
	}

	CNodeGrid::~CNodeGrid() { }

	void CNodeGrid::Initialize()
	{
		{ // Create the mesh data.
			struct Vertex
			{
				Math::Vector3 position;
				Math::Vector3 normal;
			};

			Graphics::CMeshData_::Data data { };
			data.topology = Graphics::PRIMITIVE_TOPOLOGY_LINELIST;
			data.vertexCount = ((m_data.width + m_data.length) << 1) * 6;
			data.vertexStride = sizeof(Vertex);

			m_meshData.SetData(data);
			m_meshData.Initialize();

			const float wh = m_data.cellSize.x * (m_data.width - 1) * 0.5f;
			const float hh = m_data.cellSize.y * (m_data.height - 1) * 0.5f;
			const float lh = m_data.cellSize.z * (m_data.length - 1) * 0.5f;
			u32 index = 0;

			auto buildGrid = [&](const Math::Vector3& offset, const Math::Vector3& right, float halfW, const Math::Vector3& up, float halfH, Math::Vector3 normal){
				for(u32 i = 0; i < m_data.width; ++i)
				{
					*(Vertex*)m_meshData.GetVertexAt(index++) = { offset + right * (-halfW + m_data.cellSize.x * i) + up * -halfH, normal };
					*(Vertex*)m_meshData.GetVertexAt(index++) = { offset + right * (-halfW + m_data.cellSize.x * i) + up * halfH, normal };
				}

				for(u32 i = 0; i < m_data.length; ++i)
				{
					*(Vertex*)m_meshData.GetVertexAt(index++) = { offset + right * -halfW + up * (-halfH + m_data.cellSize.y * i), normal };
					*(Vertex*)m_meshData.GetVertexAt(index++) = { offset + right * halfW + up * (-halfH + m_data.cellSize.y * i), normal };
				}
			};
			
			buildGrid(Math::Vector3(-wh, 0.0f, 0.0f), m_data.forward, lh, m_data.up, hh, Math::VEC3_RIGHT);
			buildGrid(Math::Vector3( wh, 0.0f, 0.0f), m_data.forward, lh, m_data.up, hh, Math::VEC3_LEFT);
			buildGrid(Math::Vector3(0.0f, -hh, 0.0f), m_data.right, wh, m_data.forward, lh, Math::VEC3_UP);
			buildGrid(Math::Vector3(0.0f,  hh, 0.0f), m_data.right, wh, m_data.forward, lh, Math::VEC3_DOWN);
			buildGrid(Math::Vector3(0.0f, 0.0f, -lh), m_data.right, wh, m_data.up, hh, Math::VEC3_FORWARD);
			buildGrid(Math::Vector3(0.0f, 0.0f,  lh), m_data.right, wh, m_data.up, hh, Math::VEC3_BACKWARD);
		}

		{ // Create the mesh renderer.
			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(this);

			Graphics::CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CNodeGrid::PreRender, this);
			data.pMaterial = m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64("MATERIAL_NODE_GRID")));
			data.pMeshData = &m_meshData;
			m_pMeshRenderer->SetData(data);

			m_pMeshRenderer->Initialize();
		}
	}

	void CNodeGrid::LateUpdate()
	{
		m_pMeshRenderer->SetActive(m_bActive);
		if(m_bActive)
		{
			static const u32 frameBufferHash = Math::FNV1a_32("DrawBuffer");
			static const u32 vpHash = Math::FNV1a_32("VP");
			static const u32 cameraPositionHash = Math::FNV1a_32("CameraPosition");

			Math::SIMDMatrix vp = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix();
			vp *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix();
			Math::SIMDVector camPos = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition();
		
			m_pMaterial->SetFloat(frameBufferHash, vpHash, vp.f32, 16);
			m_pMaterial->SetFloat(frameBufferHash, cameraPositionHash, camPos.ToFloat(), 4);
		}
	}
	
	void CNodeGrid::PreRender()
	{
		static const u32 maxtrixBufferHash = Math::FNV1a_32("DrawBuffer");
		static const u32 worldHash = Math::FNV1a_32("World");

		Math::SIMDMatrix world = m_transform.GetWorldMatrix();
		m_pMaterial->SetFloat(maxtrixBufferHash, worldHash, world.f32, 16);
	}

	void CNodeGrid::Release()
	{
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}
};
