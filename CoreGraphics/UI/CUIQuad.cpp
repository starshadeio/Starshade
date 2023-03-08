//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIQuad.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIQuad.h"
#include "CUIImage.h"
#include "CUIText.h"
#include "CFont.h"
#include "../Graphics/CMeshRenderer_.h"
#include "../Graphics/CMaterial.h"
#include "../Graphics/CShader.h"
#include "../Graphics/CRootSignature.h"
#include "../Graphics/CTexture.h"
#include "../Graphics/CGraphicsAPI.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include "../Application/CSceneManager.h"
#include <Utilities/CMemoryFree.h>
#include <Math/CMathFNV.h>

namespace UI
{
	CUIQuad::CUIQuad(const CVObject* pObject) : 
		CVComponent(pObject),
		m_bForceReset(true),
		m_instanceMax(0),
		m_meshData(pObject),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr),
		m_pTexture(nullptr),
		m_pGraphicsAPI(nullptr)
	{
	}

	CUIQuad::~CUIQuad()
	{
	}

	void CUIQuad::Initialize()
	{
		{ // Create the mesh data.
			Graphics::CMeshData_::Data data { };
			data.topology = Graphics::PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.vertexCount = 4;
			data.instanceMax = m_instanceMax;
			data.indexCount = 6;
			data.vertexStride = sizeof(Vertex);
			data.instanceStride = sizeof(Instance);
			data.indexStride = sizeof(Index);

			m_meshData.SetData(data);
			m_meshData.Initialize();
			m_meshData.SetInstanceCount(0);

			static const float halfSize = 0.5f;
			static const Math::Vector2 right(1.0f, 0.0f);
			static const Math::Vector2 up(0.0f, 1.0f);
			
			*(Vertex*)m_meshData.GetVertexAt(0) = { { right * 0.0f + up * 0.0f }, { 0.0f, 1.0f } };
			*(Vertex*)m_meshData.GetVertexAt(1) = { { right * 1.0f + up * 0.0f }, { 1.0f, 1.0f } };
			*(Vertex*)m_meshData.GetVertexAt(2) = { { right * 1.0f + up * 1.0f }, { 1.0f, 0.0f } };
			*(Vertex*)m_meshData.GetVertexAt(3) = { { right * 0.0f + up * 1.0f }, { 0.0f, 0.0f } };

			*(Index*)m_meshData.GetIndexAt(0) = 0;
			*(Index*)m_meshData.GetIndexAt(1) = 1;
			*(Index*)m_meshData.GetIndexAt(2) = 2;
			*(Index*)m_meshData.GetIndexAt(3) = 0;
			*(Index*)m_meshData.GetIndexAt(4) = 2;
			*(Index*)m_meshData.GetIndexAt(5) = 3;
		}
		
		m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, m_data.matHash));

		{ // Create the mesh renderer.
			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(m_pObject);

			Graphics::CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CUIQuad::PreRender, this);
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

	void CUIQuad::PreRender()
	{
		if(m_bForceReset)
		{
			m_meshData.ResetInstances();
			for(auto pRenderer : m_rendererList)
			{
				pRenderer->ResetInstanceList();
				pRenderer->DrawSetup();
			}

			m_dirtyQueue.clear();
			m_bForceReset = false;
		}
		else if(!m_dirtyQueue.empty())
		{
			m_meshData.ForceInstancesDirty();
			while(!m_dirtyQueue.empty())
			{
				m_dirtyQueue.front()->DrawSetup();
				m_dirtyQueue.pop_front();
			}
		}

		static const u32 textureHash = Math::FNV1a_32("g_texture");
		static const u32 dataBufferHash = Math::FNV1a_32("DataBuffer");
		static const u32 screenSizeHash = Math::FNV1a_32("ScreenSize");
		static const u32 maxtrixBufferHash = Math::FNV1a_32("MatrixBuffer");
		static const u32 wvpHash = Math::FNV1a_32("WVP");
		
		Math::Vector4 screenSize(m_pGraphicsAPI->GetWidth(), m_pGraphicsAPI->GetHeight(), 1.0f / m_pGraphicsAPI->GetWidth(), 1.0f / m_pGraphicsAPI->GetHeight());
		m_pMaterial->SetFloat(dataBufferHash, screenSizeHash, &screenSize[0], 4);

		Math::SIMDMatrix mtx = Math::SIMD_MTX4X4_IDENTITY;
		m_pMaterial->SetFloat(maxtrixBufferHash, wvpHash, mtx.f32, 16);
		
		m_pMaterial->SetTexture(textureHash, m_pTexture);
	}
	
	void CUIQuad::Render()
	{
		m_pMaterial->GetShader()->GetRootSignature()->Bind();
		Graphics::CMeshForceRender_ render(m_pMeshRenderer);
	}

	void CUIQuad::Release()
	{
		m_rendererList.clear();
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Registration methods.
	//-----------------------------------------------------------------------------------------------
		
	void CUIQuad::RegisterRenderer(CUIRenderer* pRenderer)
	{
		m_rendererList.push_back(pRenderer);
		m_instanceMax += pRenderer->GetInstanceMax();

		if(m_pTexture == nullptr)
		{
			m_pTexture = reinterpret_cast<Graphics::CTexture*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_TEXTURE, pRenderer->GetTextureHash()));
		}
	}

	void CUIQuad::RegisterText(CUIText* pText)
	{
		m_rendererList.push_back(pText);
		m_instanceMax += pText->GetInstanceMax();

		if(m_pTexture == nullptr)
		{
			m_pTexture = reinterpret_cast<CFont*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_FONT, pText->GetTextureHash()))->GetTexture();
		}
	}
	
	void CUIQuad::AddDirty(class CUIRenderer* pRenderer)
	{
		m_dirtyQueue.push_back(pRenderer);
	}

	CUIQuad::Instance* CUIQuad::PullInstance()
	{
		assert(m_meshData.GetInstanceCount() < m_meshData.GetInstanceMax());
		return reinterpret_cast<Instance*>(m_meshData.GetInstanceNext());
	}
};
