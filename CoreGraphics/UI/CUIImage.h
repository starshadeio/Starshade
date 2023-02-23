//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIImage.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUIIMAGE_H
#define CUIIMAGE_H

#include "CUIRenderer.h"
#include <Math/CMathVector4.h>
#include <Math/CMathVectorInt2.h>
#include <Globals/CGlobals.h>

namespace UI
{
	class CUIImage : public CUIRenderer
	{
	public:
		struct Data : RendererData
		{
			bool bSplitImage = false;
			bool bFillCenter = true;
			bool bIgnoreCells = false;

			Math::Vector4 texCoord = { 0.0f, 0.0f, 1.0f, 1.0f };
			Math::Vector4 centerPartition;

			Math::VectorInt2 cells = { 1, 1 };
			u32 cellIndex = 0;

			u64 matHash;
			u64 texHash;
		};

	public:
		CUIImage(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection = nullptr);
		~CUIImage();
		CUIImage(const CUIImage&) = delete;
		CUIImage(CUIImage&&) = delete;
		CUIImage& operator = (const CUIImage&) = delete;
		CUIImage& operator = (CUIImage&&) = delete;

		void Initialize() final;
		void DrawSetup() final;
		void SetTexCoord(const Math::Vector4& texCoord);
		void SetTexIndex(u32 index);
		
		bool CommandElement(const CUICommandList::StateElem& stateElem) final;
		static bool SetFullDataFromKeyValue(CUIImage* pImage, const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::wstring& key, const std::wstring& value, Data& data);
		static bool SetDataFromKeyValue(CUIImage* pImage, const CUIScript* pScript, const std::wstring& key, const std::wstring& value, Data& data);
		
		// Accessors.
		virtual inline u32 GetInstanceMax() const { return m_data.bSplitImage ? (m_data.bFillCenter ? 9 : 8) : 1; }
		virtual inline u32 GetInstanceCount() const { return IsActive() ? GetInstanceMax() : 0; }

		inline u64 GetMaterialHash() const final { return m_data.matHash; }
		inline u64 GetTextureHash() const final { return m_data.texHash; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		
	private:
		void SetupSingle();
		void SetupSplit();

		void OnIndexChange(const void* params);

	private:
		void BuildFromSection(const CUIScript::Section* pSection) final;

		// Accessors.
		inline const RendererData& GetRendererData() const final { return m_data; }

	private:
		Data m_data;
		Math::Vector2 m_cellSize;
	};
};

#endif
