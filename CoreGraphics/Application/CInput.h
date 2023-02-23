//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInput.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CINPUT_H
#define CINPUT_H

#include "CInputData.h"
#include <unordered_map>

namespace App
{
	class CInput
	{
	private:
		CInput();
		~CInput();
		CInput(const CInput&) = delete;
		CInput(CInput&&) = delete;
		CInput& operator = (const CInput&) = delete;
		CInput& operator = (CInput&&) = delete;

	public:
		static CInput& Instance()
		{
			static CInput instance;
			return instance;
		}

		void Initialize(class CPanel* pPanel);
		void Reset();
		void Update();
		void PostUpdate();
		void Release();

		void CreateLayout(u32 layoutHash, bool bActive);
		void SwitchLayout(u32 layoutHash);

		void RegisterBinding(const u32* pLayoutHashList, u32 layoutHashCount, const InputBindingSet& bindingSet);

		std::wstring KeybindingString(u32 layoutHash, u32 bindingHash);

		// Accessors.
		inline class CInputDeviceList* GetDeviceList() const { return m_pDeviceList; }
		inline class CInputLayout* GetActiveLayout() const { return m_pLayout; }

	private:
		std::unordered_map<u32, class CInputLayout*> m_layoutMap;
		class CInputDeviceList* m_pDeviceList;
		class CInputLayout* m_pLayout;
	};
};

#endif
