//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUISelect.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUISELECT_H
#define CUISELECT_H

#include "CUIElement.h"
#include "../Application/CInputData.h"
#include <Math/CMathVector2.h>
#include <Objects/CVComponent.h>

namespace UI
{
	class CUISelect : public CUIElement
	{
	protected:
		struct SelectData : ElementData
		{
			bool bHasTooltip = false;
			bool bHasKeybinding = false;
			u64 tooltipHash = 0;
			u32 inputLayoutHash = 0;
			u32 keybindHash = 0;
		};

	public:
		CUISelect(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection);
		virtual ~CUISelect();
		CUISelect(const CUISelect&) = delete;
		CUISelect(CUISelect&&) = delete;
		CUISelect& operator = (const CUISelect&) = delete;
		CUISelect& operator = (CUISelect&&) = delete;

		bool TestPoint(const Math::Vector2& point);
		
		bool CommandElement(const CUICommandList::StateElem& stateElem) override;
		static bool SetDataFromKeyValue(const CUIScript* pScript, const std::wstring& key, const std::wstring& value, SelectData& data);
		
		virtual void OnEnter() { m_bHovered = true; }
		virtual void OnExit() { m_bHovered = false; }
		virtual bool OnSelect(const App::InputCallbackData& callback) { return false; }

		// Accessors.
		inline bool IsHovered() const { return m_bHovered; }

		inline bool HasTooltip() const { return GetSelectData().bHasTooltip; }
		inline u64 GetTooltipHash() const { return GetSelectData().tooltipHash; }

		inline bool HasKeybinding() const { return GetSelectData().bHasKeybinding; }
		inline u32 GetInputLayoutHash() const { return GetSelectData().inputLayoutHash; }
		inline u32 GetKeybindHash() const { return GetSelectData().keybindHash; }
		
	protected:
		void BuildFromSection(const CUIScript::Section* pSection) override;
		
		// Accessors.
		virtual inline const SelectData& GetSelectData() const = 0;
		inline const Math::Vector2& GetRelativeCursorPoint() const { return m_relativeCursorPt; }

	private:
		bool m_bHovered;
		Math::Vector2 m_relativeCursorPt;

	protected:
		class CUICanvas* m_pCanvas;
	};
};

#endif
