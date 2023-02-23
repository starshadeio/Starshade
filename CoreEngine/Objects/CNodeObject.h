//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CNodeObject.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODEOBJECT_H
#define CNODEOBJECT_H

#include "../Globals/CGlobals.h"
#include "../Application/CCoreManager.h"
#include <fstream>

class CNodeObject
{
public:
	CNodeObject(u64 hash, u32 viewHash = 0, u32 layer = 0, u32 tagHash = 0);
	virtual ~CNodeObject();
	CNodeObject(const CNodeObject&) = delete;
	CNodeObject(CNodeObject&&) = delete;
	CNodeObject& operator = (const CNodeObject&) = delete;
	CNodeObject& operator = (CNodeObject&&) = delete;
	
	void Save(std::ofstream& file) const;
	void Load(std::ifstream& file);
	
	template<typename T>
	T::Data* AddComponent()
	{
		return reinterpret_cast<T::Data*>(App::CCoreManager::Instance().NodeRegistry().AddComponentToObject(T::HASH, this));
	}
	
	template<typename T>
	T::Data* GetComponent() const
	{
		return reinterpret_cast<T::Data*>(App::CCoreManager::Instance().NodeRegistry().GetComponentFromObject(T::HASH, this));
	}
	
	template<typename T>
	bool TryToGetComponent(T::Data** ppData) const
	{
		void* pData;
		if(App::CCoreManager::Instance().NodeRegistry().TryToGetComponentFromObject(T::HASH, this, &pData))
		{
			*ppData = reinterpret_cast<T::Data*>(pData);
			return true;
		}

		return false;
	}
	
	template<typename T>
	void RemoveComponent()
	{
		App::CCoreManager::Instance().NodeRegistry().RemoveComponentFromObject(T::HASH, this);
	}

	// Accessors.
	inline u32 GetViewHash() const { return m_viewHash; }
	inline u32 GetLayer() const { return m_layer; }
	inline u32 GetTagHash() const { return m_tagHash; }
	inline u64 GetHash() const { return m_hash; }

protected:
	u32 m_viewHash;
	u32 m_layer;
	u32 m_tagHash;
	u64 m_hash;
};

#endif
