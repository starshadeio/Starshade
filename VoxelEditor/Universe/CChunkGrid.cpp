//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CChunkGrid.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CChunkGrid.h"
#include "../Application/CAppData.h"
#include "../Application/CEditor.h"
#include <Graphics/CMeshRenderer_.h>
#include <Graphics/CMaterial.h>
#include <Graphics/CGraphicsAPI.h>
#include <Factory/CFactory.h>
#include <Resources/CResourceManager.h>
#include <Application/CSceneManager.h>
#include <Utilities/CMemoryFree.h>
#include <Math/CMathFNV.h>

namespace Universe
{
	CChunkGrid::CChunkGrid() :
		CVObject(L"Chunk Grid", App::VIEW_HASH_MAIN),
		m_scale(1.0f),
		m_transform(this),
		m_meshData(this),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr),
		m_pMaterialUnderlay(nullptr)
	{
	}

	CChunkGrid::~CChunkGrid()
	{
	}

	void CChunkGrid::Initialize()
	{
		{ // Create the mesh data.
			struct Vertex
			{
				Math::Vector3 position;
				Math::Vector3 adjacent;
				Math::Vector3 normal;
			};

			typedef u16 Index;

			Graphics::CMeshData_::Data data { };
			data.topology = Graphics::PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.vertexCount = 4;
			data.indexCount = 6;
			data.instanceMax = (128 * 4 * 2) * 6;
			data.vertexStride = sizeof(Vertex);
			data.instanceStride = sizeof(InstanceLine);
			data.indexStride = sizeof(Index);

			m_meshData.SetData(data);
			m_meshData.Initialize();
			
			m_meshData.SetInstanceCount(0);
			
			Math::Vector3 normal = Math::VEC3_ONE.Normalized();
			*(Vertex*)m_meshData.GetVertexAt(0) = { { 0.0f }, { 1.0f }, normal };
			*(Vertex*)m_meshData.GetVertexAt(1) = { { 0.0f }, { 1.0f }, normal };
			*(Vertex*)m_meshData.GetVertexAt(2) = { { 1.0f }, { 0.0f }, normal };
			*(Vertex*)m_meshData.GetVertexAt(3) = { { 1.0f }, { 0.0f }, normal };
			
			*(u16*)m_meshData.GetIndexAt(0) = 0;
			*(u16*)m_meshData.GetIndexAt(1) = 1;
			*(u16*)m_meshData.GetIndexAt(2) = 2;
			*(u16*)m_meshData.GetIndexAt(3) = 1;
			*(u16*)m_meshData.GetIndexAt(4) = 3;
			*(u16*)m_meshData.GetIndexAt(5) = 2;
		}

		{ // Create the mesh renderer.
			m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64("MATERIAL_CHUNK_GRID")));
			m_pMaterialUnderlay = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64("MATERIAL_CHUNK_GRID_UNDERLAY")));
			
			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(this);

			Graphics::CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CChunkGrid::PreRender, this, std::placeholders::_1);
			data.pMeshData = &m_meshData;
			data.bDynamicInstances = true;
			data.bDynamicInstanceCount = true;
			data.bSkipRegistration = false;
			m_pMeshRenderer->SetData(data);
			m_pMeshRenderer->AddMaterial(m_pMaterial);
			m_pMeshRenderer->AddMaterial(m_pMaterialUnderlay);

			m_pMeshRenderer->Initialize();
		}

		m_pGraphicsAPI = CFactory::Instance().GetGraphicsAPI();

		App::CEditor::Instance().Nodes().Gizmo().Workspace().SetChunkGrid(this);

		SetExtents(-m_data.startingHalfExtents + m_data.startingOffset, m_data.startingHalfExtents + m_data.startingOffset);
	}

	void CChunkGrid::PostInitialize()
	{
		m_pMeshRenderer->PopMaterial();
	}

	void CChunkGrid::PreRender(Graphics::CMaterial* pMaterial)
	{
		static const u32 dataBufferHash = Math::FNV1a_32("DataBuffer");
		static const u32 screenSizeHash = Math::FNV1a_32("ScreenSize");

		static const u32 matrixBufferHash = Math::FNV1a_32("MatrixBuffer");
		static const u32 worldHash = Math::FNV1a_32("World");
		static const u32 vpHash = Math::FNV1a_32("VP");
		static const u32 vpClipHash = Math::FNV1a_32("VPClip");
		static const u32 cameraPositionHash = Math::FNV1a_32("CameraPosition");
		static const u32 cameraForwardHash = Math::FNV1a_32("CameraForward");
		
		Math::Vector4 screenSize(m_pGraphicsAPI->GetWidth(), m_pGraphicsAPI->GetHeight(), 1.0f / m_pGraphicsAPI->GetWidth(), 1.0f / m_pGraphicsAPI->GetHeight());

		Math::SIMDMatrix world = m_transform.GetWorldMatrix();// * Math::SIMDMatrix::Translate(-App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition());
		
		Math::SIMDMatrix mtx = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix();

		Math::SIMDMatrix mtxClip = Math::SIMDMatrix::Transpose(Math::SIMDMatrix(
			App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetFrustumPlane(0).m_xmm,
			App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetFrustumPlane(1).m_xmm,
			App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetFrustumPlane(2).m_xmm,
			App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetFrustumPlane(3).m_xmm
		));
		
		pMaterial->SetFloat(dataBufferHash, screenSizeHash, &screenSize[0], 4);
		pMaterial->SetFloat(matrixBufferHash, worldHash, world.f32, 16);
		pMaterial->SetFloat(matrixBufferHash, vpHash, mtx.f32, 16);
		pMaterial->SetFloat(matrixBufferHash, vpClipHash, mtxClip.f32, 16);
		pMaterial->SetFloat(matrixBufferHash, cameraPositionHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition().ToFloat(), 4);
		pMaterial->SetFloat(matrixBufferHash, cameraForwardHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetForward().ToFloat(), 4);
	}

	void CChunkGrid::Release()
	{
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CChunkGrid::SetActive(bool bActive)
	{
		m_pMeshRenderer->SetActive(bActive);
	}
		
	void CChunkGrid::SetEdit(bool bEdit)
	{
		if(bEdit)
		{
			if(m_pMeshRenderer->GetMaterialCount() == 1)
			{
				m_pMeshRenderer->AddMaterial(m_pMaterialUnderlay);
			}
		}
		else
		{
			if(m_pMeshRenderer->GetMaterialCount() == 2)
			{
				m_pMeshRenderer->PopMaterial();
			}
		}
	}

	void CChunkGrid::SetExtents(const Math::Vector3& minExtents, const Math::Vector3& maxExtents)
	{
		auto lastScale = m_scale;
		auto lastOffset = m_offset;
		m_scale = maxExtents - minExtents;
		m_offset = m_scale * 0.5f + minExtents;

		if(lastScale != m_scale || lastOffset != m_offset)
		{
			OnUpdateScale();
		}
	}

	void CChunkGrid::SetOffset(const Math::Vector3& offset)
	{
		m_offset = offset;
		OnUpdateScale();
	}

	void CChunkGrid::OnUpdateScale()
	{
		{
			auto offsetVec = *(Math::Vector3*)m_offset.ToFloat();
			App::CEditor::Instance().Nodes().GetChunkEditor().GetNode()->GetChunkNode()->SetExtentsWorkspace(offsetVec - m_scale * 0.5f, offsetVec + m_scale * 0.5f);
		}

		m_meshData.ResetInstances();
		auto DrawLine = [this](const Math::SIMDVector& offset, const Math::SIMDVector& dir, const Math::Vector3& normal, float length, float width, const Math::Color& color){
			InstanceLine data;
			data.world = Math::SIMDMatrix::Scale(dir * length) * Math::SIMDMatrix::Translate(offset + m_offset);
			data.world.Transpose();

			data.color = color;
			data.width = width;
			data.normal = normal;

			*(InstanceLine*)m_meshData.GetInstanceNext() = data;
		};
		
		auto r = Math::SIMD_VEC_RIGHT * 0.5f * m_scale.x;
		auto u = Math::SIMD_VEC_UP * 0.5f * m_scale.y;
		auto f = Math::SIMD_VEC_FORWARD * 0.5f * m_scale.z;
		auto outlineColor = Math::Color(0.0f, 0.0f, 0.0f, 0.75f);
		float outlineWidth = 3.0f;

		auto innerColor = Math::Color(0.0f, 0.0f, 0.0f, 0.5f);
		float innerWidth = 2.0f;
		
		DrawLine(-r - u - f, Math::SIMD_VEC_RIGHT, Math::VEC3_ZERO, m_scale.x, outlineWidth, outlineColor);
		DrawLine(-r - u + f, Math::SIMD_VEC_RIGHT, Math::VEC3_ZERO, m_scale.x, outlineWidth, outlineColor);
		DrawLine(-r + u + f, Math::SIMD_VEC_RIGHT, Math::VEC3_ZERO, m_scale.x, outlineWidth, outlineColor);
		DrawLine(-r + u - f, Math::SIMD_VEC_RIGHT, Math::VEC3_ZERO, m_scale.x, outlineWidth, outlineColor);
		
		for(float y = 1.0f; y < m_scale.y; y += 1.0f)
		{
			DrawLine(-r - u - f + (Math::SIMD_VEC_UP * y), Math::SIMD_VEC_FORWARD, Math::VEC3_LEFT, m_scale.z, innerWidth, innerColor);
			DrawLine( r - u - f + (Math::SIMD_VEC_UP * y), Math::SIMD_VEC_FORWARD, Math::VEC3_RIGHT, m_scale.z, innerWidth, innerColor);
		}
		
		for(float z = 1.0f; z < m_scale.z; z += 1.0f)
		{
			DrawLine(-r - u - f + (Math::SIMD_VEC_FORWARD * z), Math::SIMD_VEC_UP, Math::VEC3_LEFT, m_scale.y, innerWidth, innerColor);
			DrawLine( r - u - f + (Math::SIMD_VEC_FORWARD * z), Math::SIMD_VEC_UP, Math::VEC3_RIGHT, m_scale.y, innerWidth, innerColor);
		}

		DrawLine(-r - u - f, Math::SIMD_VEC_UP, Math::VEC3_ZERO, m_scale.y, outlineWidth, outlineColor);
		DrawLine( r - u - f, Math::SIMD_VEC_UP, Math::VEC3_ZERO, m_scale.y, outlineWidth, outlineColor);
		DrawLine( r - u + f, Math::SIMD_VEC_UP, Math::VEC3_ZERO, m_scale.y, outlineWidth, outlineColor);
		DrawLine(-r - u + f, Math::SIMD_VEC_UP, Math::VEC3_ZERO, m_scale.y, outlineWidth, outlineColor);
		
		for(float x = 1.0f; x < m_scale.x; x += 1.0f)
		{
			DrawLine(-r - u - f + (Math::SIMD_VEC_RIGHT * x), Math::SIMD_VEC_FORWARD, Math::VEC3_DOWN, m_scale.z, innerWidth, innerColor);
			DrawLine(-r + u - f + (Math::SIMD_VEC_RIGHT * x), Math::SIMD_VEC_FORWARD, Math::VEC3_UP, m_scale.z, innerWidth, innerColor);
		}
		
		for(float z = 1.0f; z < m_scale.z; z += 1.0f)
		{
			DrawLine(-r - u - f + (Math::SIMD_VEC_FORWARD * z), Math::SIMD_VEC_RIGHT, Math::VEC3_DOWN, m_scale.x, innerWidth, innerColor);
			DrawLine(-r + u - f + (Math::SIMD_VEC_FORWARD * z), Math::SIMD_VEC_RIGHT, Math::VEC3_UP, m_scale.x, innerWidth, innerColor);
		}

		DrawLine(-r - u - f, Math::SIMD_VEC_FORWARD, Math::VEC3_ZERO, m_scale.z, outlineWidth, outlineColor);
		DrawLine( r - u - f, Math::SIMD_VEC_FORWARD, Math::VEC3_ZERO, m_scale.z, outlineWidth, outlineColor);
		DrawLine( r + u - f, Math::SIMD_VEC_FORWARD, Math::VEC3_ZERO, m_scale.z, outlineWidth, outlineColor);
		DrawLine(-r + u - f, Math::SIMD_VEC_FORWARD, Math::VEC3_ZERO, m_scale.z, outlineWidth, outlineColor);
		
		for(float x = 1.0f; x < m_scale.x; x += 1.0f)
		{
			DrawLine(-r - u - f + (Math::SIMD_VEC_RIGHT * x), Math::SIMD_VEC_UP, Math::VEC3_BACKWARD, m_scale.y, innerWidth, innerColor);
			DrawLine(-r - u + f + (Math::SIMD_VEC_RIGHT * x), Math::SIMD_VEC_UP, Math::VEC3_FORWARD, m_scale.y, innerWidth, innerColor);
		}
		
		for(float y = 1.0f; y < m_scale.y; y += 1.0f)
		{
			DrawLine(-r - u - f + (Math::SIMD_VEC_UP * y), Math::SIMD_VEC_RIGHT, Math::VEC3_BACKWARD, m_scale.x, innerWidth, innerColor);
			DrawLine(-r - u + f + (Math::SIMD_VEC_UP * y), Math::SIMD_VEC_RIGHT, Math::VEC3_FORWARD, m_scale.x, innerWidth, innerColor);
		}
	}
};
