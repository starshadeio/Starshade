//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CCyberNode.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CCyberNode.h"
#include "../Application/CAppData.h"
#include "../Application/CNodeSelect.h"
#include <Graphics/CMeshRenderer_.h>
#include <Graphics/CMaterial.h>
#include <Factory/CFactory.h>
#include <Resources/CResourceManager.h>
#include <Application/CSceneManager.h>
#include <Utilities/CMemoryFree.h>
#include <Math/CMathFNV.h>

namespace Universe
{
	CCyberNode::CCyberNode(const wchar_t* pName, u32 viewHash) :
		CVObject(pName, viewHash),
		m_transform(this),
		m_transformInternal(nullptr),
		m_callback(this),
		m_volume(this),
		m_meshData(this),
		m_pMeshRenderer(nullptr),
		m_pMaterial(nullptr) {
	}

	CCyberNode::~CCyberNode() { }

	void CCyberNode::Initialize()
	{
		{ // Create the mesh data.
			struct Vertex
			{
				Math::Vector3 position;
			};

			Graphics::CMeshData_::Data data { };
			data.topology = Graphics::PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.vertexCount = 8;
			data.vertexStride = sizeof(Vertex);
			data.indexCount = 36;
			data.indexStride = sizeof(u16);

			m_meshData.SetData(data);
			m_meshData.Initialize();

			Math::Vector3 hs = m_data.size * 0.5f;

			u32 index = 0;
			*(Vertex*)m_meshData.GetVertexAt(index++) = { { -hs.x, -hs.y, -hs.z } };
			*(Vertex*)m_meshData.GetVertexAt(index++) = { {  hs.x, -hs.y, -hs.z } };
			*(Vertex*)m_meshData.GetVertexAt(index++) = { {  hs.x, -hs.y,  hs.z } };
			*(Vertex*)m_meshData.GetVertexAt(index++) = { { -hs.x, -hs.y,  hs.z } };
			*(Vertex*)m_meshData.GetVertexAt(index++) = { { -hs.x,  hs.y, -hs.z } };
			*(Vertex*)m_meshData.GetVertexAt(index++) = { {  hs.x,  hs.y, -hs.z } };
			*(Vertex*)m_meshData.GetVertexAt(index++) = { {  hs.x,  hs.y,  hs.z } };
			*(Vertex*)m_meshData.GetVertexAt(index++) = { { -hs.x,  hs.y,  hs.z } };

			index = 0;

			// Left.
			*(u16*)m_meshData.GetIndexAt(index++) = 3;
			*(u16*)m_meshData.GetIndexAt(index++) = 0;
			*(u16*)m_meshData.GetIndexAt(index++) = 4;
			*(u16*)m_meshData.GetIndexAt(index++) = 3;
			*(u16*)m_meshData.GetIndexAt(index++) = 4;
			*(u16*)m_meshData.GetIndexAt(index++) = 7;

			// Right.
			*(u16*)m_meshData.GetIndexAt(index++) = 1;
			*(u16*)m_meshData.GetIndexAt(index++) = 2;
			*(u16*)m_meshData.GetIndexAt(index++) = 6;
			*(u16*)m_meshData.GetIndexAt(index++) = 1;
			*(u16*)m_meshData.GetIndexAt(index++) = 6;
			*(u16*)m_meshData.GetIndexAt(index++) = 5;

			// Bottom.
			*(u16*)m_meshData.GetIndexAt(index++) = 0;
			*(u16*)m_meshData.GetIndexAt(index++) = 3;
			*(u16*)m_meshData.GetIndexAt(index++) = 2;
			*(u16*)m_meshData.GetIndexAt(index++) = 0;
			*(u16*)m_meshData.GetIndexAt(index++) = 2;
			*(u16*)m_meshData.GetIndexAt(index++) = 1;

			// Top.
			*(u16*)m_meshData.GetIndexAt(index++) = 4;
			*(u16*)m_meshData.GetIndexAt(index++) = 5;
			*(u16*)m_meshData.GetIndexAt(index++) = 6;
			*(u16*)m_meshData.GetIndexAt(index++) = 4;
			*(u16*)m_meshData.GetIndexAt(index++) = 6;
			*(u16*)m_meshData.GetIndexAt(index++) = 7;

			// Back.
			*(u16*)m_meshData.GetIndexAt(index++) = 0;
			*(u16*)m_meshData.GetIndexAt(index++) = 1;
			*(u16*)m_meshData.GetIndexAt(index++) = 5;
			*(u16*)m_meshData.GetIndexAt(index++) = 0;
			*(u16*)m_meshData.GetIndexAt(index++) = 5;
			*(u16*)m_meshData.GetIndexAt(index++) = 4;

			// Front.
			*(u16*)m_meshData.GetIndexAt(index++) = 3;
			*(u16*)m_meshData.GetIndexAt(index++) = 6;
			*(u16*)m_meshData.GetIndexAt(index++) = 2;
			*(u16*)m_meshData.GetIndexAt(index++) = 3;
			*(u16*)m_meshData.GetIndexAt(index++) = 7;
			*(u16*)m_meshData.GetIndexAt(index++) = 6;
		}

		{ // Create the mesh renderer.
			m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64("MATERIAL_CYBER_NODE")));

			m_pMeshRenderer = CFactory::Instance().CreateMeshRenderer(this);

			Graphics::CMeshRenderer_::Data data { };
			data.onPreRender = std::bind(&CCyberNode::PreRender, this);
			data.pMeshData = &m_meshData;
			m_pMeshRenderer->SetData(data);
			m_pMeshRenderer->AddMaterial(m_pMaterial);

			m_pMeshRenderer->Initialize();
		}

		m_transformInternal.SetParent(&m_transform);

		{ // Create collider.
			Physics::CVolumeOBB::Data data { };
			data.halfSize = 0.5f;
			data.bAllowRays = true;

			m_volume.SetData(data);
			m_volume.Register(m_transform.GetWorldMatrix());
		}

		{ // Setup the logic callback(s).
			Logic::CCallback::Data data { };
			data.callbackMap.insert({ Logic::CALLBACK_INTERACT, std::bind(&CCyberNode::InteractCallback, this, std::placeholders::_1) });
			m_callback.SetData(data);
		}
	}

	void CCyberNode::PreRender()
	{
		static const u32 maxtrixBufferHash = Math::FNV1a_32("DrawBuffer");
		static const u32 wvpHash = Math::FNV1a_32("WVP");

		Math::SIMDMatrix mtx = m_transformInternal.GetWorldMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix();

		m_pMaterial->SetFloat(maxtrixBufferHash, wvpHash, mtx.f32, 16);
	}

	void CCyberNode::Release()
	{
		m_volume.Deregister();
		SAFE_RELEASE_DELETE(m_pMeshRenderer);
		m_meshData.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Callbacks.
	//-----------------------------------------------------------------------------------------------

	bool CCyberNode::InteractCallback(void* pVal)
	{
		App::CNodeSelect::PickedInfo* pInfo = reinterpret_cast<App::CNodeSelect::PickedInfo*>(pVal);
		m_transformInternal.SetScale(pInfo->val == 1 ? 0.9f : 1.0f);
		if(pInfo->val == 2)
		{
			return App::CSceneManager::Instance().LoadScene(App::SCENE_HASH_NODE);
		}

		return false;
	}
};
