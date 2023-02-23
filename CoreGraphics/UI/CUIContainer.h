//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIContainer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUICONTAINER_H
#define CUICONTAINER_H

#include "CUIElement.h"
#include "CUIScript.h"

namespace UI
{
	class CUIContainer : public CUIElement
	{
	public:
		struct Data : ElementData
		{
		};

	public:
		CUIContainer(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection = nullptr);
		~CUIContainer();
		CUIContainer(const CUIContainer&) = delete;
		CUIContainer(CUIContainer&&) = delete;
		CUIContainer& operator = (const CUIContainer&) = delete;
		CUIContainer& operator = (CUIContainer&&) = delete;
		
		static bool SetDataFromKeyValue(const CUIScript* pScript, const std::wstring& key, const std::wstring& value, Data& data);
		
		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	protected:
		void OnUpdate() final;

		void BuildFromSection(const CUIScript::Section* pSection) final;
		inline const ElementData& GetElementData() const final { return m_data; }

	private:
		Data m_data;
	};
};

#endif
