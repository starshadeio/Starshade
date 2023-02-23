//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CDebugOBB.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CDebugOBB.h"
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
	CDebugOBB::CDebugOBB(const wchar_t* pName, u32 viewHash) :
		CVObject(pName, viewHash),
		m_transform(this),
		m_meshData(this),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr) {
	}

	CDebugOBB::~CDebugOBB() { }

	void CDebugOBB::Initialize()
	{
		{ // Create the mesh data.
			CMeshData_::Data data { };
			data.topology = PRIMITIVE_TOPOLOGY_LINELIST;
			data.vertexCount = 24;
			data.vertexStride = sizeof(DebugVertex3D);

			m_meshData.SetData(data);
			m_meshData.Initialize();

			u32 index = 0;

			// Bottom.
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3(-m_data.halfSize.x, -m_data.halfSize.y, -m_data.halfSize.z) };
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3( m_data.halfSize.x, -m_data.halfSize.y, -m_data.halfSize.z) };

			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3( m_data.halfSize.x, -m_data.halfSize.y, -m_data.halfSize.z) };
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3( m_data.halfSize.x, -m_data.halfSize.y,  m_data.halfSize.z) };

			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3( m_data.halfSize.x, -m_data.halfSize.y,  m_data.halfSize.z) };
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3(-m_data.halfSize.x, -m_data.halfSize.y,  m_data.halfSize.z) };

			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3(-m_data.halfSize.x, -m_data.halfSize.y,  m_data.halfSize.z) };
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3(-m_data.halfSize.x, -m_data.halfSize.y, -m_data.halfSize.z) };

			// Sides.
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3(-m_data.halfSize.x, -m_data.halfSize.y, -m_data.halfSize.z) };
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3(-m_data.halfSize.x,  m_data.halfSize.y, -m_data.halfSize.z) };

			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3( m_data.halfSize.x, -m_data.halfSize.y, -m_data.halfSize.z) };
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3( m_data.halfSize.x,  m_data.halfSize.y, -m_data.halfSize.z) };
			
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3( m_data.halfSize.x, -m_data.halfSize.y,  m_data.halfSize.z) };
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3( m_data.halfSize.x,  m_data.halfSize.y,  m_data.halfSize.z) };
			
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3(-m_data.halfSize.x, -m_data.halfSize.y,  m_data.halfSize.z) };
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3(-m_data.halfSize.x,  m_data.halfSize.y,  m_data.halfSize.z) };

			// Top.
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3(-m_data.halfSize.x,  m_data.halfSize.y, -m_data.halfSize.z) };
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3( m_data.halfSize.x,  m_data.halfSize.y, -m_data.halfSize.z) };

			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3( m_data.halfSize.x,  m_data.halfSize.y, -m_data.halfSize.z) };
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3( m_data.halfSize.x,  m_data.halfSize.y,  m_data.halfSize.z) };

			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3( m_data.halfSize.x,  m_data.halfSize.y,  m_data.halfSize.z) };
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3(-m_data.halfSize.x,  m_data.halfSize.y,  m_data.halfSize.z) };

			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3(-m_data.halfSize.x,  m_data.halfSize.y,  m_data.halfSize.z) };
			*(DebugVertex3D*)m_meshData.GetVertexAt(index++) = { m_data.offset + Math::Vector3(-m_data.halfSize.x,  m_data.halfSize.y, -m_data.halfSize.z) };
		}

		{ // Create the mesh renderer.
			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(this);

			CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CDebugOBB::PreRender, this);
			data.pMaterial = m_pMaterial = reinterpret_cast<CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matHash));
			data.pMeshData = &m_meshData;
			m_pMeshRenderer->SetData(data);

			m_pMeshRenderer->Initialize();
		}
	}

	void CDebugOBB::PreRender()
	{
		static const u32 maxtrixBufferHash = Math::FNV1a_32("MatrixBuffer");
		static const u32 wvpHash = Math::FNV1a_32("WVP");

		Math::SIMDMatrix mtx = Math::SIMD_MTX4X4_IDENTITY;
		mtx *= m_transform.GetWorldMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix();

		m_pMaterial->SetFloat(maxtrixBufferHash, wvpHash, mtx.f32, 16);
	}

	void CDebugOBB::Release()
	{
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}
};
