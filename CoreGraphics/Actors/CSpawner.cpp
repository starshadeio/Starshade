//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CSpawner.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CSpawner.h"
#include "CPawn.h"
#include <Application/CCoreManager.h>
#include <Logic/CNodeTransform.h>

namespace Actor
{
	//-----------------------------------------------------------------------------------------------
	// CSpawner methods.
	//-----------------------------------------------------------------------------------------------
	
	CSpawner::CSpawner() : 
		CNodeComponent(HASH)
	{
	}

	CSpawner::~CSpawner()
	{
	}

	void CSpawner::Register()
	{
		App::CCoreManager::Instance().NodeRegistry().RegisterComponent<CSpawner>();
	}

	void CSpawner::Spawn(CPawn* pawn)
	{
		auto elem = m_dataMap.begin();

		auto& transform = *reinterpret_cast<Logic::CNodeTransform::Data*>(App::CCoreManager::Instance().NodeRegistry().GetComponentFromObject(Logic::CNodeTransform::HASH, elem->second.GetThis()));
		pawn->SetPosition(transform.GetPosition() + Math::SIMD_VEC_UP * 2.0f);
		pawn->SetEulerAngles(*(Math::Vector3*)transform.GetEuler().ToFloat());
	}
	
	//---------------------------------------------
	// Object methods.
	//---------------------------------------------

	void* CSpawner::AddToObject(u64 objHash)
	{
		return &m_dataMap.insert({ objHash, Data(objHash) }).first->second;
	}
	
	void* CSpawner::GetFromObject(u64 objHash)
	{
		return &m_dataMap.find(objHash)->second;
	}

	bool CSpawner::TryToGetFromObject(u64 objHash, void** ppData)
	{
		auto elem = m_dataMap.find(objHash);
		if(elem == m_dataMap.end()) return false;
		*ppData = &elem->second;
		return true;
	}

	void CSpawner::RemoveFromObject(u64 objHash)
	{
		m_dataMap.erase(objHash);
	}
};
