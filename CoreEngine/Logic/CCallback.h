//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Logic/CCallback.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCALLBACK_H
#define CCALLBACK_H

#include "../Objects/CVComponent.h"
#include "../Globals/CGlobals.h"
#include "../Math/CMathFNV.h"
#include <unordered_map>
#include <functional>

namespace Logic {
	const u32 CALLBACK_INTERACT = Math::FNV1a_32("Interact");

	class CCallback : public CVComponent
	{
	public:
		struct Data
		{
			std::unordered_map<u32, std::function<bool(void*)>> callbackMap;
		};

	public:
		CCallback(const CVObject* pObject) : CVComponent(pObject) { }
		~CCallback() { }
		CCallback(const CCallback&) = delete;
		CCallback(CCallback&&) = default;
		CCallback& operator = (const CCallback&) = delete;
		CCallback& operator = (CCallback&&) = default;

		bool CallCallback(u32 funcHash, void* pVal) { return m_data.callbackMap.find(funcHash)->second(pVal); }

		// Modifiers.
		inline void SetData(Data& data) { m_data = std::move(data); }

	private:
		Data m_data;
	};
};

#endif
