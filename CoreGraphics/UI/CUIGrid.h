//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIGrid.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUIGRID_H
#define CUIGRID_H

#include "CUIRenderer.h"
#include <Math/CMathVectorInt2.h>

namespace UI
{
	class CUIGrid : public CUIRenderer
	{
	public:
		struct Data : RendererData
		{
			float thickness = 1.0f;
			Math::VectorInt2 cells = { 1, 1 };

			u64 matHash;
			u64 texHash;
		};

	public:
		CUIGrid(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection = nullptr);
		~CUIGrid();
		CUIGrid(const CUIGrid&) = delete;
		CUIGrid(CUIGrid&&) = delete;
		CUIGrid& operator = (const CUIGrid&) = delete;
		CUIGrid& operator = (CUIGrid&&) = delete;
		
		void Initialize() final;
		void DrawSetup() final;
		
		bool CommandElement(const CUICommandList::StateElem& stateElem) final;
		static bool SetFullDataFromKeyValue(const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::wstring& key, const std::wstring& value, Data& data);
		static bool SetDataFromKeyValue(const CUIScript* pScript, const std::wstring& key, const std::wstring& value, Data& data);
		
		// Accessors.
		virtual inline u32 GetInstanceMax() const { return m_instanceCount; }
		virtual inline u32 GetInstanceCount() const { return IsActive() ? GetInstanceMax() : 0; }

		inline u64 GetMaterialHash() const final { return m_data.matHash; }
		inline u64 GetTextureHash() const final { return m_data.texHash; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		
	private:
		void BuildFromSection(const CUIScript::Section* pSection) final;
		
		// Accessors.
		inline const RendererData& GetRendererData() const final { return m_data; }
		
	private:
		u32 m_instanceCount;

		Data m_data;
	};
};

#endif
