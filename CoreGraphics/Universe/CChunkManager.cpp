//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunkManager.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CChunkManager.h"
#include "CChunkNode.h"
#include "../Application/CSceneManager.h"
#include "../Graphics/CMaterial.h"
#include "../Graphics/CShader.h"
#include "../Graphics/CRootSignature.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include <Application/CCommandManager.h>
#include <Utilities/CMemoryFree.h>
#include <Math/CMathFNV.h>

namespace Universe
{
	CChunkManager::CChunkManager() : 
		CVObject(L"Chunk Manager"),
		m_pMaterial(nullptr),
		m_pMaterialWire(nullptr)
	{
	}

	CChunkManager::~CChunkManager()
	{
	}
	
	void CChunkManager::Initialize()
	{
		SetViewHash(m_data.viewHash);

		m_pMaterial = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64("MATERIAL_VOXEL")));
		m_pMaterialWire = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64("MATERIAL_VOXELWIRE")));

		{ // Setup commands.
			App::CCommand::Properties props = { };
			props.bSaveEnabled = true;
			props.bUndoEnabled = true;

			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_BLOCK_EDIT, std::bind(&CChunkManager::BlockEdit, this, std::placeholders::_1, std::placeholders::_2), props);
		}
	}
	
	void CChunkManager::Update()
	{
		static const u32 frameBufferHash = Math::FNV1a_32("DrawBuffer");
		static const u32 viewHash = Math::FNV1a_32("View");
		static const u32 projHash = Math::FNV1a_32("Proj");
		static const u32 worldHash = Math::FNV1a_32("World");
		static const u32 texHash = Math::FNV1a_32("diffuse_texture");

		m_pMaterial->SetFloat(frameBufferHash, viewHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrixInv().f32, 16);
		m_pMaterial->SetFloat(frameBufferHash, projHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix().f32, 16);
		m_pMaterial->SetTexture(texHash, m_pTexture);
		m_pMaterial->SetFloat(frameBufferHash, worldHash, Math::SIMD_MTX4X4_IDENTITY.f32, 16);
		
		/*m_pMaterialWire->SetFloat(frameBufferHash, viewHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix().f32, 16);
		m_pMaterialWire->SetFloat(frameBufferHash, projHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix().f32, 16);
		//m_pMaterialWire->SetTexture(texHash, m_pTexture);
		m_pMaterialWire->SetFloat(frameBufferHash, worldHash, Math::SIMD_MTX4X4_IDENTITY.f32, 16);*/
	}
	
	void CChunkManager::LateUpdate()
	{
		while(!m_updateQueue.empty())
		{
			m_updateQueue.front()->LateUpdate();
			m_updateQueue.pop();
		}

		for(auto& node : m_chunkMap)
		{
			node.first->FrustumCulling();
		}
	}

	void CChunkManager::Render(u32 viewHash)
	{
		// It's assumed that chunks will only exist in a single view, so only the first chunk needs to be checked for a node match.
		if(m_chunkMap.empty() || m_chunkMap.begin()->first->GetViewHash() != viewHash) return;

		m_pMaterial->GetShader()->GetRootSignature()->Bind();

		while(!m_renderQueue.empty())
		{
			m_renderQueue.front()->ForceRender(0);
			m_renderQueue.pop();
		}
	}

	void CChunkManager::Release()
	{
		for(auto& node : m_chunkMap)
		{
			for(auto& chunk : node.second)
			{
				SAFE_RELEASE_DELETE(chunk.second);
			}
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Chunk methods.
	//-----------------------------------------------------------------------------------------------

	CChunk* CChunkManager::RegisterChunk(const CChunkNode* pChunkNode, const Math::VectorInt3& chunkCoord, const CChunk::Data& data, bool bInitIfNotFound)
	{
		auto node = m_chunkMap.insert({ pChunkNode, { } }).first;
		auto chunk = node->second.find(chunkCoord);

		assert(chunk == node->second.end());

		if(chunk == node->second.end())
		{
			CChunk* pChunk = new CChunk(this);
			pChunk->SetData(data);
			pChunk->SetChunkCoord(chunkCoord);

			if(bInitIfNotFound)
			{
				pChunk->Initialize();
			}

			node->second.insert({ chunkCoord, pChunk });
			return pChunk;
		}

		return nullptr;
	}

	bool CChunkManager::DeregisterChunk(const CChunkNode* pChunkNode, const Math::VectorInt3& chunkCoord)
	{
		auto node = m_chunkMap.find(pChunkNode);
		if(node == m_chunkMap.end()) return false;

		auto chunk = node->second.find(chunkCoord);
		if(chunk != node->second.end())
		{
			auto pChunk = chunk->second;
			node->second.erase(chunk);
			SAFE_RELEASE_DELETE(pChunk);
			return true;
		}

		return false;
	}

	bool CChunkManager::ClearNode(const class CChunkNode* pChunkNode)
	{
		auto node = m_chunkMap.find(pChunkNode);
		if(node == m_chunkMap.end()) return false;

		for(auto& chunk : node->second)
		{
			auto pChunk = chunk.second;
			SAFE_RELEASE_DELETE(pChunk);
		}

		m_chunkMap.erase(node);
		return true;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Prototype methods.
	//-----------------------------------------------------------------------------------------------

	u8 CChunkManager::LODDown(const class CChunkNode* pChunkNode)
	{
		auto node = m_chunkMap.find(pChunkNode);
		if(node == m_chunkMap.end()) return 0;
		if(node->second.empty()) return 0;

		u8 lodLevel = node->second.begin()->second->GetLODLevel();
		if(lodLevel == 0) return lodLevel;
		
		for(auto& chunk : node->second)
		{
			chunk.second->SetLODLevel(lodLevel - 1);
		}
		
		for(auto& chunk : node->second)
		{
			chunk.second->ForceRebuild();
		}
		
		return node->second.begin()->second->GetLODLevel();
	}

	u8 CChunkManager::LODUp(const class CChunkNode* pChunkNode)
	{
		auto node = m_chunkMap.find(pChunkNode);
		if(node == m_chunkMap.end()) return 0;
		if(node->second.empty()) return 0;
		
		u8 lodLevel = node->second.begin()->second->GetLODLevel();
		if(lodLevel == node->second.begin()->second->GetLODLevelMax()) return lodLevel;
		
		for(auto& chunk : node->second)
		{
			chunk.second->SetLODLevel(lodLevel + 1);
		}
		
		for(auto& chunk : node->second)
		{
			chunk.second->ForceRebuild();
		}

		return node->second.begin()->second->GetLODLevel();
	}

	//-----------------------------------------------------------------------------------------------
	// Chunk methods.
	//-----------------------------------------------------------------------------------------------

	bool CChunkManager::BlockEdit(const void* param, bool bInverse)
	{
		ChunkBlockUpdateData data = *(ChunkBlockUpdateData*)param;
		
		if(!bInverse)
		{
			for(size_t i = 0; i < data.coordCount; ++i)
			{
				CChunk* pChunk = GetChunk(data.pChunkNode, data.coordList[i].chunkCoord);

				if(pChunk == nullptr)
				{
					pChunk = data.pChunkNode->CreateChunk(data.coordList[i].chunkCoord);
				}

				const u16 lastId = pChunk->UpdateBlock(data.coordList[i].blockIndex, data.coordList[i].block.bFilled ? data.coordList[i].block.id : 256);
				data.coordList[i].block.bFilled = lastId <= 0xFF;
				if(data.coordList[i].block.bFilled) data.coordList[i].block.id = static_cast<u8>(lastId);
			}
			
			App::CCommandManager::Instance().RecordUndo(&data, data.GetSize(), alignof(ChunkBlockUpdateData));
		}
		else
		{
			for(size_t i = 0; i < data.coordCount; ++i)
			{
				CChunk* pChunk = GetChunk(data.pChunkNode, data.coordList[i].chunkCoord);

				if(pChunk == nullptr)
				{
					pChunk = data.pChunkNode->CreateChunk(data.coordList[i].chunkCoord);
				}

				pChunk->UpdateBlock(data.coordList[i].blockIndex, data.coordList[i].block.bFilled ? data.coordList[i].block.id : 256);
			}
		}

		return true;
	}

	CChunk* CChunkManager::CreateChunk(const CChunkNode* pChunkNode, const Math::VectorInt3& chunkCoord, bool bBuild)
	{
		CChunk::Data data { };
		data.offset = Math::VectorInt3(chunkCoord.x * pChunkNode->GetBlockCountX(), chunkCoord.y * pChunkNode->GetBlockCountY(), chunkCoord.z * pChunkNode->GetBlockCountZ());
		data.width = pChunkNode->GetBlockCountX();
		data.height = pChunkNode->GetBlockCountY();
		data.length = pChunkNode->GetBlockCountZ();
		data.blockSize = pChunkNode->GetBlockSize();

		data.matHash = Math::FNV1a_64("MATERIAL_VOXEL");;
		data.matWireHash = Math::FNV1a_64("MATERIAL_VOXELWIRE");;

		CChunk* pChunk = RegisterChunk(pChunkNode, chunkCoord, data, false);
		pChunk->Setup();

		if(bBuild)
		{
			pChunk->RebuildMesh();
		}

		auto SetAdjacent = [&pChunk](CChunk* pChunkAdj, SIDE side, SIDE sideInv){
			if(pChunkAdj)
			{
				pChunk->SetAdjacentChunk(side, pChunkAdj);
				pChunkAdj->SetAdjacentChunk(sideInv, pChunk);
			}
		};

		SetAdjacent(GetChunk(pChunkNode, chunkCoord + Math::VectorInt3(-1,  0,  0)), SIDE_LEFT, SIDE_RIGHT);
		SetAdjacent(GetChunk(pChunkNode, chunkCoord + Math::VectorInt3( 1,  0,  0)), SIDE_RIGHT, SIDE_LEFT);
		SetAdjacent(GetChunk(pChunkNode, chunkCoord + Math::VectorInt3( 0, -1,  0)), SIDE_BOTTOM, SIDE_TOP);
		SetAdjacent(GetChunk(pChunkNode, chunkCoord + Math::VectorInt3( 0,  1,  0)), SIDE_TOP, SIDE_BOTTOM);
		SetAdjacent(GetChunk(pChunkNode, chunkCoord + Math::VectorInt3( 0,  0, -1)), SIDE_BACK, SIDE_FRONT);
		SetAdjacent(GetChunk(pChunkNode, chunkCoord + Math::VectorInt3( 0,  0,  1)), SIDE_FRONT, SIDE_BACK);

		return pChunk;
	}
};
