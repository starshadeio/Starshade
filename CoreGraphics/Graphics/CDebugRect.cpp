//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDebugRect.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CDebugRect.h"
#include "../Graphics/CMeshRenderer_.h"
#include "../Graphics/CMaterial.h"
#include "../Graphics/CShader.h"
#include "../Graphics/CRootSignature.h"
#include "../Graphics/CGraphicsAPI.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include "../Application/CSceneManager.h"
#include <Utilities/CMemoryFree.h>
#include <Utilities/CTimer.h>

namespace Graphics
{
	CDebugRect::CDebugRect(const CVObject* pObject) :
		CVComponent(pObject),
		m_meshData(nullptr),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr),
		m_pGraphicsAPI(nullptr)
	{
	}

	CDebugRect::~CDebugRect()
	{
	}

	void CDebugRect::Initialize()
	{
		{ // Create the mesh data.
			Graphics::CMeshData_::Data data { };
			data.topology = Graphics::PRIMITIVE_TOPOLOGY_LINELIST;
			data.vertexCount = 8;
			data.instanceMax = m_data.instanceMax;
			data.vertexStride = sizeof(DebugVertex2D);
			data.instanceStride = sizeof(DebugInstance2D);

			m_meshData.SetData(data);
			m_meshData.Initialize();
			m_meshData.SetInstanceCount(0);

			static const float halfSize = 0.5f;
			static const Math::Vector2 right(1.0f, 0.0f);
			static const Math::Vector2 up(0.0f, 1.0f);
			
			*(DebugVertex2D*)m_meshData.GetVertexAt(0) = { { right * 0.0f + up * 0.0f } };
			*(DebugVertex2D*)m_meshData.GetVertexAt(1) = { { right * 1.0f + up * 0.0f } };
			*(DebugVertex2D*)m_meshData.GetVertexAt(2) = { { right * 1.0f + up * 0.0f } };
			*(DebugVertex2D*)m_meshData.GetVertexAt(3) = { { right * 1.0f + up * 1.0f } };
			*(DebugVertex2D*)m_meshData.GetVertexAt(4) = { { right * 1.0f + up * 1.0f } };
			*(DebugVertex2D*)m_meshData.GetVertexAt(5) = { { right * 0.0f + up * 1.0f } };
			*(DebugVertex2D*)m_meshData.GetVertexAt(6) = { { right * 0.0f + up * 1.0f } };
			*(DebugVertex2D*)m_meshData.GetVertexAt(7) = { { right * 0.0f + up * 0.0f } };
		}
		
		m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matHash));

		{ // Create the mesh renderer.
			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(m_pObject);

			Graphics::CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CDebugRect::PreRender, this);
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

	void CDebugRect::PreRender()
	{
		{
			m_meshData.ResetInstances();
			size_t sz = m_drawQueue.size();
			for(size_t i = 0; i < sz; ++i)
			{
				auto& data = m_drawQueue.front();
				*(DebugInstance2D*)m_meshData.GetInstanceNext() = data.instance;
				data.time -= Util::CTimer::Instance().GetDelta();

				m_drawQueue.pop_front();
				if(data.time > 0.0f) m_drawQueue.push_back(data);
			}
		}

		static const u32 dataBufferHash = Math::FNV1a_32("DataBuffer");
		static const u32 screenSizeHash = Math::FNV1a_32("ScreenSize");
		static const u32 matrixBufferHash = Math::FNV1a_32("MatrixBuffer");
		static const u32 wvpHash = Math::FNV1a_32("WVP");
		
		Math::Vector4 screenSize(m_pGraphicsAPI->GetWidth(), m_pGraphicsAPI->GetHeight(), 1.0f / m_pGraphicsAPI->GetWidth(), 1.0f / m_pGraphicsAPI->GetHeight());
		m_pMaterial->SetFloat(dataBufferHash, screenSizeHash, &screenSize[0], 4);

		Math::SIMDMatrix mtx = Math::SIMD_MTX4X4_IDENTITY;
		m_pMaterial->SetFloat(matrixBufferHash, wvpHash, mtx.f32, 16);
	}

	void CDebugRect::Render()
	{
		// m_pMaterial->GetShader()->GetRootSignature()->Bind();
		Graphics::CMeshForceRender_ render(m_pMeshRenderer);
	}

	void CDebugRect::Release()
	{
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Draw methods
	//-----------------------------------------------------------------------------------------------

	void CDebugRect::Draw(const Math::Vector2& position, const Math::Vector2& scale, float rotation, const Math::Color& color, float time)
	{
		DebugDraw2D data;
		data.instance.world = Math::Matrix3x2::Scale(scale) * Math::Matrix3x2::Rotate(rotation) * Math::Matrix3x2::Translate(position);
		data.instance.color = color;
		data.time = time;

		Draw(data);
	}

	void CDebugRect::Draw(const Math::Matrix3x2& mtx, const Math::Color& color, float time)
	{
		DebugDraw2D data;
		data.instance.world = mtx;
		data.instance.color = color;
		data.time = time;

		Draw(data);
	}

	void CDebugRect::Draw(const DebugDraw2D& data)
	{
		m_drawQueue.push_back(data);
	}
};
