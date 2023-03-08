//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunkMesh.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CChunkMesh.h"

namespace Universe
{
	CChunkMesh::CChunkMesh(const CVObject* pObject) :
		CVComponent(pObject),
		m_bDirty(false),
		m_bAwaitingRebuild(false),
		m_bUpdateQueued(false),
		m_meshIndex(0),
		m_meshData{ pObject, pObject },
		m_meshContainer(pObject),
		m_pMeshRendererList{ nullptr, nullptr },
		m_pMaterial(nullptr),
		m_pMaterialWire(nullptr)
	{
	}

	CChunkMesh::~CChunkMesh()
	{
	}
};
