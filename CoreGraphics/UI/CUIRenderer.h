//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIRenderer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUIRENDERER_H
#define CUIRENDERER_H

#include "CUIQuad.h"
#include "CUIElement.h"
#include <vector>

namespace UI
{
	class CUIRenderer : public CUIElement
	{
	public:
		friend class CUICanvas;

	protected:
		struct RendererData : ElementData
		{
			Math::Color color = Math::COLOR_WHITE;
		};

	protected:
		CUIRenderer(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection);
		virtual ~CUIRenderer();

		CUIRenderer(const CUIRenderer&) = delete;
		CUIRenderer(CUIRenderer&&) = delete;
		CUIRenderer& operator = (const CUIRenderer&) = delete;
		CUIRenderer& operator = (CUIRenderer&&) = delete;

	public:
		void Reset();
		virtual void DrawSetup();
		void Release() override { }
		
		void OnRegistration(CUIQuad* pQuad);
		
		void SetColor(const Math::Color& color);
		
		bool CommandElement(const CUICommandList::StateElem& stateElem) override;
		static bool SetDataFromKeyValue(const CUIScript* pScript, const std::wstring& key, const std::wstring& value, RendererData& data);
		
		void MarkAsDirty();

		// Accessors.
		inline s32 GetDepth() const { return GetRendererData().depth; }
		
		virtual inline u32 GetInstanceMax() const { return 1; }
		virtual inline u32 GetInstanceCount() const { return 1; }

		virtual inline u64 GetMaterialHash() const = 0;
		virtual inline u64 GetTextureHash() const = 0;

		inline const CUIQuad* GetQuad() const { return m_pQuad; }
		
		// Modifiers.
		inline void ResetInstanceList() { m_pInstanceList.clear(); }

	protected:
		void OnUpdate() final;
		void OnActiveChange(bool bActive) final;
		CUIQuad::Instance* PullInstance(u32 index);
		
		void BuildFromSection(const CUIScript::Section* pSection) override;
		
		// Accessors.
		virtual inline const RendererData& GetRendererData() const = 0;
		inline const ElementData& GetElementData() const final { return GetRendererData(); }

	private:
		bool m_bDirty;
		
		std::vector<CUIQuad::Instance*> m_pInstanceList;

		CUIQuad* m_pQuad;
	};
};

#endif
