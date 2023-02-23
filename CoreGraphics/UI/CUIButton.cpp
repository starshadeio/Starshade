//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIButton.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIButton.h"
#include "CUICanvas.h"
#include "CUIFrame.h"
#include "../Utilities/CDebug.h"
#include <Application/CCommandManager.h>
#include <Utilities/CConvertUtil.h>

namespace UI
{
	CUIButton::CUIButton(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection) :
		CUISelect(pObject, pCanvas, pSection),
		m_background(pObject, pCanvas),
		m_foreground(pObject, pCanvas),
		m_text(pObject, pCanvas)
	{
	}

	CUIButton::~CUIButton()
	{
	}

	void CUIButton::OnRegister(CUICanvas* pCanvas)
	{
		CUISelect::OnRegister(pCanvas);

		CUIFrame::Data frameData { };
		frameData.anchorMin = 0.0f;
		frameData.anchorMax = 1.0f;
		frameData.pivot = 0.5f;
		frameData.pParent = GetFrame();

		if(m_data.bUseBackgroud)
		{ // Setup UI image.
			m_data.bgData.frameData = frameData;
			m_data.bgData.frameData.rect[0] = { m_data.bgInset.x, m_data.bgInset.z };
			m_data.bgData.frameData.rect[1] = { m_data.bgInset.y, m_data.bgInset.w };
			m_background.SetData(m_data.bgData);
			m_background.Initialize();

			pCanvas->RegisterElement(&m_background);
		}
		
		if(m_data.bUseForegroud)
		{ // Setup UI image.
			m_data.fgData.frameData = frameData;
			m_data.fgData.frameData.rect[0] = { m_data.fgInset.x, m_data.fgInset.z };
			m_data.fgData.frameData.rect[1] = { m_data.fgInset.y, m_data.fgInset.w };
			m_data.fgData.depth = m_data.bgData.depth + 1;
			m_foreground.SetData(m_data.fgData);
			m_foreground.Initialize();

			pCanvas->RegisterElement(&m_foreground);
		}
		
		if(m_data.bUseText)
		{ // Setup UI text.
			m_data.txData.frameData = frameData;
			m_data.txData.frameData.rect[0] = { m_data.txInset.x, m_data.txInset.z };
			m_data.txData.frameData.rect[1] = { m_data.txInset.y, m_data.txInset.w };
			m_text.SetData(m_data.txData);
			m_text.Initialize();
			
			pCanvas->RegisterElement(&m_text);
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// CUICommand methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUIButton::CommandElement(const CUICommandList::StateElem& stateElem)
	{
		if(CUISelect::CommandElement(stateElem))
		{
			return true;
		}

		struct MapData
		{
			const CUICommandList::StateElem& stateElem;
			CUIButton* pButton;
			Data& data;
		};

		typedef std::function<void(const MapData&)> BUTTON_FUNC;
		static const std::unordered_map<std::wstring, BUTTON_FUNC> BUTTON_MAP {
			{ L"bEnable", [](const MapData& mapData) { mapData.pButton->SetEnable(mapData.stateElem.val.b); } },
			{ L"bHold", [](const MapData& mapData) { mapData.pButton->SetHold(mapData.stateElem.val.b); } },

			{ L"bgColorEnter", [](const MapData& mapData) { mapData.data.bgColorEnter = CUIElement::CommandElementVector<Math::Color>(mapData.stateElem, mapData.data.bgColorEnter); } },
			{ L"bgColorExit", [](const MapData& mapData) { mapData.data.bgColorExit = CUIElement::CommandElementVector<Math::Color>(mapData.stateElem, mapData.data.bgColorExit); } },
			{ L"bgColorClick", [](const MapData& mapData) { mapData.data.bgColorClick = CUIElement::CommandElementVector<Math::Color>(mapData.stateElem, mapData.data.bgColorClick); } },
			{ L"bgColorDisable", [](const MapData& mapData) { mapData.data.bgColorDisable = CUIElement::CommandElementVector<Math::Color>(mapData.stateElem, mapData.data.bgColorDisable); } },

			{ L"fgColorEnter", [](const MapData& mapData) { mapData.data.fgColorEnter = CUIElement::CommandElementVector<Math::Color>(mapData.stateElem, mapData.data.fgColorEnter); } },
			{ L"fgColorExit", [](const MapData& mapData) { mapData.data.fgColorExit = CUIElement::CommandElementVector<Math::Color>(mapData.stateElem, mapData.data.fgColorExit); } },
			{ L"fgColorClick", [](const MapData& mapData) { mapData.data.fgColorClick = CUIElement::CommandElementVector<Math::Color>(mapData.stateElem, mapData.data.fgColorClick); } },
			{ L"fgColorDisable", [](const MapData& mapData) { mapData.data.fgColorDisable = CUIElement::CommandElementVector<Math::Color>(mapData.stateElem, mapData.data.fgColorDisable); } },

			{ L"txColorEnter", [](const MapData& mapData) { mapData.data.txColorEnter = CUIElement::CommandElementVector<Math::Color>(mapData.stateElem, mapData.data.txColorEnter); } },
			{ L"txColorExit", [](const MapData& mapData) { mapData.data.txColorExit = CUIElement::CommandElementVector<Math::Color>(mapData.stateElem, mapData.data.txColorExit); } },
			{ L"txColorClick", [](const MapData& mapData) { mapData.data.txColorClick = CUIElement::CommandElementVector<Math::Color>(mapData.stateElem, mapData.data.txColorClick); } },
			{ L"txColorDisable", [](const MapData& mapData) { mapData.data.txColorDisable = CUIElement::CommandElementVector<Math::Color>(mapData.stateElem, mapData.data.txColorDisable); } },

			{ L"fg", [](const MapData& mapData) { mapData.pButton->m_foreground.CommandElement(mapData.stateElem(1)); } },
			//6{ L"bg", [](const MapData& mapData) { mapData.pButton->m_background.CommandElement(mapData.stateElem(1)); } },
			{ L"tx", [](const MapData& mapData) { mapData.pButton->m_text.CommandElement(mapData.stateElem(1)); } },
			
			{ L"onEnter", [](const MapData& mapData) { mapData.data.onEnterHash = Math::FNV1a_64(mapData.stateElem.valStr.c_str(), mapData.stateElem.valStr.size()); } },
			{ L"onExit", [](const MapData& mapData) { mapData.data.onExitHash = Math::FNV1a_64(mapData.stateElem.valStr.c_str(), mapData.stateElem.valStr.size()); } },
			{ L"onPress", [](const MapData& mapData) { mapData.data.onPressHash = Math::FNV1a_64(mapData.stateElem.valStr.c_str(), mapData.stateElem.valStr.size()); } },
			{ L"onRelease", [](const MapData& mapData) { mapData.data.onReleaseHash = Math::FNV1a_64(mapData.stateElem.valStr.c_str(), mapData.stateElem.valStr.size()); } },
		};
		
		const auto& elem = BUTTON_MAP.find(stateElem.GetSubElemAt(0));
		if(elem == BUTTON_MAP.end()) return false;

		elem->second({ stateElem, this, m_data });

		return true;
	}

	//-----------------------------------------------------------------------------------------------
	// Section methods.
	//-----------------------------------------------------------------------------------------------

	bool CUIButton::SetDataFromKeyValue(CUIButton* pButton, const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::vector<std::wstring>& keys, const std::wstring& value, Data& data)
	{
		struct MapData
		{
			CUIButton* pButton;
			const CUIScript* pScript;
			const Util::CScriptObject::PropBlock& prop;
			const std::vector<std::wstring>& keys;
			const std::wstring& value;
			Data& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> BUTTON_MAP {
			{ L"bEnable", [](const MapData& mapData) { mapData.data.bEnable = Util::StringToBool(mapData.value); } },
			{ L"bHold", [](const MapData& mapData) { mapData.data.bHold = Util::StringToBool(mapData.value); } },

			{ L"bUseBackgroud", [](const MapData& mapData) { mapData.data.bUseBackgroud = Util::StringToBool(mapData.value); } },
			{ L"bUseForegroud", [](const MapData& mapData) { mapData.data.bUseForegroud = Util::StringToBool(mapData.value); } },
			{ L"bUseText", [](const MapData& mapData) { mapData.data.bUseText = Util::StringToBool(mapData.value); } },

			{ L"bgInset", [](const MapData& mapData) { mapData.data.bgInset = Util::StringToVector<Math::Vector4>(mapData.value); } },
			{ L"fgInset", [](const MapData& mapData) { mapData.data.fgInset = Util::StringToVector<Math::Vector4>(mapData.value); } },
			{ L"txInset", [](const MapData& mapData) { mapData.data.txInset = Util::StringToVector<Math::Vector4>(mapData.value); } },

			{ L"bgColorEnter", [](const MapData& mapData) { mapData.data.bgColorEnter = Util::StringToVector<Math::Color>(mapData.value); } },
			{ L"bgColorExit", [](const MapData& mapData) { mapData.data.bgColorExit = Util::StringToVector<Math::Color>(mapData.value); } },
			{ L"bgColorClick", [](const MapData& mapData) { mapData.data.bgColorClick = Util::StringToVector<Math::Color>(mapData.value); } },
			{ L"bgColorDisable", [](const MapData& mapData) { mapData.data.bgColorDisable = Util::StringToVector<Math::Color>(mapData.value); } },

			{ L"fgColorEnter", [](const MapData& mapData) { mapData.data.fgColorEnter = Util::StringToVector<Math::Color>(mapData.value); } },
			{ L"fgColorExit", [](const MapData& mapData) { mapData.data.fgColorExit = Util::StringToVector<Math::Color>(mapData.value); } },
			{ L"fgColorClick", [](const MapData& mapData) { mapData.data.fgColorClick = Util::StringToVector<Math::Color>(mapData.value); } },
			{ L"fgColorDisable", [](const MapData& mapData) { mapData.data.fgColorDisable = Util::StringToVector<Math::Color>(mapData.value); } },

			{ L"txColorEnter", [](const MapData& mapData) { mapData.data.txColorEnter = Util::StringToVector<Math::Color>(mapData.value); } },
			{ L"txColorExit", [](const MapData& mapData) { mapData.data.txColorExit = Util::StringToVector<Math::Color>(mapData.value); } },
			{ L"txColorClick", [](const MapData& mapData) { mapData.data.txColorClick = Util::StringToVector<Math::Color>(mapData.value); } },
			{ L"txColorDisable", [](const MapData& mapData) { mapData.data.txColorDisable = Util::StringToVector<Math::Color>(mapData.value); } },

			{ L"bg", [](const MapData& mapData) { CUIImage::SetFullDataFromKeyValue(&mapData.pButton->m_background, mapData.pScript, mapData.prop, mapData.keys[1], mapData.value, mapData.data.bgData); } },
			{ L"fg", [](const MapData& mapData) { CUIImage::SetFullDataFromKeyValue(&mapData.pButton->m_foreground, mapData.pScript, mapData.prop, mapData.keys[1], mapData.value, mapData.data.fgData); } },
			{ L"tx", [](const MapData& mapData) { CUIText::SetFullDataFromKeyValue(mapData.pScript, mapData.prop, mapData.keys[1], mapData.value, mapData.data.txData); } },
			
			{ L"onEnter", [](const MapData& mapData) { mapData.data.onEnterHash = Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()); } },
			{ L"onExit", [](const MapData& mapData) { mapData.data.onExitHash = Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()); } },
			{ L"onPress", [](const MapData& mapData) { mapData.data.onPressHash = Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()); } },
			{ L"onRelease", [](const MapData& mapData) { mapData.data.onReleaseHash = Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()); } },

			{ L"actionEnable", [](const MapData& mapData) { App::CCommandManager::Instance().RegisterAction(Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()), std::bind(&CUIButton::SetEnable, mapData.pButton, true)); } },
			{ L"actionDisable", [](const MapData& mapData) { App::CCommandManager::Instance().RegisterAction(Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()), std::bind(&CUIButton::SetEnable, mapData.pButton, false)); } },

			{ L"actionHold", [](const MapData& mapData) { App::CCommandManager::Instance().RegisterAction(Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()), std::bind(&CUIButton::SetHold, mapData.pButton, true)); } },
			{ L"actionRelease", [](const MapData& mapData) { App::CCommandManager::Instance().RegisterAction(Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()), std::bind(&CUIButton::SetHold, mapData.pButton, false)); } },
		};

		const auto& elem = BUTTON_MAP.find(keys[0]);
		if(elem == BUTTON_MAP.end()) return false;

		elem->second({ pButton, pScript, prop, keys, value, data });
		return true;
	}
		
	void CUIButton::BuildFromSection(const CUIScript::Section* pSection)
	{
		CUISelect::BuildFromSection(pSection);
		
		if(pSection->cls.size())
		{
			CUIScript::ProcessBlock(*pSection->pScript->GetClass(pSection->cls).typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
				SetDataFromKeyValue(this, pSection->pScript, prop, str, prop.propList[1], m_data);
			});
		}

		CUIScript::ProcessBlock(*pSection->typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
			SetDataFromKeyValue(this, pSection->pScript, prop, str, prop.propList[1], m_data);
		});

		const bool bEnable = m_data.bEnable;
		m_data.bEnable = !m_data.bEnable;
		SetEnable(bEnable);

		if(bEnable)
		{
			const bool bHold = m_data.bHold;
			m_data.bHold = !m_data.bHold;
			SetHold(bHold);
		}
	}

	//-----------------------------------------------------------------------------------------------
	// State methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUIButton::SetEnable(bool bEnable)
	{
		if(m_data.bEnable == bEnable) return;
		if(m_data.bEnable = bEnable)
		{
			SetBackgroundColor(m_data.bgColorExit);
			SetForegroundColor(m_data.fgColorExit);
			SetTextColor(m_data.txColorExit);
		}
		else
		{
			SetBackgroundColor(m_data.bgColorDisable);
			SetForegroundColor(m_data.fgColorDisable);
			SetTextColor(m_data.txColorDisable);
		}
	}

	void CUIButton::SetHold(bool bHold)
	{
		if(m_data.bHold == bHold) return;
		if(m_data.bHold = bHold)
		{
			SetBackgroundColor(m_data.bgColorClick);
			SetForegroundColor(m_data.fgColorClick);
			SetTextColor(m_data.txColorClick);
		}
		else
		{
			SetBackgroundColor(m_data.bgColorExit);
			SetForegroundColor(m_data.fgColorExit);
			SetTextColor(m_data.txColorExit);
		}
	}

	//-----------------------------------------------------------------------------------------------
	// State methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUIButton::OnEnter()
	{
		if(!m_data.bEnable) return;
		if(m_data.bHold && !m_data.bAllowHoldToggle) return;

		if(!m_data.bHold)
		{
			SetBackgroundColor(m_data.bgColorEnter);
			SetForegroundColor(m_data.fgColorEnter);
			SetTextColor(m_data.txColorEnter);
		}

		if(m_data.onEnterHash)
		{
			App::CCommandManager::Instance().Execute(m_data.onEnterHash);
		}
	}

	void CUIButton::OnExit()
	{
		if(!m_data.bEnable) return;
		if(m_data.bHold && !m_data.bAllowHoldToggle) return;
		
		if(!m_data.bHold)
		{
			SetBackgroundColor(m_data.bgColorExit);
			SetForegroundColor(m_data.fgColorExit);
			SetTextColor(m_data.txColorExit);
		}
		
		if(m_data.onExitHash)
		{
			App::CCommandManager::Instance().Execute(m_data.onExitHash);
		}
	}

	bool CUIButton::OnSelect(const App::InputCallbackData& callback)
	{
		if(!m_data.bEnable) return false;
		if(m_data.bHold && !m_data.bAllowHoldToggle) return false;

		if(callback.bPressed)
		{
			if(!m_data.bHold)
			{
				SetBackgroundColor(m_data.bgColorClick);
				SetForegroundColor(m_data.fgColorClick);
				SetTextColor(m_data.txColorClick);
			}

			if(m_data.onPressHash)
			{
				App::CCommandManager::Instance().Execute(m_data.onPressHash);
			}
		}
		else
		{
			if(!m_data.bHold)
			{
				SetBackgroundColor(m_data.bgColorEnter);
				SetForegroundColor(m_data.fgColorEnter);
				SetTextColor(m_data.txColorEnter);
			}

			if(m_data.onReleaseHash)
			{
				App::CCommandManager::Instance().Execute(m_data.onReleaseHash);
			}
		}

		return true;
	}
};
