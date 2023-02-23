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
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include <Application/CCommandManager.h>
#include <Utilities/CMemoryFree.h>
#include <Math/CMathFNV.h>

namespace Universe
{
	CChunkManager::CChunkManager() : 
		CVObject(L"Chunk Manager"/*, Math::FNV1a_32(L"Main")*/),
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
		static const u32 vpHash = Math::FNV1a_32("VP");
		static const u32 worldHash = Math::FNV1a_32("World");
		static const u32 texHash = Math::FNV1a_32("diffuse_texture");

		Math::SIMDMatrix mtx = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix();
		mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix();
		
		m_pMaterial->SetFloat(frameBufferHash, vpHash, mtx.f32, 16);
		m_pMaterial->SetTexture(texHash, m_pTexture);
		m_pMaterial->SetFloat(frameBufferHash, worldHash, Math::SIMD_MTX4X4_IDENTITY.f32, 16);
		
		/*m_pMaterialWire->SetFloat(frameBufferHash, vpHash, mtx.f32, 16);
		//m_pMaterialWire->SetTexture(texHash, m_pTexture);
		m_pMaterialWire->SetFloat(frameBufferHash, worldHash, Math::SIMD_MTX4X4_IDENTITY.f32, 16);*/
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

	CChunk* CChunkManager::RegisterChunk(const CChunkNode* pChunkNode, const Math::VectorInt3& chunkCoord, const CChunk::Data& data)
	{
		auto node = m_chunkMap.insert({ pChunkNode, { } }).first;
		auto chunk = node->second.find(chunkCoord);

		assert(chunk == node->second.end());

		if(chunk == node->second.end())
		{
			CChunk* pChunk = new CChunk(this);
			pChunk->SetData(data);
			pChunk->Initialize();
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
				const u16 lastId = pChunk->UpdateBlock(data.coordList[i].blockIndex, data.coordList[i].block.bEmpty ? 256 : data.coordList[i].block.id);
				data.coordList[i].block.bEmpty = lastId > 0xFF;
				if(!data.coordList[i].block.bEmpty) data.coordList[i].block.id = static_cast<u8>(lastId);
			}
			
			App::CCommandManager::Instance().RecordUndo(&data, data.GetSize(), alignof(ChunkBlockUpdateData));
		}
		else
		{
			for(size_t i = 0; i < data.coordCount; ++i)
			{
				CChunk* pChunk = GetChunk(data.pChunkNode, data.coordList[i].chunkCoord);
				pChunk->UpdateBlock(data.coordList[i].blockIndex, data.coordList[i].block.bEmpty ? 256 : data.coordList[i].block.id);
			}
		}

		return true;
	}
};
