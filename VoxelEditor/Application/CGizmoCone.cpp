//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoCone.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmoCone.h"
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
	CGizmoCone::CGizmoCone(const CVObject* pObject) :
		CVComponent(pObject),
		m_meshData(nullptr),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr),
		m_pGraphicsAPI(nullptr)
	{
	}

	CGizmoCone::~CGizmoCone()
	{
	}

	void CGizmoCone::Initialize()
	{
		{ // Create the mesh data.
			Graphics::CMeshData_::Data data { };
			data.topology = Graphics::PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.vertexCount = m_data.iterations * 2 + 1;
			data.indexCount = ((m_data.iterations - 2) + m_data.iterations) * 3;
			data.instanceMax = m_data.instanceMax;
			data.vertexStride = sizeof(GizmoVertexGeo);
			data.instanceStride = sizeof(GizmoInstanceGeo);
			data.indexStride = sizeof(u16);

			m_meshData.SetData(data);
			m_meshData.Initialize();
			m_meshData.SetInstanceCount(0);

			u32 vertex = 0;
			u32 index = 0;
			u32 capOffset = m_data.iterations;

			const float stepSize = (1.0f / m_data.iterations) * Math::g_2Pi;

			*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { Math::VEC3_FORWARD * m_data.height, 0.0f };
			for(u32 i = 0; i < m_data.iterations; ++i)
			{
				float a0 = stepSize * i;
				Math::Vector3 v0(cosf(a0) * m_data.radius, sinf(a0) * m_data.radius, 0.0f);
				Math::Vector3 v1(0.0f, 0.0f, m_data.height);

				*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex) = { v0, Math::Vector3::Cross(Math::Vector3::Cross(v1 - v0, v0), v1 - v0).Normalized() };
				*(GizmoVertexGeo*)m_meshData.GetVertexAt(capOffset + vertex) = { v0, Math::VEC3_BACKWARD };
				++vertex;
			}

			for(u32 i = 0; i < m_data.iterations; ++i)
			{
				*(u16*)m_meshData.GetIndexAt(index++) = i + 1;
				*(u16*)m_meshData.GetIndexAt(index++) = (i + 1) % m_data.iterations + 1;
				*(u16*)m_meshData.GetIndexAt(index++) = 0;
			}

			vertex = ++capOffset;
			for(u32 i = 1; i < m_data.iterations - 1; ++i)
			{
				*(u16*)m_meshData.GetIndexAt(index++) = capOffset;
				*(u16*)m_meshData.GetIndexAt(index++) = vertex + 2;
				*(u16*)m_meshData.GetIndexAt(index++) = vertex + 1;
				vertex += 1;
			}
		}
		
		m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matHash));

		{ // Create the mesh renderer.
			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(m_pObject);

			Graphics::CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CGizmoCone::PreRender, this);
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

	void CGizmoCone::PreRender()
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

	void CGizmoCone::Render()
	{
		Graphics::CMeshForceRender_ render(m_pMeshRenderer);
	}

	void CGizmoCone::Release()
	{
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Draw methods.
	//-----------------------------------------------------------------------------------------------

	void CGizmoCone::Draw(const Math::SIMDVector& origin, const Math::SIMDVector& offset, const Math::SIMDQuaternion& rotation, const Math::SIMDVector& scale, const Math::Color& color)
	{
		GizmoDrawGeo data;
		data.instance.world = Math::SIMDMatrix::Scale(scale) * Math::SIMDMatrix::Rotate(rotation) * Math::SIMDMatrix::Translate(offset);
		data.instance.world.Transpose();

		data.instance.origin = *(Math::Vector3*)origin.ToFloat();
		data.instance.color = color;
		m_drawQueue.push_back(data);
	}
};
