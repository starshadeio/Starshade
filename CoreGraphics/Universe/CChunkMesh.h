//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunkMesh.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCHUNKMESH_H
#define CCHUNKMESH_H

#include "CChunkData.h"
#include "../Graphics/CMeshData_.h"
#include "../Graphics/CMeshContainer_.h"
#include <Math/CMathVector2.h>
#include <Math/CMathVectorInt3.h>
#include <Globals/CGlobals.h>
#include <Utilities/CFuture.h>
#include <Objects/CVComponent.h>
#include <shared_mutex>
#include <unordered_set>
#include <unordered_map>

namespace Graphics
{
	class CMeshRenderer_;
	class CMaterial;
};

namespace Universe
{
	class CChunkMesh : CVComponent
	{
	private:
		struct Vertex
		{
			Math::Vector3 position;
			Math::Vector3 normal;
			Math::Vector3 tangent;
			Math::Vector4 texCoord;
		};

		typedef u32 Index;
		
	public:
		struct Data
		{
			u64 matHash;
			u64 matWireHash;
		};

	public:
		CChunkMesh(const CVObject* pObject);
		~CChunkMesh();
		CChunkMesh(const CChunkMesh&) = delete;
		CChunkMesh(CChunkMesh&&) = delete;
		CChunkMesh& operator = (const CChunkMesh&) = delete;
		CChunkMesh& operator = (CChunkMesh&&) = delete;

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		mutable std::shared_mutex m_mutex;
		
		bool m_bDirty;
		bool m_bAwaitingRebuild;
		bool m_bUpdateQueued;
		u8 m_meshIndex;

		Data m_data;
		
		Graphics::CMeshContainer_ m_meshContainer;
		Graphics::CMeshData_ m_meshData[2];
		Graphics::CMeshRenderer_* m_pMeshRendererList[2];
		Util::CFuture<void> m_meshFuture[2];
		Graphics::CMaterial* m_pMaterial;
		Graphics::CMaterial* m_pMaterialWire;
	};
};

#endif
