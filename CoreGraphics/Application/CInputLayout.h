//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CInputLayout.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CINPUTLAYOUT_H
#define CINPUTLAYOUT_H

#include "CInputData.h"
#include <Math/CMathVector2.h>
#include <Globals/CGlobals.h>
#include <unordered_map>

namespace App
{
	class CInputLayout
	{
	public:
		CInputLayout();
		~CInputLayout();
		CInputLayout(const CInputLayout&) = delete;
		CInputLayout(CInputLayout&&) = delete;
		CInputLayout& operator = (const CInputLayout&) = delete;
		CInputLayout& operator = (CInputLayout&&) = delete;

		void Initialize(u32 hash, class CInputDeviceList* pDeviceList);
		void Update();
		void PostUpdate();

		bool Register(const InputBindingSet& bindingSet);
		
		// Accessors.
		inline u32 GetHash() const { return m_hash; }
		inline const InputBindingSet& GetBindingSet(u32 hash) const { return m_bindMap.find(hash)->second; }

	private:
		u32 m_hash;

		class CInputDeviceList* m_pDeviceList;
		std::unordered_map<u32, InputBindingSet> m_bindMap;
	};
};

#endif
