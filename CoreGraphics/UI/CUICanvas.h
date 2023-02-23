//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUICanvas.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUICANVAS_H
#define CUICANVAS_H

#include "CUIElement.h"
#include "CUIFrame.h"
#include "../Graphics/CMeshData_.h"
#include <Logic/CCamera.h>
#include <Math/CMathVector2.h>
#include <Objects/CCompQueue.h>
#include <unordered_map>
#include <vector>

namespace Graphics
{
	class CRenderingSystem;
	class CMaterial;
};

namespace UI
{
	class CUICanvas : public CUIElement
	{
	public:
		friend Graphics::CRenderingSystem;

	public:
		enum class Space
		{
			Screen,
			World,
		};

	public:
		struct Data : ElementData
		{
			Space space = Space::Screen;
		};

	public:
		CUICanvas(const CVObject* pObject);
		~CUICanvas();
		CUICanvas(const CUICanvas&) = delete;
		CUICanvas(CUICanvas&&) = delete;
		CUICanvas& operator = (const CUICanvas&) = delete;
		CUICanvas& operator = (CUICanvas&&) = delete;

		void Initialize() final;
		void UpdateElements();
		void Release() final;
		
		void OnResize(const Math::Rect& rect) final;
		
		void RegisterElement(CUIElement* pElement);
		void MarkFrameAsDirty(CUIFrame* pFrame);
		
		class CUISelect* TestPoint(const Math::Vector2& point);

		// Accessors.
		inline const CVObject* GetObject() const { return m_pObject; }
		inline bool IsActive() const { return m_bActive; }

		inline Space GetSpace() const { return m_data.space; }

		// Modifiers.
		inline void SetActive(bool bActive) { m_bActive = bActive; }
		inline void SetData(const Data& data) { m_data = data; }
		
	protected:
		inline const ElementData& GetElementData() const final { return m_data; }

	private:
		void Render();
		
	private:
		bool m_bActive;
		Data m_data;

		std::vector<class CUISelect*> m_uiSelectList;
		std::unordered_map<u64, std::unordered_map<u64, std::unordered_map<s32, std::vector<class CUIRenderer*>>>> m_uiRenderers;
		std::vector<class CUIQuad*> m_uiQuadList;

		CCompQueue<CUIFrame> m_updateFrames;
	};
};

#endif
