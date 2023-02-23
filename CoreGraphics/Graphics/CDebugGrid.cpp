//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CDebugGrid.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CDebugGrid.h"
#include "CDebug3D.h"
#include "CMeshRenderer_.h"
#include "CMaterial.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include "../Application/CSceneManager.h"
#include <Utilities/CMemoryFree.h>
#include <Math/CMathFNV.h>

namespace Graphics
{
	CDebugGrid::CDebugGrid(const wchar_t* pName, u32 viewHash) :
		CVObject(pName, viewHash),
		m_transform(this),
		m_meshData(this),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr) {
	}

	CDebugGrid::~CDebugGrid() { }

	void CDebugGrid::Initialize()
	{
		{ // Create the mesh data.
			CMeshData_::Data data { };
			data.topology = PRIMITIVE_TOPOLOGY_LINELIST;
			data.vertexCount = (m_data.width + m_data.length) << 1;
			data.vertexStride = sizeof(DebugVertex3D);

			m_meshData.SetData(data);
			m_meshData.Initialize();

			const float wh = m_data.cellSize.x * (m_data.width - 1) * 0.5f;
			const float lh = m_data.cellSize.y * (m_data.length - 1) * 0.5f;
			u32 index = 0;

			for(u32 i = 0; i < m_data.width; ++i)
			{
				*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.right * (-wh + m_data.cellSize.x * i) + m_data.forward * -lh };
				*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.right * (-wh + m_data.cellSize.x * i) + m_data.forward * lh };
			}

			for(u32 i = 0; i < m_data.length; ++i)
			{
				*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.right * -wh + m_data.forward * (-lh + m_data.cellSize.y * i) };
				*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.right * wh + m_data.forward * (-lh + m_data.cellSize.y * i) };
			}
		}

		{ // Create the mesh renderer.
			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(this);

			CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CDebugGrid::PreRender, this);
			data.pMaterial = m_pMaterial = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matHash));
			data.pMeshData = &m_meshData;
			m_pMeshRenderer->SetData(data);

			m_pMeshRenderer->Initialize();
		}
	}

	void CDebugGrid::PreRender()
	{
		static const u32 maxtrixBufferHash = Math::FNV1a_32("MatrixBuffer");
		static const u32 wvpHash = Math::FNV1a_32("WVP");

		Math::SIMDMatrix mtx = Math::SIMD_MTX4X4_IDENTITY;
		mtx *= m_transform.GetWorldMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix();

		m_pMaterial->SetFloat(maxtrixBufferHash, wvpHash, mtx.f32, 16);
	}

	void CDebugGrid::Release()
	{
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}
};
