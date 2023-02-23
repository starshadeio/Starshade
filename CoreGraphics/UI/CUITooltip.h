//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUITooltip.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUITOOLTIP_H
#define CUITOOLTIP_H

#include "CUIText.h"

namespace UI
{
	class CUITooltip : public CUIElement
	{
	public:
		struct Data : ElementData
		{
			CUIText::Data txData = { };
		};

	public:
		CUITooltip(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection = nullptr);
		~CUITooltip();
		CUITooltip(const CUITooltip&) = delete;
		CUITooltip(CUITooltip&&) = delete;
		CUITooltip& operator = (const CUITooltip&) = delete;
		CUITooltip& operator = (CUITooltip&&) = delete;

		void OnRegister(class CUICanvas* pCanvas) final;
		void Release() final;

		void SetTooltip(const class CUISelect* pSelect);
		
		static bool SetDataFromKeyValue(CUITooltip* pTooltip, const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::vector<std::wstring>& key, const std::wstring& value, Data& data);
		
		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		
	protected:
		void BuildFromSection(const CUIScript::Section* pSection) final;

		inline const ElementData& GetElementData() const final { return m_data; }

	private:
		Data m_data;

		CUIText m_text;
	};
};

#endif
