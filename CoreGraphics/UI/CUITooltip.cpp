//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUITooltip.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUITooltip.h"
#include "CUICanvas.h"
#include "CUIFrame.h"
#include "CUISelect.h"
#include "../Application/CSceneManager.h"
#include "../Application/CInput.h"
#include <Application/CCommandManager.h>
#include <Application/CLocalization.h>
#include <Utilities/CConvertUtil.h>

namespace UI
{
	CUITooltip::CUITooltip(const CVObject* pObject, CUICanvas* pCanvas, const CUIScript::Section* pSection) :
		CUIElement(pObject, pCanvas, pSection),
		m_text(pObject, pCanvas)
	{
	}

	CUITooltip::~CUITooltip()
	{
	}

	void CUITooltip::OnRegister(CUICanvas* pCanvas)
	{
		CUIFrame::Data frameData { };
		frameData.anchorMin = 0.0f;
		frameData.anchorMax = 1.0f;
		frameData.pivot = 0.5f;
		frameData.pParent = GetFrame();
		
		m_data.txData.frameData = frameData;
		m_text.SetData(m_data.txData);
		m_text.Initialize();
			
		pCanvas->RegisterElement(&m_text);
		
		App::CSceneManager::Instance().UIEventSystem().RegisterTooltip(this);
	}
	
	void CUITooltip::Release()
	{
		CUIElement::Release();

		App::CSceneManager::Instance().UIEventSystem().DeregisterTooltip(this);
	}

	void CUITooltip::SetTooltip(const CUISelect* pSelect)
	{
		if(pSelect && pSelect->HasTooltip())
		{
			if(pSelect->GetKeybindHash())
			{
				m_text.SetText(App::CLocalization::Instance().Get(pSelect->GetTooltipHash()).Format({
					L"(" + App::CInput::Instance().KeybindingString(pSelect->GetInputLayoutHash(), pSelect->GetKeybindHash()) + L")"
				}).c_str());
			}
			else
			{
				m_text.SetText(App::CLocalization::Instance().Get(pSelect->GetTooltipHash()).Get().c_str());
			}
		}
		else
		{
			m_text.SetText(L"");
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Section methods.
	//-----------------------------------------------------------------------------------------------

	bool CUITooltip::SetDataFromKeyValue(CUITooltip* pTooltip, const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::vector<std::wstring>& keys, const std::wstring& value, Data& data)
	{
		struct MapData
		{
			CUITooltip* pTooltip;
			const CUIScript* pScript;
			const Util::CScriptObject::PropBlock& prop;
			const std::vector<std::wstring>& keys;
			const std::wstring& value;
			Data& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> TOOLTIP_MAP {
			{ L"tx", [](const MapData& mapData) { CUIText::SetFullDataFromKeyValue(mapData.pScript, mapData.prop, mapData.keys[1], mapData.value, mapData.data.txData); } },
		};

		const auto& elem = TOOLTIP_MAP.find(keys[0]);
		if(elem == TOOLTIP_MAP.end()) return false;

		elem->second({ pTooltip, pScript, prop, keys, value, data });
		return true;
	}
		
	void CUITooltip::BuildFromSection(const CUIScript::Section* pSection)
	{
		CUIElement::BuildFromSection(pSection);

		if(pSection->cls.size())
		{
			CUIScript::ProcessBlock(*pSection->pScript->GetClass(pSection->cls).typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
				SetDataFromKeyValue(this, pSection->pScript, prop, str, prop.propList[1], m_data);
			});
		}

		CUIScript::ProcessBlock(*pSection->typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
			SetDataFromKeyValue(this, pSection->pScript, prop, str, prop.propList[1], m_data);
		});
	}
};
