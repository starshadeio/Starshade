//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CNodeRegistry.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CNodeRegistry.h"
#include "CNodeObject.h"
#include <functional>

CNodeRegistry::CNodeRegistry()
{
}

CNodeRegistry::~CNodeRegistry()
{
}

void CNodeRegistry::Initialize()
{
	for(auto elem : m_compList)
	{
		elem->Initialize();
	}
}

void CNodeRegistry::PostInitialize()
{
	for(auto elem : m_compList)
	{
		elem->PostInitialize();
	}
}

void CNodeRegistry::Update()
{
	for(auto elem : m_updateList)
	{
		elem->Update();
	}
}

void CNodeRegistry::LateUpdate()
{
	for(auto elem : m_lateUpdateList)
	{
		elem->LateUpdate();
	}
}

void CNodeRegistry::Release()
{
	for(auto elem : m_compList)
	{
		elem->Release();
	}
}

void CNodeRegistry::OnMove(const Math::Rect& rect)
{
	for(auto elem : m_compList)
	{
		elem->OnMove(rect);
	}
}

void CNodeRegistry::OnResize(const Math::Rect& rect)
{
	for(auto elem : m_compList)
	{
		elem->OnResize(rect);
	}
}

//-------------------------------------------------------------------------------------------------
// Object methods.
//-------------------------------------------------------------------------------------------------

// pObject
void* CNodeRegistry::AddComponentToObject(u32 compHash, CNodeObject* pObject)
{
	return m_compMap.find(compHash)->second->AddToObject(pObject);
}

void* CNodeRegistry::GetComponentFromObject(u32 compHash, const CNodeObject* pObject)
{
	return m_compMap.find(compHash)->second->GetFromObject(pObject);
}

bool CNodeRegistry::TryToGetComponentFromObject(u32 compHash, const CNodeObject* pObject, void** ppData)
{
	return m_compMap.find(compHash)->second->TryToGetFromObject(pObject, ppData);
}

void CNodeRegistry::RemoveComponentFromObject(u32 compHash, CNodeObject* pObject)
{
	m_compMap.find(compHash)->second->RemoveFromObject(pObject);
}

// pObject::HASH
void* CNodeRegistry::AddComponentToObject(u32 compHash, u64 objHash)
{
	return m_compMap.find(compHash)->second->AddToObject(objHash);
}

void* CNodeRegistry::GetComponentFromObject(u32 compHash, u64 objHash)
{
	return m_compMap.find(compHash)->second->GetFromObject(objHash);
}

bool CNodeRegistry::TryToGetComponentFromObject(u32 compHash, u64 objHash, void** ppData)
{
	return m_compMap.find(compHash)->second->TryToGetFromObject(objHash, ppData);
}

void CNodeRegistry::RemoveComponentFromObject(u32 compHash, u64 objHash)
{
	m_compMap.find(compHash)->second->RemoveFromObject(objHash);
}
