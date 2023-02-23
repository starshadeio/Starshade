//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CVComponent.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CVComponent.h"
#include "CVObject.h"
#include "../Logic/CTransform.h"

CVComponent::CVComponent(const CVObject* pObject) : 
	m_pObject(pObject) 
{
	if(m_pObject)
	{
		AddComponent();
	}
}

CVComponent::~CVComponent()
{
	if(m_pObject)
	{
		RemoveComponent();
	}
}

void CVComponent::AddComponent()
{
	const_cast<CVObject*>(m_pObject)->AddComponent(this);
}

void CVComponent::RemoveComponent()
{
	const_cast<CVObject*>(m_pObject)->RemoveComponent(this);
}

void CVComponent::OnPhysicsRegistered()
{
	const_cast<CVObject*>(m_pObject)->OnPhysicsRegistered();
}

void CVComponent::OnPhysicsDeregistered()
{
	const_cast<CVObject*>(m_pObject)->OnPhysicsDeregistered();
}

