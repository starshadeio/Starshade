//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUICanvas.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUICanvas.h"
#include "CUIRenderer.h"
#include "CUIImage.h"
#include "CUIGrid.h"
#include "CUIText.h"
#include "CUIQuad.h"
#include "CUISelect.h"
#include "../Application/CSceneManager.h"
#include "../Graphics/CGraphicsAPI.h"
#include "../Factory/CFactory.h"
#include <Utilities/CMemoryFree.h>
#include <cassert>
#include <algorithm>

namespace UI
{
	CUICanvas::CUICanvas(const CVObject* pObject) : 
		CUIElement(pObject, this, nullptr),
		m_bActive(true)
	{
	}

	CUICanvas::~CUICanvas()
	{
	}
	
	void CUICanvas::Initialize()
	{
		CUIElement::Initialize();

		App::CSceneManager::Instance().RenderingSystem().Register(this);
		App::CSceneManager::Instance().UIEventSystem().RegisterCanvas(this);

		for(auto& elem0 : m_uiRenderers)
		{
			for(auto& elem1 : elem0.second)
			{
				if(elem1.second.empty()) continue;

				for(auto& elem2 : elem1.second)
				{
					if(elem2.second.empty()) continue;

					m_uiQuadList.push_back(new CUIQuad(GetObject()));
					CUIQuad& quad = *m_uiQuadList.back();
					CUIQuad::Data data { };
					data.matHash = elem0.first;
					data.texHash = elem1.first;
					quad.SetData(data);

					for(auto pRenderer : elem2.second)
					{
						auto pText = dynamic_cast<CUIText*>(pRenderer);
						if(pText)
						{
							quad.RegisterText(pText);
						}
						else
						{
							quad.RegisterRenderer(pRenderer);
						}

						pRenderer->OnRegistration(&quad);
					}

					quad.Initialize();
				}
			}
		}

		std::sort(m_uiQuadList.begin(), m_uiQuadList.end(), [](CUIQuad* a, CUIQuad* b){
			return a->GetRendererAt(0)->GetDepth() < b->GetRendererAt(0)->GetDepth();
		});
		
		GetFrame()->SetSize({ CFactory::Instance().GetGraphicsAPI()->GetWidth(), CFactory::Instance().GetGraphicsAPI()->GetHeight() });
	}

	void CUICanvas::UpdateElements()
	{
		m_updateFrames.Update();
	}

	void CUICanvas::Render()
	{
		for(auto& pQuad : m_uiQuadList)
		{
			pQuad->Render();
		}
	}
	
	void CUICanvas::Release()
	{
		for(auto& pQuad : m_uiQuadList)
		{
			SAFE_RELEASE_DELETE(pQuad);
		}

		App::CSceneManager::Instance().UIEventSystem().DeregisterCanvas(this);
		App::CSceneManager::Instance().RenderingSystem().Deregister(this);
	}
	
	void CUICanvas::OnResize(const Math::Rect& rect)
	{
		GetFrame()->SetSize({ static_cast<float>(rect.w), static_cast<float>(rect.h) });

		for(auto& pQuad : m_uiQuadList)
		{
			pQuad->ForceReset();
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// UI element registration methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUICanvas::RegisterElement(CUIElement* pElement)
	{
		CUIRenderer* pRenderer = dynamic_cast<CUIRenderer*>(pElement);
		CUISelect* pSelectable = dynamic_cast<CUISelect*>(pElement);

		if(pRenderer != nullptr)
		{ // Registering a CUIRenderer.
			auto elem0 = m_uiRenderers.find(pRenderer->GetMaterialHash());
			if(elem0 == m_uiRenderers.end())
			{
				elem0 = m_uiRenderers.insert({ pRenderer->GetMaterialHash(), std::unordered_map<u64, std::unordered_map<s32, std::vector<CUIRenderer*>>>() }).first;
			}
		
			auto elem1 = elem0->second.find(pRenderer->GetTextureHash());
			if(elem1 == elem0->second.end())
			{
				elem1 = elem0->second.insert({ pRenderer->GetTextureHash(), std::unordered_map<s32, std::vector<CUIRenderer*>>() }).first;
			}
		
			auto elem2 = elem1->second.find(pRenderer->GetDepth());
			if(elem2 == elem1->second.end())
			{
				elem2 = elem1->second.insert({ pRenderer->GetDepth(), std::vector<CUIRenderer*>() }).first;
			}

			elem2->second.push_back(pRenderer);
		}
		else if(pSelectable != nullptr)
		{ // Registering a CUISelect.
			m_uiSelectList.push_back(pSelectable);
		}
		
		pElement->OnRegister(this);
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUICanvas::MarkFrameAsDirty(CUIFrame* pFrame)
	{
		m_updateFrames.Push(pFrame, pFrame->GetLayer());
	}
	
	// Method for testing if a point overlaps any registered CUIElement that allows for it.
	CUISelect* CUICanvas::TestPoint(const Math::Vector2& point)
	{
		CUISelect* pSelected = nullptr;
		for(CUISelect* pSelectable : m_uiSelectList)
		{
			if(!pSelectable->IsActive())
				continue;

			if(pSelectable->TestPoint(point))
			{
				pSelected = pSelectable;
			}
		}

		return pSelected;
	}
};
