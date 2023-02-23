//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CNodeComponent.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODECOMPONENT_H
#define CNODECOMPONENT_H

#include "../Globals/CGlobals.h"
#include "../Math/CMathRect.h"

class CNodeObject;

class CNodeComponent
{
public:
	friend class CNodeRegistry;

protected:
	CNodeComponent(u32 hash);
	virtual ~CNodeComponent();
	
	CNodeComponent(const CNodeComponent&) = default;
	CNodeComponent(CNodeComponent&&) = default;
	CNodeComponent& operator = (const CNodeComponent&) = default;
	CNodeComponent& operator = (CNodeComponent&&) = default;
	
	virtual void Initialize() { }
	virtual void PostInitialize() { }
	virtual void Update() { }
	virtual void LateUpdate() { }
	virtual void Release() { }

	virtual void OnMove(const Math::Rect& rect) { }
	virtual void OnResize(const Math::Rect& rect) { }

	virtual void* AddToObject(CNodeObject* pObject);
	virtual void* GetFromObject(const CNodeObject* pObject);
	virtual bool TryToGetFromObject(const CNodeObject* pObject, void** ppData);
	virtual void RemoveFromObject(CNodeObject* pObject);

	virtual void* AddToObject(u64 objHash) = 0;
	virtual void* GetFromObject(u64 objHash) = 0;
	virtual bool TryToGetFromObject(u64 objHash, void** ppData) = 0;
	virtual void RemoveFromObject(u64 objHash) = 0;

private:
	u32 m_hash;
};

#endif
