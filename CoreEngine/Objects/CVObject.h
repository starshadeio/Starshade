//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CVObject.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CVOBJECT_H
#define CVOBJECT_H

#include "CVComponent.h"
#include "../Globals/CGlobals.h"
#include "../Math/CMathFNV.h"
#include "../Math/CMathRect.h"
#include <vector>
#include <fstream>
#include <string>
#include <shared_mutex>
#ifdef _DEBUG
#include <unordered_set>
#include <cassert>
#endif

namespace Logic
{
	class CTransform;
};

namespace Physics
{
	class CVolume;
};

class CVObject
{
public:
	friend CVComponent;

protected:
	CVObject(const std::wstring& name, u32 viewHash = 0, u32 layer = 0, u32 tagHash = 0);
	CVObject(u64 hash, u32 viewHash = 0, u32 layer = 0, u32 tagHash = 0);
	virtual ~CVObject();
	CVObject(const CVObject&) = delete;
	CVObject(CVObject&&) = delete;
	CVObject& operator = (const CVObject&) = delete;
	CVObject& operator = (CVObject&&) = delete;

protected:
	virtual void Initialize() { }
	virtual void PostInitialize() { }
	virtual void Update() { }
	virtual void LateUpdate() { }
	virtual void Release() { }

	virtual void OnMove(const Math::Rect& rect) { }
	virtual void OnResize(const Math::Rect& rect) { }
	
	virtual void SaveToFile(std::ofstream& file) const { }
	virtual void LoadFromFile(std::ifstream& file) { }
	
public:
	template<typename T>
	T* FindComponent() const
	{
		std::shared_lock<std::shared_mutex> lk(m_mutex);

		for(CVComponent* pComponent : m_componentList)
		{
			T* pT = dynamic_cast<T*>(pComponent);
			if(pT != nullptr) {
				return pT;
			}
		}

		return nullptr;
	}

	// Accessors.
	inline u32 GetLayer() const { return m_layer; }
	inline u32 GetTagHash() const { return m_tagHash; }
	inline u64 GetHash() const { return m_hash; }

	inline u32 GetViewHash() const { return m_viewHash; }
	inline bool HasPhysics() const { return m_bHasPhysics; }

	inline Logic::CTransform* GetTransform() const { return m_pTransform; }

private:
	void AddComponent(CVComponent* pComponent);
	void RemoveComponent(CVComponent* pComponent);
	
	// Modifiers.
	inline void OnPhysicsRegistered() { m_bHasPhysics = true; }
	inline void OnPhysicsDeregistered() { m_bHasPhysics = false; }

protected:
	inline void SetLayer(u32 layer) { m_layer = layer; }
	inline void SetTagHash(u32 tagHash) { m_tagHash = tagHash; }
	inline void SetViewHash(u32 viewHash) { m_viewHash = viewHash; }

private:
	mutable std::shared_mutex m_mutex;

	u32 m_layer;
	u32 m_tagHash;
	u32 m_viewHash;
	u64 m_hash;

	bool m_bHasPhysics;

	Logic::CTransform* m_pTransform;
#ifdef _DEBUG
	static std::unordered_set<u64> m_idSet;
#endif

	std::vector<CVComponent*> m_componentList;
};

#endif
