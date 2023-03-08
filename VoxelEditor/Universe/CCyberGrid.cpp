//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CCyberGrid.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CCyberGrid.h"
#include <Graphics/CMeshRenderer_.h>
#include <Graphics/CMaterial.h>
#include <Factory/CFactory.h>
#include <Resources/CResourceManager.h>
#include <Application/CSceneManager.h>
#include <Utilities/CMemoryFree.h>
#include <Math/CMathFNV.h>

namespace Universe
{
	CCyberGrid::CCyberGrid(const wchar_t* pName, u32 viewHash) :
		CVObject(pName, viewHash),
		m_transform(this),
		m_meshData(this),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr) {
	}

	CCyberGrid::~CCyberGrid() { }

	void CCyberGrid::Initialize()
	{
		{ // Create the mesh data.
			struct Vertex
			{
				Math::Vector3 position;
			};

			Graphics::CMeshData_::Data data { };
			data.topology = Graphics::PRIMITIVE_TOPOLOGY_LINELIST;
			data.vertexCount = (((m_data.width + m_data.length - 4) * (m_data.height - 2)) << 1) + (((m_data.width - 2) * (m_data.length - 2)) << 1);
			data.vertexStride = sizeof(Vertex);

			m_meshData.SetData(data);
			m_meshData.Initialize();

			const float wh = m_data.cellSize.x * (m_data.width - 1) * 0.5f;
			const float hh = m_data.cellSize.y * (m_data.height - 1) * 0.5f;
			const float lh = m_data.cellSize.z * (m_data.length - 1) * 0.5f;
			u32 index = 0;

			for(u32 i = 1; i < m_data.height - 1; ++i)
			{
				for(u32 j = 1; j < m_data.width - 1; ++j)
				{
					*(Vertex*)m_meshData.GetVertexAt(index++) = { m_data.right * (-wh + m_data.cellSize.x * j) + m_data.up * (-hh + m_data.cellSize.y * i) + m_data.forward * -lh };
					*(Vertex*)m_meshData.GetVertexAt(index++) = { m_data.right * (-wh + m_data.cellSize.x * j) + m_data.up * (-hh + m_data.cellSize.y * i) + m_data.forward * lh };
				}

				for(u32 j = 1; j < m_data.length - 1; ++j)
				{
					*(Vertex*)m_meshData.GetVertexAt(index++) = { m_data.right * -wh + m_data.up * (-hh + m_data.cellSize.y * i) + m_data.forward * (-lh + m_data.cellSize.z * j) };
					*(Vertex*)m_meshData.GetVertexAt(index++) = { m_data.right * wh + m_data.up * (-hh + m_data.cellSize.y * i) + m_data.forward * (-lh + m_data.cellSize.z * j) };
				}
			}

			for(u32 i = 1; i < m_data.length - 1; ++i)
			{
				for(u32 j = 1; j < m_data.width - 1; ++j)
				{
					*(Vertex*)m_meshData.GetVertexAt(index++) = { m_data.right * (-wh + m_data.cellSize.x * j) + m_data.up * -hh + m_data.forward * (-lh + m_data.cellSize.z * i) };
					*(Vertex*)m_meshData.GetVertexAt(index++) = { m_data.right * (-wh + m_data.cellSize.x * j) + m_data.up * hh + m_data.forward * (-lh + m_data.cellSize.z * i) };
				}
			}
		}

		{ // Create the mesh renderer.
			m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64("MATERIAL_CYBER_GRID")));

			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(this);

			Graphics::CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CCyberGrid::PreRender, this);
			data.pMeshData = &m_meshData;
			m_pMeshRenderer->SetData(data);
			m_pMeshRenderer->AddMaterial(m_pMaterial);

			m_pMeshRenderer->Initialize();
		}
	}

	void CCyberGrid::PreRender()
	{
		static const u32 maxtrixBufferHash = Math::FNV1a_32("DrawBuffer");
		static const u32 wvpHash = Math::FNV1a_32("WVP");

		Math::SIMDMatrix mtx = Math::SIMD_MTX4X4_IDENTITY;
		mtx *= m_transform.GetWorldMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix();

		m_pMaterial->SetFloat(maxtrixBufferHash, wvpHash, mtx.f32, 16);
	}

	void CCyberGrid::Release()
	{
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}
};
