//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIRenderer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIRenderer.h"
#include "CUICanvas.h"
#include <Utilities/CConvertUtil.h>

namespace UI
{
	CUIRenderer::CUIRenderer(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection) : 
		CUIElement(pObject, pCanvas, pSection),
		//m_bActive(true),
		m_bDirty(false),
		m_pQuad(nullptr)
	{
	}

	CUIRenderer::~CUIRenderer()
	{
	}
	
	void CUIRenderer::Reset()
	{
		m_pQuad->ForceReset();
		m_pInstanceList.clear();
	}

	void CUIRenderer::DrawSetup()
	{
		m_bDirty = false;
	}

	void CUIRenderer::OnRegistration(CUIQuad* pQuad)
	{
		m_pQuad = pQuad;
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUIRenderer::OnUpdate()
	{
		MarkAsDirty();
	}

	void CUIRenderer::OnActiveChange(bool bActive)
	{
		m_pQuad->ForceReset();
	}
	
	void CUIRenderer::MarkAsDirty()
	{
		if(m_bDirty || m_pQuad == nullptr) return;

		m_pQuad->AddDirty(this);
		m_bDirty = true;
	}

	CUIQuad::Instance* CUIRenderer::PullInstance(u32 index)
	{
		while(static_cast<u32>(m_pInstanceList.size()) <= index)
		{
			m_pInstanceList.push_back(m_pQuad->PullInstance());
		}

		return m_pInstanceList[index];
	}
	
	//-----------------------------------------------------------------------------------------------
	// CUICommand methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUIRenderer::CommandElement(const CUICommandList::StateElem& stateElem)
	{
		if(CUIElement::CommandElement(stateElem))
		{
			return true;
		}

		struct MapData
		{
			const CUICommandList::StateElem& stateElem;
			CUIRenderer* pRenderer;
			RendererData& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> RENDERER_MAP {
			{ L"color", [](const MapData& mapData) { mapData.pRenderer->SetColor(CUIElement::CommandElementVector<Math::Color>(mapData.stateElem, mapData.data.color)); } },
		};
		
		const auto& elem = RENDERER_MAP.find(stateElem.GetSubElemAt(0));
		if(elem == RENDERER_MAP.end()) return false;

		elem->second({ stateElem, this, const_cast<RendererData&>(GetRendererData()) });
		return true;
	}

	//-----------------------------------------------------------------------------------------------
	// Section methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUIRenderer::SetDataFromKeyValue(const CUIScript* pScript, const std::wstring& key, const std::wstring& value, RendererData& data)
	{
		struct MapData
		{
			const CUIScript* pScript;
			const std::wstring& prop;
			RendererData& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> RENDERER_MAP {
			{ L"color", [](const MapData& mapData) { mapData.data.color = Util::StringToVector<Math::Color>(mapData.prop); } },
		};

		const auto& elem = RENDERER_MAP.find(key);
		if(elem == RENDERER_MAP.end()) return false;

		elem->second({ pScript, value, data });
		return true;
	}

	void CUIRenderer::BuildFromSection(const CUIScript::Section* pSection)
	{
		CUIElement::BuildFromSection(pSection);

		if(pSection->typeProp)
		{
			RendererData& data = const_cast<RendererData&>(GetRendererData());
			
			if(pSection->typeProp)
			{
				if(pSection->cls.size())
				{
					CUIScript::ProcessBlock(*pSection->pScript->GetClass(pSection->cls).typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
						SetDataFromKeyValue(pSection->pScript, str[0], prop.propList[1], data);
					});
				}

				CUIScript::ProcessBlock(*pSection->typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
					SetDataFromKeyValue(pSection->pScript, str[0], prop.propList[1], data);
				});
			}
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Property methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUIRenderer::SetColor(const Math::Color& color)
	{
		RendererData& renderData = const_cast<RendererData&>(GetRendererData());
		renderData.color = color;
		MarkAsDirty();
	}
};
