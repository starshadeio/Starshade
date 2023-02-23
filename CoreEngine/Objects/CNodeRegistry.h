//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CNodeRegistry.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODEREGISTRY_H
#define CNODEREGISTRY_H

#include "CNodeComponent.h"
#include "../Globals/CGlobals.h"
#include "../Math/CMathRect.h"
#include <vector>
#include <unordered_map>

class CNodeRegistry
{
public:
	CNodeRegistry();
	~CNodeRegistry();
	CNodeRegistry(const CNodeRegistry&) = delete;
	CNodeRegistry(CNodeRegistry&&) = delete;
	CNodeRegistry& operator = (const CNodeRegistry&) = delete;
	CNodeRegistry& operator = (CNodeRegistry&&) = delete;

	void Initialize();
	void PostInitialize();
	void Update();
	void LateUpdate();
	void Release();

	void OnMove(const Math::Rect& rect);
	void OnResize(const Math::Rect& rect);

	//-----------------------------------------------------------------------------------------------

	template<typename T>
	void RegisterComponent()
	{
		auto elem = m_compMap.find(T::HASH);
		if(elem == m_compMap.end())
		{
			T* pComp = dynamic_cast<T*>(&T::Get());
			m_compMap.insert({ T::HASH, pComp });
			m_compList.push_back(pComp);

			// Have separate lists for per-frame method calls only if they are used.
			if(typeid(&CNodeComponent::Update) != typeid(&T::Update)) m_updateList.push_back(pComp);
			if(typeid(&CNodeComponent::LateUpdate) != typeid(&T::LateUpdate)) m_lateUpdateList.push_back(pComp);
		}
	}
	
	template<typename T>
	void DeregisterComponent()
	{
		auto elem = m_compMap.find(T::HASH);
		if(elem != m_compMap.end())
		{
			RemoveCompFromList(m_compList, elem->second);
			
			// Remove from separate lists for per-frame method calls only if they are used.
			if(typeid(&CNodeComponent::Update) != typeid(&T::Update)) RemoveCompFromList(m_updateList, elem->second);
			if(typeid(&CNodeComponent::LateUpdate) != typeid(&T::LateUpdate)) RemoveCompFromList(m_lateUpdateList, elem->second);

			//delete elem->second;
			m_compMap.erase(elem);
		}
	}
	
	template<typename T>
	const class CNodeComponent* GetComponent() const
	{
		return m_compMap.find(T::HASH)->second;
	}
	
	//-----------------------------------------------------------------------------------------------

	void* AddComponentToObject(u32 compHash, class CNodeObject* pObject);
	void* GetComponentFromObject(u32 compHash, const class CNodeObject* pObject);
	bool TryToGetComponentFromObject(u32 compHash, const class CNodeObject* pObject, void** ppData);
	void RemoveComponentFromObject(u32 compHash, class CNodeObject* pObject);
	
	void* AddComponentToObject(u32 compHash, u64 objHash);
	void* GetComponentFromObject(u32 compHash, u64 objHash);
	bool TryToGetComponentFromObject(u32 compHash, u64 objHash, void** ppData);
	void RemoveComponentFromObject(u32 compHash, u64 objHash);
	
private:
	inline void RemoveCompFromList(std::vector<CNodeComponent*>& list, CNodeComponent* pTarget)
	{
		size_t j = 0;
		for(size_t i = 0; i < list.size(); ++i)
		{
			if(list[i] != pTarget)
			{
				list[j++] = list[i];
			}
		}
		
		list.resize(j);
	}

private:
	std::vector<CNodeComponent*> m_compList;
	std::vector<CNodeComponent*> m_updateList;
	std::vector<CNodeComponent*> m_lateUpdateList;
	std::unordered_map<u32, CNodeComponent*> m_compMap;
};

#endif
