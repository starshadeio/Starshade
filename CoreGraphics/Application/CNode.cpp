//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CNode.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CNode.h"
#include "CNodeManager.h"
#include "CSceneManager.h"
#include "../Universe/CChunkNode.h"
#include <Math/CMathFNV.h>

namespace App
{
	CNode::CNode(const wchar_t* pName, u32 viewHash) :
		m_viewHash(viewHash),
		m_pName(pName),
		m_hash(Math::FNV1a_64(pName)),
		m_chunkNode(L"Chunk Node", viewHash),
		m_pSelectedObject(nullptr)
	{
		CSceneManager::Instance().NodeManager().RegisterNode(this);
	}

	CNode::~CNode()
	{
	}
	
	void CNode::Initialize()
	{
		{
			Universe::CChunkNode::Data data { };
			data.blockSize = 0.25f;
			data.chunkWidth = 32;
			data.chunkHeight = 32;
			data.chunkLength = 32;
			data.chunksX = 4;
			data.chunksY = 4;
			data.chunksZ = 4;
			m_chunkNode.SetData(data);
			m_chunkNode.Initialize();
		}
	}
	
	void CNode::Release()
	{
		m_chunkNode.Release();
	}
	
	//-----------------------------------------------------------------------------------------------
	// File methods.
	//-----------------------------------------------------------------------------------------------

	void CNode::Load(std::ifstream& file)
	{
		m_chunkNode.Load(file);
		
		while(!file.eof())
		{
			u64 hash;
			file.read(reinterpret_cast<char*>(&hash), sizeof(hash));
			if(file)
			{
				auto elem = m_objMap.find(hash);
				if(elem != m_objMap.end())
				{
					dynamic_cast<CNodeObject*>(elem->second)->Load(file);
				}
			}
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Object methods.
	//-----------------------------------------------------------------------------------------------

	CNodeObject* CNode::CreateNodeObject(const wchar_t* name)
	{
		return new CNodeObject(Math::FNV1a_64(name), m_viewHash);
	}

	u64 CNode::AddObject(const wchar_t* name, bool bForceSelect)
	{
		CNodeObject* pObject = CreateNodeObject(name);
		
		m_objMap.insert({pObject->GetHash(), pObject }).first->second;
		if(bForceSelect) m_pSelectedObject = pObject;

		return pObject->GetHash();
	}
	
	bool CNode::SelectObject(u64 hash)
	{
		auto elem = m_objMap.find(hash);
		if(elem != m_objMap.end())
		{
			m_pSelectedObject = elem->second;
			return true;
		}

		return false;
	}
};
