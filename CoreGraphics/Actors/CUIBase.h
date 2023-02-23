//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CUIBase.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUIBASE_H
#define CUIBASE_H

#include "CHUD.h"
#include <Globals/CGlobals.h>
#include <Objects/CVComponent.h>
#include <fstream>

namespace Actor
{
	class CUIBase : public CVComponent
	{
	public:
		CUIBase(const CVObject* pObject, u32 hash) :
			CVComponent(pObject),
			m_hash(hash),
			m_pHUD(dynamic_cast<CHUD*>(const_cast<CVObject*>(pObject))),
			m_bLoaded(false) { }
		
		virtual ~CUIBase() { }
		CUIBase(const CUIBase&) = delete;
		CUIBase(CUIBase&&) = delete;
		CUIBase& operator = (const CUIBase&) = delete;
		CUIBase& operator = (CUIBase&&) = delete;

		virtual void Load() { m_bLoaded = true; }
		virtual void Unload() { m_bLoaded = false; }
		
		void Initialize() override { }
		void Update() override { }
		void Release() override { }
		void OnResize(const Math::Rect& rect) override { }

		virtual void SaveToFile(std::ofstream& file) const { }
		virtual void LoadFromFile(std::ifstream& file) { }

		// Accessors.
		inline u32 GetHash() const { return m_hash; }
		inline bool IsLoaded() const { return m_bLoaded; }

	protected:
		CHUD* m_pHUD;

	private:
		bool m_bLoaded;
		u32 m_hash;
	};
};

#endif
