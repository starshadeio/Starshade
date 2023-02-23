//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIGridSelect.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUIGRIDSELECT_H
#define CUIGRIDSELECT_H

#include "CUISelect.h"
#include "CUIImageGrid.h"
#include "CUIImage.h"
#include "CUIGrid.h"
#include <Math/CMathVectorInt2.h>

namespace UI
{
	class CUIGridSelect : public CUISelect
	{
	public:
		struct Data : SelectData
		{
			bool bSeparateCells = false;
			bool bUseSelector = false;
			Math::VectorInt2 cells = { 1, 1 };

			CUIImageGrid::Data bgData;
			CUIGrid::Data selectorData;
			CUIGrid::Data gridData;

			u64 onEnterHash = 0;
			u64 onExitHash = 0;
			u64 onPressHash = 0;
			u64 onReleaseHash = 0;
		};

	public:
		CUIGridSelect(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection = nullptr);
		virtual ~CUIGridSelect();
		CUIGridSelect(const CUIGridSelect&) = delete;
		CUIGridSelect(CUIGridSelect&&) = delete;
		CUIGridSelect& operator = (const CUIGridSelect&) = delete;
		CUIGridSelect& operator = (CUIGridSelect&&) = delete;
		
		void OnRegister(class CUICanvas* pCanvas) final;

		void OnEnter() final;
		void OnExit() final;
		bool OnSelect(const App::InputCallbackData& callback) final;
		
		bool CommandElement(const CUICommandList::StateElem& stateElem) final;
		static bool SetDataFromKeyValue(CUIGridSelect* pGridSelect, const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::vector<std::wstring>& keys, const std::wstring& value, Data& data);
		
		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	protected:
		void OnUpdate() override;

	protected:
		void BuildFromSection(const CUIScript::Section* pSection) final;
		u32 CalculateCursorIndex();
		void CalculateSelectorPosition(u32 index);
		
		void OnIndexChange(const void* params);

		// Accessors.
		inline const ElementData& GetElementData() const final { return m_data; }
		inline const SelectData& GetSelectData() const final { return m_data; }

	private:
		Data m_data;
		
		Math::Vector2 m_cellSize;
		u32 m_index;

		CUIImageGrid m_background;
		CUIGrid m_selector;
		CUIGrid m_grid;
	};
};

#endif
