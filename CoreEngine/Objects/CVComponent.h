//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CVComponent.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CVCOMPONENT_H
#define CVCOMPONENT_H

#include "../Globals/CGlobals.h"
#include "../Math/CMathRect.h"
#include <fstream>

namespace App
{
	class CPanel;
};

class CVObject;
class CNodeObject;

class CVComponent
{
public:
	friend CNodeObject;

protected:
	CVComponent(const CVObject* pObject);
	virtual ~CVComponent();
	
	CVComponent(const CVComponent&) = default;
	CVComponent(CVComponent&&) = default;
	CVComponent& operator = (const CVComponent&) = default;
	CVComponent& operator = (CVComponent&&) = default;

public:
	virtual void operator ()() { }

protected:
	virtual void Initialize() { }
	virtual void PostInitialize() { }
	virtual void Reset() { }
	virtual bool TryUpdate(u8 frame) { Update(); return true; }
	virtual void Update() { }
	virtual void LateUpdate() { }
	virtual void Release() { }

	virtual void OnMove(const Math::Rect& rect) { }
	virtual void OnResize(const Math::Rect& rect) { }
	
	virtual void SaveToFile(std::ofstream& file) const { }
	virtual void LoadFromFile(std::ifstream& file) { }

	void AddComponent();
	void RemoveComponent();
	
	void OnPhysicsRegistered();
	void OnPhysicsDeregistered();

protected:
	const CVObject* m_pObject;
};

#endif
