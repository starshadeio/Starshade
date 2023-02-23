//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoCube.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmoCube.h"
#include <Graphics/CMeshRenderer_.h>
#include <Graphics/CMaterial.h>
#include <Graphics/CShader.h>
#include <Graphics/CGraphicsAPI.h>
#include <Factory/CFactory.h>
#include <Resources/CResourceManager.h>
#include <Application/CSceneManager.h>
#include <Utilities/CMemoryFree.h>

namespace App
{
	CGizmoCube::CGizmoCube(const CVObject* pObject) :
		CVComponent(pObject),
		m_meshData(nullptr),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr),
		m_pGraphicsAPI(nullptr)
	{
	}

	CGizmoCube::~CGizmoCube()
	{
	}

	void CGizmoCube::Initialize()
	{
		{ // Create the mesh data.
			Graphics::CMeshData_::Data data { };
			data.topology = Graphics::PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.vertexCount = 24;
			data.indexCount = 36;
			data.instanceMax = m_data.instanceMax;
			data.vertexStride = sizeof(GizmoVertexGeo);
			data.instanceStride = sizeof(GizmoInstanceGeo);
			data.indexStride = sizeof(u16);

			m_meshData.SetData(data);
			m_meshData.Initialize();
			m_meshData.SetInstanceCount(0);

			const Math::Vector3 r = Math::VEC3_RIGHT;
			const Math::Vector3 u = Math::VEC3_UP;
			const Math::Vector3 f = Math::VEC3_FORWARD;
			const float hs = 0.5f;
			
			const Math::Vector3 o = Math::VEC3_FORWARD * hs;
			
			u32 index = 0;
			u32 vertex = 0;
			
			auto AddIndices = [&](){
				*(u16*)m_meshData.GetIndexAt(index++) = vertex;
				*(u16*)m_meshData.GetIndexAt(index++) = vertex + 1;
				*(u16*)m_meshData.GetIndexAt(index++) = vertex + 3;
				*(u16*)m_meshData.GetIndexAt(index++) = vertex + 1;
				*(u16*)m_meshData.GetIndexAt(index++) = vertex + 2;
				*(u16*)m_meshData.GetIndexAt(index++) = vertex + 3;
			};

			AddIndices();
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + (-r + f - u) * hs, Math::VEC3_LEFT };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + (-r - f - u) * hs, Math::VEC3_LEFT };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + (-r - f + u) * hs, Math::VEC3_LEFT };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + (-r + f + u) * hs, Math::VEC3_LEFT };
			
			AddIndices();
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + ( r - f - u) * hs, Math::VEC3_RIGHT };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + ( r + f - u) * hs, Math::VEC3_RIGHT };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + ( r + f + u) * hs, Math::VEC3_RIGHT };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + ( r - f + u) * hs, Math::VEC3_RIGHT };
			
			AddIndices();
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + (-u - r + f) * hs, Math::VEC3_DOWN };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + (-u + r + f) * hs, Math::VEC3_DOWN };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + (-u + r - f) * hs, Math::VEC3_DOWN };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + (-u - r - f) * hs, Math::VEC3_DOWN };
			
			AddIndices();
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + ( u - r - f) * hs, Math::VEC3_UP };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + ( u + r - f) * hs, Math::VEC3_UP };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + ( u + r + f) * hs, Math::VEC3_UP };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + ( u - r + f) * hs, Math::VEC3_UP };
			
			AddIndices();
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + (-f - r - u) * hs, Math::VEC3_BACKWARD };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + (-f + r - u) * hs, Math::VEC3_BACKWARD };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + (-f + r + u) * hs, Math::VEC3_BACKWARD };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + (-f - r + u) * hs, Math::VEC3_BACKWARD };
			
			AddIndices();
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + ( f + r - u) * hs, Math::VEC3_FORWARD };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + ( f - r - u) * hs, Math::VEC3_FORWARD };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + ( f - r + u) * hs, Math::VEC3_FORWARD };
			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { o + ( f + r + u) * hs, Math::VEC3_FORWARD };
		}
		
		m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matHash));

		{ // Create the mesh renderer.
			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(m_pObject);

			Graphics::CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CGizmoCube::PreRender, this);
			data.pMaterial = m_pMaterial;
			data.pMeshData = &m_meshData;
			data.bDynamicInstances = true;
			data.bDynamicInstanceCount = true;
			data.bSkipRegistration = true;
			m_pMeshRenderer->SetData(data);

			m_pMeshRenderer->Initialize();
		}

		m_pGraphicsAPI = CFactory::Instance().GetGraphicsAPI();
	}

	void CGizmoCube::PreRender()
	{
		{
			m_meshData.ResetInstances();
			size_t sz = m_drawQueue.size();
			for(size_t i = 0; i < sz; ++i)
			{
				auto& data = m_drawQueue.front();
				*(GizmoInstanceGeo*)m_meshData.GetInstanceNext() = data.instance;

				m_drawQueue.pop_front();
			}
		}
		
		static const u32 matrixBufferHash = Math::FNV1a_32("MatrixBuffer");
		static const u32 vpHash = Math::FNV1a_32("VP");
		static const u32 cameraPositionHash = Math::FNV1a_32("CameraPosition");
		static const u32 cameraForwardHash = Math::FNV1a_32("CameraForward");
		
		Math::SIMDMatrix mtx = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix();

		m_pMaterial->SetFloat(matrixBufferHash, vpHash, mtx.f32, 16);
		m_pMaterial->SetFloat(matrixBufferHash, cameraPositionHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition().ToFloat(), 4);
		m_pMaterial->SetFloat(matrixBufferHash, cameraForwardHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetForward().ToFloat(), 4);
	}

	void CGizmoCube::Render()
	{
		Graphics::CMeshForceRender_ render(m_pMeshRenderer);
	}

	void CGizmoCube::Release()
	{
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Draw methods.
	//-----------------------------------------------------------------------------------------------

	void CGizmoCube::Draw(const Math::SIMDVector& origin, const Math::SIMDVector& offset, const Math::SIMDQuaternion& rotation, const Math::SIMDVector& scale, const Math::Color& color)
	{
		GizmoDrawGeo data;
		data.instance.world = Math::SIMDMatrix::Scale(scale) * Math::SIMDMatrix::Rotate(rotation) * Math::SIMDMatrix::Translate(offset);
		data.instance.world.Transpose();
		
		data.instance.origin = *(Math::Vector3*)origin.ToFloat();
		data.instance.color = color;
		m_drawQueue.push_back(data);
	}
};
