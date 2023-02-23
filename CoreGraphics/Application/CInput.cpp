//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInput.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CInput.h"
#include "CInputDeviceList.h"
#include "CInputDevice.h"
#include "CInputLayout.h"
#include "../Factory/CFactory.h"
#include <Utilities/CMemoryFree.h>

namespace App
{
	CInput::CInput() :
		m_pDeviceList(nullptr),
		m_pLayout(nullptr)
	{
	}

	CInput::~CInput() { }

	void CInput::Initialize(class CPanel* pPanel)
	{
		m_pDeviceList = CFactory::Instance().CreateInputDeviceList();
		m_pDeviceList->Initialize(pPanel);
	}

	void CInput::Reset()
	{
		m_pDeviceList->Reset();
	}

	void CInput::Update()
	{
		if(m_pLayout) m_pLayout->Update();
	}

	void CInput::PostUpdate()
	{
		if(m_pLayout) m_pLayout->PostUpdate();
	}

	void CInput::Release()
	{
		m_layoutMap.clear();
		SAFE_RELEASE_DELETE(m_pDeviceList);
	}

	//
	// Layout methods.
	//

	void CInput::CreateLayout(u32 layoutHash, bool bActive)
	{
		CInputLayout* pLayout = new CInputLayout();
		pLayout->Initialize(layoutHash, m_pDeviceList);
		if(bActive) { m_pLayout = pLayout; }

		m_layoutMap.insert({ layoutHash, pLayout });
	}

	void CInput::SwitchLayout(u32 layoutHash)
	{
		m_pLayout = m_layoutMap.find(layoutHash)->second;
	}

	//
	// (De)resgistration methods.
	//

	void CInput::RegisterBinding(const u32* pLayoutHashList, u32 layoutHashCount, const InputBindingSet& bindingSet)
	{
		for(u32 i = 0; i < layoutHashCount; ++i)
		{
			m_layoutMap.find(pLayoutHashList[i])->second->Register(bindingSet);
		}
	}

	//
	// Utility methods.
	//

	std::wstring CInput::KeybindingString(u32 layoutHash, u32 bindingHash)
	{
		const auto pLayout = m_layoutMap.find(layoutHash)->second;
		auto& binding = pLayout->GetBindingSet(bindingHash);

		const auto& pDevice = m_pDeviceList->GetDevice(binding.bindings[0].device);
		
		std::wstring res = L"";
		if((binding.bindings[0].modifier & INPUT_MODIFIER_FLAG_CTRL) == INPUT_MODIFIER_FLAG_CTRL)
		{
			res += INPUT_MODIFIER_STR[0] + L'+';
		}
		if((binding.bindings[0].modifier & INPUT_MODIFIER_FLAG_ALT) == INPUT_MODIFIER_FLAG_ALT)
		{
			res += INPUT_MODIFIER_STR[1] + L'+';
		}
		if((binding.bindings[0].modifier & INPUT_MODIFIER_FLAG_SHIFT) == INPUT_MODIFIER_FLAG_SHIFT)
		{
			res += INPUT_MODIFIER_STR[2] + L'+';
		}

		return res + pDevice->GetCodeName(binding.bindings[0].code);
	}
};
