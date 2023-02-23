//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CVObject.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CVObject.h"
#include "../Logic/CTransform.h"

#ifdef _DEBUG
	std::unordered_set<u64> CVObject::m_idSet;
#endif

CVObject::CVObject(const std::wstring& name, u32 viewHash, u32 layer, u32 tagHash) : 
	m_layer(layer),
	m_tagHash(tagHash),
	m_hash(Math::FNV1a_64(name.c_str(), name.size())),
	m_viewHash(viewHash),
	m_bHasPhysics(false),
	m_pTransform(nullptr)
{
#ifdef _DEBUG
	assert(m_idSet.insert(m_hash).second);
#endif
}

CVObject::CVObject(u64 hash, u32 viewHash, u32 layer, u32 tagHash) : 
	m_layer(layer),
	m_tagHash(tagHash),
	m_hash(hash),
	m_viewHash(viewHash),
	m_bHasPhysics(false),
	m_pTransform(nullptr)
{
#ifdef _DEBUG
	assert(m_idSet.insert(m_hash).second);
#endif
}

CVObject::~CVObject() { }

void CVObject::AddComponent(CVComponent* pComponent) 
{
	std::lock_guard<std::shared_mutex> lk(m_mutex);

	Logic::CTransform* pTransform = dynamic_cast<Logic::CTransform*>(pComponent);
	if(pTransform) { m_pTransform = pTransform; }

	m_componentList.push_back(pComponent);
}

void CVObject::RemoveComponent(CVComponent* pComponent) 
{
	std::lock_guard<std::shared_mutex> lk(m_mutex);

	size_t j = 0;
	for(size_t i = 0; i < m_componentList.size(); ++i)
	{
		if(m_componentList[i] != pComponent) 
		{
			m_componentList[j++] = m_componentList[i];
		}
	}

	if(j < m_componentList.size())
	{
		m_componentList.resize(j);
	}

	Logic::CTransform* pTransform = dynamic_cast<Logic::CTransform*>(pComponent);
	if(pTransform == m_pTransform) { m_pTransform = nullptr; }
}