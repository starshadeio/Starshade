//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CPawn.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CPawn.h"
#include "CMotor.h"

namespace Actor
{
	CPawn::CPawn(const wchar_t* pName, u32 viewHash) : 
		CVObject(pName, viewHash), 
		m_euler(0.0f),
		m_position(0.0f),
		m_pMotor(nullptr) { }

	CPawn::~CPawn() { }
	
	//-----------------------------------------------------------------------------------------------
	// Motor methods.
	//-----------------------------------------------------------------------------------------------

	bool CPawn::RegisterMotor(CMotor* pMotor)
	{
		return m_motorMap.insert({ pMotor->GetHash(), pMotor }).second;
	}

	bool CPawn::DeregisterMotor(u32 key)
	{
		return m_motorMap.erase(key) == 1;
	}

	bool CPawn::LoadMotor(u32 key)
	{
		auto elem = m_motorMap.find(key);
		if(elem != m_motorMap.end())
		{
			std::lock_guard<std::mutex> lk(m_motorMutex);

			if(m_pMotor) m_pMotor->Unload();
			m_pMotor = elem->second;
			m_pMotor->Load();

			return true;
		}

		return false;
	}
};
