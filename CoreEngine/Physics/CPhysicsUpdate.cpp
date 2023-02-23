//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CPhysicsUpdate.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPhysicsUpdate.h"

namespace Physics
{
	CPhysicsUpdate::CPhysicsUpdate() : CVComponent(nullptr) 
	{
		Reset();
	}

	void CPhysicsUpdate::Reset()
	{
		m_data.updateFunc = nullptr;
		m_pRef = nullptr;
	}

	void CPhysicsUpdate::Update()
	{
		if(m_data.updateFunc)
		{
			m_data.updateFunc();
		}
	}
};
