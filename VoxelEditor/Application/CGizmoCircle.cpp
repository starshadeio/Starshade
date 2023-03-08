//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoCircle.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmoCircle.h"
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
	CGizmoCircle::CGizmoCircle(const CVObject* pObject) :
		CVComponent(pObject),
		m_meshData(nullptr),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr),
		m_pGraphicsAPI(nullptr)
	{
	}

	CGizmoCircle::~CGizmoCircle()
	{
	}

	void CGizmoCircle::Initialize()
	{
		{ // Create the mesh data.
			Graphics::CMeshData_::Data data { };
			data.topology = Graphics::PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.vertexCount = m_data.iterations * (m_data.bHalfCircle ? 2 : 4);
			data.indexCount = m_data.iterations * (m_data.bHalfCircle ? 3 : 6);
			data.instanceMax = m_data.instanceMax;
			data.vertexStride = sizeof(GizmoVertexLine);
			data.instanceStride = sizeof(GizmoInstanceLine);
			data.indexStride = sizeof(u16);

			m_meshData.SetData(data);
			m_meshData.Initialize();
			m_meshData.SetInstanceCount(0);
			
			u32 vertex = 0;
			u32 index = 0;

			const float stepSize = (1.0f / m_data.iterations) * Math::g_2Pi;

			for(u32 i = 0; i < (m_data.iterations >> (m_data.bHalfCircle ? 1 : 0)); ++i)
			{
				float a0 = stepSize * i;
				float a1 = stepSize * ((i + 1) % m_data.iterations);
				Math::Vector3 v0(cosf(a0) * m_data.radius, sinf(a0) * m_data.radius, 0.0f);
				Math::Vector3 v1(cosf(a1) * m_data.radius, sinf(a1) * m_data.radius, 0.0f);
				Math::Vector3 n = (v1 - v0).Normalized();
				
				*(u16*)m_meshData.GetIndexAt(index++) = vertex + 0;
				*(u16*)m_meshData.GetIndexAt(index++) = vertex + 1;
				*(u16*)m_meshData.GetIndexAt(index++) = vertex + 2;
				*(u16*)m_meshData.GetIndexAt(index++) = vertex + 1;
				*(u16*)m_meshData.GetIndexAt(index++) = vertex + 3;
				*(u16*)m_meshData.GetIndexAt(index++) = vertex + 2;

				*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { v0, n };
				*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { v0, n };

				*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { v1, n };
				*(GizmoVertexGeo*)m_meshData.GetVertexAt(vertex++) = { v1, n };
			}
		}
		
		m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matHash));

		{ // Create the mesh renderer.
			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(m_pObject);

			Graphics::CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CGizmoCircle::PreRender, this);
			data.pMeshData = &m_meshData;
			data.bDynamicInstances = true;
			data.bDynamicInstanceCount = true;
			data.bSkipRegistration = true;
			m_pMeshRenderer->SetData(data);
			m_pMeshRenderer->AddMaterial(m_pMaterial);

			m_pMeshRenderer->Initialize();
		}

		m_pGraphicsAPI = CFactory::Instance().GetGraphicsAPI();
	}

	void CGizmoCircle::PreRender()
	{
		{
			m_meshData.ResetInstances();
			size_t sz = m_drawQueue.size();
			for(size_t i = 0; i < sz; ++i)
			{
				auto& data = m_drawQueue.front();
				*(GizmoInstanceLine*)m_meshData.GetInstanceNext() = data.instance;

				m_drawQueue.pop_front();
			}
		}
		
		static const u32 dataBufferHash = Math::FNV1a_32("DataBuffer");
		static const u32 screenSizeHash = Math::FNV1a_32("ScreenSize");

		static const u32 matrixBufferHash = Math::FNV1a_32("MatrixBuffer");
		static const u32 vpHash = Math::FNV1a_32("VP");
		static const u32 cameraPositionHash = Math::FNV1a_32("CameraPosition");
		static const u32 cameraForwardHash = Math::FNV1a_32("CameraForward");
		
		Math::Vector4 screenSize(m_pGraphicsAPI->GetWidth(), m_pGraphicsAPI->GetHeight(), 1.0f / m_pGraphicsAPI->GetWidth(), 1.0f / m_pGraphicsAPI->GetHeight());
		m_pMaterial->SetFloat(dataBufferHash, screenSizeHash, &screenSize[0], 4);

		Math::SIMDMatrix mtx = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix();

		m_pMaterial->SetFloat(matrixBufferHash, vpHash, mtx.f32, 16);
		m_pMaterial->SetFloat(matrixBufferHash, cameraPositionHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition().ToFloat(), 4);
		m_pMaterial->SetFloat(matrixBufferHash, cameraForwardHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetForward().ToFloat(), 4);
	}

	void CGizmoCircle::Render()
	{
		Graphics::CMeshForceRender_ render(m_pMeshRenderer);
	}

	void CGizmoCircle::Release()
	{
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Draw methods.
	//-----------------------------------------------------------------------------------------------

	void CGizmoCircle::Draw(const Math::SIMDVector& origin, const Math::SIMDVector& offset, const Math::SIMDQuaternion& rotation, const Math::SIMDVector& scale, float width, const Math::Color& color)
	{
		GizmoDrawLine data;
		data.instance.world = Math::SIMDMatrix::Scale(scale) * Math::SIMDMatrix::Rotate(rotation) * Math::SIMDMatrix::Translate(offset);
		data.instance.world.Transpose();

		data.instance.origin = *(Math::Vector3*)origin.ToFloat();
		data.instance.color = color;
		data.instance.width = width;
		m_drawQueue.push_back(data);
	}
};
