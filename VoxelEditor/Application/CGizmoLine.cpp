//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoLine.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmoLine.h"
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
	CGizmoLine::CGizmoLine(const CVObject* pObject) :
		CVComponent(pObject),
		m_meshData(nullptr),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr),
		m_pGraphicsAPI(nullptr)
	{
	}

	CGizmoLine::~CGizmoLine()
	{
	}

	void CGizmoLine::Initialize()
	{
		{ // Create the mesh data.
			Graphics::CMeshData_::Data data { };
			data.topology = Graphics::PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.vertexCount = 4;
			data.indexCount = 6;
			data.instanceMax = m_data.instanceMax;
			data.vertexStride = sizeof(GizmoVertexLine);
			data.instanceStride = sizeof(GizmoInstanceLine);
			data.indexStride = sizeof(u16);

			m_meshData.SetData(data);
			m_meshData.Initialize();
			m_meshData.SetInstanceCount(0);
			
			Math::Vector3 normal = Math::VEC3_ONE.Normalized();
			*(GizmoVertexLine*)m_meshData.GetVertexAt(0) = { { 0.0f }, normal };
			*(GizmoVertexLine*)m_meshData.GetVertexAt(1) = { { 0.0f }, normal };
			*(GizmoVertexLine*)m_meshData.GetVertexAt(2) = { { 1.0f }, normal };
			*(GizmoVertexLine*)m_meshData.GetVertexAt(3) = { { 1.0f }, normal };
			
			*(u16*)m_meshData.GetIndexAt(0) = 0;
			*(u16*)m_meshData.GetIndexAt(1) = 1;
			*(u16*)m_meshData.GetIndexAt(2) = 2;
			*(u16*)m_meshData.GetIndexAt(3) = 1;
			*(u16*)m_meshData.GetIndexAt(4) = 3;
			*(u16*)m_meshData.GetIndexAt(5) = 2;
		}
		
		m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matHash));

		{ // Create the mesh renderer.
			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(m_pObject);

			Graphics::CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CGizmoLine::PreRender, this);
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

	void CGizmoLine::PreRender()
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

	void CGizmoLine::Render()
	{
		Graphics::CMeshForceRender_ render(m_pMeshRenderer);
	}

	void CGizmoLine::Release()
	{
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Draw methods.
	//-----------------------------------------------------------------------------------------------

	void CGizmoLine::Draw(const Math::SIMDVector& origin, const Math::SIMDVector& offset, const Math::SIMDVector& dir, float length, float width, const Math::Color& color)
	{
		GizmoDrawLine data;
		data.instance.world = Math::SIMDMatrix::Scale(dir * length) * Math::SIMDMatrix::Translate(offset);
		data.instance.world.Transpose();

		data.instance.origin = *(Math::Vector3*)origin.ToFloat();
		data.instance.color = color;
		data.instance.width = width;
		m_drawQueue.push_back(data);
	}
};
