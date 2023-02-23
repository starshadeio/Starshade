//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CNodeManager.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CNodeManager.h"
#include "CNode.h"

namespace App
{
	CNodeManager::CNodeManager()
	{
	}

	CNodeManager::~CNodeManager()
	{
	}
		
	void CNodeManager::Initialize()
	{
		for(auto& pNode : m_loadedList)
		{
			pNode->Initialize();
		}
	}

	void CNodeManager::PostInitialize()
	{
		for(auto& pNode : m_loadedList)
		{
			pNode->PostInitialize();
		}
	}

	void CNodeManager::Update()
	{
		for(auto& pNode : m_loadedList)
		{
			pNode->Update();
		}
	}
	
	void CNodeManager::LateUpdate()
	{
		for(auto& pNode : m_loadedList)
		{
			pNode->LateUpdate();
		}
	}

	void CNodeManager::Release()
	{
		for(auto& elem : m_nodeMap)
		{
			elem.second->Release();
		}
	}

	void CNodeManager::OnMove(const Math::Rect& rect)
	{
		for(auto& pNode : m_loadedList)
		{
			pNode->OnMove(rect);
		}
	}

	void CNodeManager::OnResize(const Math::Rect& rect)
	{
		for(auto& pNode : m_loadedList)
		{
			pNode->OnResize(rect);
		}
	}

	//-----------------------------------------------------------------------------------------------
	// (De)registration methods.
	//-----------------------------------------------------------------------------------------------

	void CNodeManager::RegisterNode(CNode* pNode)
	{
		m_nodeMap.insert({ pNode->GetHash(), pNode });
		m_loadedList.push_back(pNode);
	}

	void CNodeManager::DeregisterNode(CNode* pNode)
	{
		m_nodeMap.erase(pNode->GetHash());

		size_t j = 0;
		for(size_t i = 0; i < m_loadedList.size(); ++i)
		{
			if(m_loadedList[i] != pNode)
			{
				m_loadedList[j++] = m_loadedList[i];
			}
		}

		m_loadedList.resize(j);
	}
};
