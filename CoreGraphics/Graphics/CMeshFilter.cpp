//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshFilter.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMeshFilter.h"
#include <Application/CCoreManager.h>
#include <Objects/CNodeObject.h>
#include <Utilities/CMemoryFree.h>

namespace Graphics
{
	//-----------------------------------------------------------------------------------------------
	// CMeshFilter::Data methods.
	//-----------------------------------------------------------------------------------------------
	
	CMeshFilter::Data::Data(u64 thisHash) :
		m_bInstanceDirty(false),
		m_instanceMax(0),
		m_instanceStride(0),
		m_instanceSize(0),
		m_instanceCount(0),
		m_this(thisHash),
		m_pInstanceList(nullptr),
		m_pMesh(nullptr)
	{
	}

	CMeshFilter::Data::~Data()
	{
	}

	void CMeshFilter::Data::Initialize()
	{
		m_instanceSize = m_instanceMax * m_instanceStride;
		if(m_instanceSize)
		{
			m_pInstanceList = new u8[m_instanceSize];
		}
	}

	void CMeshFilter::Data::Release()
	{
		SAFE_DELETE_ARRAY(m_pInstanceList);
	}

	//-----------------------------------------------------------------------------------------------
	// CMeshFilter methods.
	//-----------------------------------------------------------------------------------------------
	
	CMeshFilter::CMeshFilter() : 
		CNodeComponent(HASH)
	{
	}

	CMeshFilter::~CMeshFilter()
	{
	}
	
	void CMeshFilter::Initialize()
	{
		for(auto& elem : m_dataMap)
		{
			elem.second.Initialize();
		}
	}

	void CMeshFilter::Release()
	{
		for(auto& elem : m_dataMap)
		{
			elem.second.Release();
		}
	}
	
	void CMeshFilter::Register()
	{
		App::CCoreManager::Instance().NodeRegistry().RegisterComponent<CMeshFilter>();
	}
	
	//---------------------------------------------
	// Object methods.
	//---------------------------------------------

	void* CMeshFilter::AddToObject(u64 objHash)
	{
		return &m_dataMap.insert({ objHash, Data(objHash) }).first->second;
	}

	void CMeshFilter::RemoveFromObject(u64 objHash)
	{
		m_dataMap.erase(objHash);
	}
	
	void* CMeshFilter::GetFromObject(u64 objHash)
	{
		return &m_dataMap.find(objHash)->second;
	}

	bool CMeshFilter::TryToGetFromObject(u64 objHash, void** ppData)
	{
		auto elem = m_dataMap.find(objHash);
		if(elem == m_dataMap.end()) return false;
		*ppData = &elem->second;
		return true;
	}
};
