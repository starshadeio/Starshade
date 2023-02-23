//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInputLayout.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CInputLayout.h"
#include "CInputDeviceList.h"
#include "CInputMouse.h"
#include "../Factory/CFactory.h"
#include <Math/CMathFNV.h>
#include <Utilities/CMemoryFree.h>

namespace App
{
	CInputLayout::CInputLayout() :
		m_pDeviceList(nullptr) {
	}

	CInputLayout::~CInputLayout() { }

	void CInputLayout::Initialize(u32 hash, class CInputDeviceList* pDeviceList)
	{
		m_hash = hash;
		m_pDeviceList = pDeviceList;
	}

	void CInputLayout::Update()
	{
		m_pDeviceList->Update();
	}

	void CInputLayout::PostUpdate()
	{
		m_pDeviceList->PostUpdate();
	}

	bool CInputLayout::Register(const InputBindingSet& bindingSet)
	{
		u32 bindingHash = Math::FNV1a_32(bindingSet.name.c_str());
		if(m_bindMap.find(bindingHash) != m_bindMap.end()) { return false; }
		m_bindMap.insert({ bindingHash, bindingSet });
		m_pDeviceList->Register(m_hash, bindingHash, bindingSet);
		return true;
	}
};
