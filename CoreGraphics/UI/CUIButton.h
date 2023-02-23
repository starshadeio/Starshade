//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIButton.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUIBUTTON_H
#define CUIBUTTON_H

#include "CUISelect.h"
#include "CUIImage.h"
#include "CUIText.h"
#include "CUIFrame.h"
#include <functional>

namespace UI
{
	class CUIButton : public CUISelect
	{
	public:
		struct Data : SelectData
		{
			bool bEnable = true;
			bool bHold = false;
			bool bAllowHoldToggle = false;

			bool bUseBackgroud = false;
			bool bUseForegroud = false;
			bool bUseText = false;
			
			Math::Vector4 bgInset = Math::VEC4_ZERO;
			Math::Vector4 fgInset = Math::VEC4_ZERO;
			Math::Vector4 txInset = Math::VEC4_ZERO;
			
			Math::Color bgColorEnter = Math::COLOR_WHITE;
			Math::Color bgColorExit = Math::COLOR_WHITE;
			Math::Color bgColorClick = Math::COLOR_WHITE;
			Math::Color bgColorDisable = Math::COLOR_WHITE;
			
			Math::Color fgColorEnter = Math::COLOR_WHITE;
			Math::Color fgColorExit = Math::COLOR_WHITE;
			Math::Color fgColorClick = Math::COLOR_WHITE;
			Math::Color fgColorDisable = Math::COLOR_WHITE;
			
			Math::Color txColorEnter = Math::COLOR_WHITE;
			Math::Color txColorExit = Math::COLOR_WHITE;
			Math::Color txColorClick = Math::COLOR_WHITE;
			Math::Color txColorDisable = Math::COLOR_WHITE;

			CUIImage::Data bgData;
			CUIImage::Data fgData;
			CUIText::Data txData;
			
			u64 onEnterHash = 0;
			u64 onExitHash = 0;
			u64 onPressHash = 0;
			u64 onReleaseHash = 0;
		};

	public:
		CUIButton(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection = nullptr);
		virtual ~CUIButton();
		CUIButton(const CUIButton&) = delete;
		CUIButton(CUIButton&&) = delete;
		CUIButton& operator = (const CUIButton&) = delete;
		CUIButton& operator = (CUIButton&&) = delete;

		void OnRegister(class CUICanvas* pCanvas) final;
		
		void SetEnable(bool bEnable);
		void SetHold(bool bHold);

		void OnEnter() final;
		void OnExit() final;
		bool OnSelect(const App::InputCallbackData& callback) final;
		
		bool CommandElement(const CUICommandList::StateElem& stateElem) final;
		static bool SetDataFromKeyValue(CUIButton* pButton, const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::vector<std::wstring>& keys, const std::wstring& value, Data& data);
		
		// Accessors.
		inline bool GetEnable() const { return m_data.bEnable; }
		inline bool GetHold() const { return m_data.bHold; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

		inline void SetBackgroundColor(const Math::Color& color) { m_background.SetColor(m_data.bgData.color = color); }
		inline void SetForegroundColor(const Math::Color& color) { m_foreground.SetColor(m_data.fgData.color = color); }
		inline void SetTextColor(const Math::Color& color) { m_text.SetColor(m_data.txData.color = color); }

		inline void SetBackgroundTexCoord(const Math::Vector4& texCoord) { m_background.SetTexCoord(m_data.bgData.texCoord = texCoord); }
		inline void SetForegroundTexCoord(const Math::Vector4& texCoord) { m_foreground.SetTexCoord(m_data.fgData.texCoord = texCoord); }
		
	protected:
		void BuildFromSection(const CUIScript::Section* pSection) final;

		inline const ElementData& GetElementData() const final { return m_data; }
		inline const SelectData& GetSelectData() const final { return m_data; }

	private:
		Data m_data;

		CUIImage m_background;
		CUIImage m_foreground;
		CUIText m_text;
	};
};

#endif
