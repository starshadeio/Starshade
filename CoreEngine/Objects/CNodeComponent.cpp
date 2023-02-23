//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CNodeComponent.h
//
//-------------------------------------------------------------------------------------------------

#include "CNodeComponent.h"
#include "CNodeObject.h"
#include "../Application/CCoreManager.h"

CNodeComponent::CNodeComponent(u32 hash) :
	m_hash(hash)
{
}

CNodeComponent::~CNodeComponent()
{
}

void* CNodeComponent::AddToObject(CNodeObject* pObject)
{
	return AddToObject(pObject->GetHash());
}

void* CNodeComponent::GetFromObject(const CNodeObject* pObject)
{
	return GetFromObject(pObject->GetHash());
}

bool CNodeComponent::TryToGetFromObject(const CNodeObject* pObject, void** ppData)
{
	return TryToGetFromObject(pObject->GetHash(), ppData);
}

void CNodeComponent::RemoveFromObject(CNodeObject* pObject)
{
	RemoveFromObject(pObject->GetHash());
}
