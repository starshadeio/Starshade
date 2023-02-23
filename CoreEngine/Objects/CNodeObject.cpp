//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Objects/CNodeObject.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CNodeObject.h"
#include "../Math/CMathVector3.h"
#include "../Logic/CNodeTransform.h"

CNodeObject::CNodeObject(u64 hash, u32 viewHash, u32 layer, u32 tagHash) :
	m_hash(hash),
	m_viewHash(viewHash),
	m_layer(layer),
	m_tagHash(tagHash)
{
}

CNodeObject::~CNodeObject()
{
}

void CNodeObject::Save(std::ofstream& file) const
{
	Logic::CNodeTransform::Data* pTransform;
	if(TryToGetComponent<Logic::CNodeTransform>(&pTransform))
	{
		file.write(reinterpret_cast<const char*>(pTransform->GetPosition().ToFloat()), sizeof(float) * 3);
		file.write(reinterpret_cast<const char*>(pTransform->GetEuler().ToFloat()), sizeof(float) * 3);
	}
}

void CNodeObject::Load(std::ifstream& file)
{
	Logic::CNodeTransform::Data* pTransform;
	if(TryToGetComponent<Logic::CNodeTransform>(&pTransform))
	{
		Math::Vector3 vec;

		file.read(reinterpret_cast<char*>(vec.v), sizeof(float) * 3);
		pTransform->SetPosition(vec);
		file.read(reinterpret_cast<char*>(vec.v), sizeof(float) * 3);
		pTransform->SetEuler(vec);
	}
}
