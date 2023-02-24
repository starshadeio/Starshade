//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel App
//
// File: Application/CNodeLoader.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CNodeLoader.h"
#include <Logic/CNodeTransform.h>
#include <Actors/CSpawner.h>
#include <fstream>
#include <cassert>

namespace App
{
	CNodeLoader::CNodeLoader() : 
		m_rootNode(L"Root")
	{
	}

	CNodeLoader::~CNodeLoader()
	{
	}
	
	void CNodeLoader::Initialize()
	{
		{ // Add node objects.
			// Spawn points.
			m_rootNode.AddObject(L"spawn_point");
			
			m_rootNode.AddComponent<Logic::CNodeTransform, Logic::CNodeTransform::Data>([](Logic::CNodeTransform::Data* pTransform){
				pTransform->SetPosition(Math::SIMDVector(0.0f, -15.0f, 0.0f));
				pTransform->SetEulerConstraints(Logic::AXIS_CONSTRAINT_FLAG_X | Logic::AXIS_CONSTRAINT_FLAG_Z);
				pTransform->SetScaleConstraints(Logic::AXIS_CONSTRAINT_FLAG_ALL);
			});

			m_rootNode.AddComponent<Actor::CSpawner, Actor::CSpawner::Data>([](Actor::CSpawner::Data* pData){
			});
		}
	}

	void CNodeLoader::PostInitialize()
	{
		{ // Load data.
			std::ifstream file(L".\\Data\\Root.dat", std::ios::binary);
			assert(file.is_open());

			m_rootNode.Load(file);
			
			file.close();
		}
	}
};
