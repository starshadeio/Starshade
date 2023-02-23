//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIText.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUITEXT_H
#define CUITEXT_H

#include "CUIRenderer.h"
#include <string>

namespace UI
{
	class CUIText : public CUIRenderer
	{
	public:
		enum class HAlign
		{
			Left,
			Center,
			Right,
		};

		enum class VAlign
		{
			Top,
			Center,
			Bottom
		};

	public:
		struct Data : RendererData
		{
			HAlign hAlign = HAlign::Center;
			VAlign vAlign = VAlign::Center;
			float fontSize = 16.0f;
			float spacingScale = 1.0f;
			u64 matHash;
			u64 fontHash;
			u32 maxText;
			std::wstring text;
		};

	public:
		CUIText(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection = nullptr);
		~CUIText();
		CUIText(const CUIText&) = delete;
		CUIText(CUIText&&) = delete;
		CUIText& operator = (const CUIText&) = delete;
		CUIText& operator = (CUIText&&) = delete;

		void Initialize() final;
		void DrawSetup() final;
		
		bool CommandElement(const CUICommandList::StateElem& stateElem) final;
		static bool SetFullDataFromKeyValue(const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::wstring& key, const std::wstring& value, Data& data);
		static bool SetDataFromKeyValue(const CUIScript* pScript, const std::wstring& key, const std::wstring& value, Data& data);
		
		// Accessors.
		virtual inline u32 GetInstanceMax() const { return m_data.maxText; }
		virtual inline u32 GetInstanceCount() const { return IsActive() ? static_cast<u32>(m_data.text.size()) : 0; }

		inline u64 GetMaterialHash() const final { return m_data.matHash; }
		inline u64 GetTextureHash() const final { return m_data.fontHash; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

		inline void SetText(const wchar_t* str) 
		{
			if(wcslen(str) < m_data.text.length())
			{
				Reset();
			}

			m_data.text = str;
			MarkAsDirty();
			m_bUpdateText = true;
		}

		inline void SetText(std::wstring&& str)
		{
			if(str.length() < m_data.text.length())
			{
				Reset();
			}

			m_data.text = std::move(str);
			MarkAsDirty();
			m_bUpdateText = true;
		}

	private:
		void BuildString();
		void UpdateMetrics();
		void CalculateStart();
		
		void BuildFromSection(const CUIScript::Section* pSection) final;
		
		// Accessors.
		inline const RendererData& GetRendererData() const final { return m_data; }

	private:
		Data m_data;
		
		bool m_bUpdateText;
		float m_metricScale;
		float m_padding;

		Math::Vector2 m_position;
		Math::Vector2 m_offset;
		Math::Vector2 m_size;
		
		class CFont* m_pFont;
		Graphics::CGraphicsAPI* m_pGraphics;
	};
};

#endif
