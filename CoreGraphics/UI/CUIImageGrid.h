//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIImageGrid.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUIIMAGEGRID_H
#define CUIIMAGEGRID_H

#include "CUIRenderer.h"
#include <vector>

namespace UI
{
	class CUIImageGrid : public CUIRenderer
	{
	public:
		struct Data : RendererData
		{
			Math::VectorInt2 uvCells = { 1, 1 };
			Math::VectorInt2 cells = { 1, 1 };

			u64 matHash;
			u64 texHash;
		};

	public:
		CUIImageGrid(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection = nullptr);
		~CUIImageGrid();
		CUIImageGrid(const CUIImageGrid&) = delete;
		CUIImageGrid(CUIImageGrid&&) = delete;
		CUIImageGrid& operator = (const CUIImageGrid&) = delete;
		CUIImageGrid& operator = (CUIImageGrid&&) = delete;
		
		void Initialize() final;
		void DrawSetup() final;
		void SetCellIndex(u32 cell, u32 index);
		
		bool CommandElement(const CUICommandList::StateElem& stateElem) final;
		static bool SetFullDataFromKeyValue(CUIImageGrid* pImageGrid, const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::wstring& key, const std::wstring& value, Data& data);
		static bool SetDataFromKeyValue(CUIImageGrid* pImageGrid, const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::wstring& key, const std::wstring& value, Data& data);
		
		// Accessors.
		virtual inline u32 GetInstanceMax() const { return m_instanceCount; }
		virtual inline u32 GetInstanceCount() const { return IsActive() ? GetInstanceMax() : 0; }

		inline u64 GetMaterialHash() const final { return m_data.matHash; }
		inline u64 GetTextureHash() const final { return m_data.texHash; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		
	private:
		void BuildFromSection(const CUIScript::Section* pSection) final;
		void OnCellIndexChange(const void* params);
		
		// Accessors.
		inline const RendererData& GetRendererData() const final { return m_data; }
		
	private:
		u32 m_instanceCount;
		Data m_data;
		
		Math::Vector2 m_cellSize;
		std::vector<u32> m_indexList;
	};
};

#endif
