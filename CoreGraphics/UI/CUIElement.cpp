//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIElement.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIElement.h"
#include "CUICanvas.h"
#include "CUIFrame.h"
#include "../Application/CSceneManager.h"
#include "../Graphics/CGraphicsAPI.h"
#include "../Factory/CFactory.h"
#include <Utilities/CConvertUtil.h>
#include <unordered_map>

namespace UI
{
	CUIElement::CUIElement(const CVObject* pObject, CUICanvas* pCanvas, const CUIScript::Section* pSection) :
		CVComponent(pObject),
		m_frame(pObject, pCanvas),
		m_pSection(pSection)
	{
		m_frame.SetOnUpdate(std::bind(&CUIElement::OnUpdate, this));
		m_frame.SetOnActiveChange(std::bind(&CUIElement::OnActiveChange, this, std::placeholders::_1));
	}

	CUIElement::~CUIElement()
	{
	}
	
	void CUIElement::Initialize()
	{
		if(m_pSection)
		{
			BuildFromSection(m_pSection);
		}

		m_frame.SetData(GetElementData().frameData);
		m_frame.Initialize();
	}
	
	void CUIElement::DebugUpdate()
	{
		m_frame.DebugUpdate();
	}
	
	//-----------------------------------------------------------------------------------------------
	// CUICommand methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUIElement::CommandElement(const CUICommandList::StateElem& stateElem)
	{
		struct MapData
		{
			const CUICommandList::StateElem& stateElem;
			CUIFrame* pFrame;
			ElementData& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> ELEM_MAP {
			{ L"position", [](const MapData& mapData) { mapData.pFrame->SetPosition(CUIElement::CommandElementVector<Math::Vector2>(mapData.stateElem, mapData.pFrame->GetPosition())); } },
			{ L"size", [](const MapData& mapData) { mapData.pFrame->SetSize(CUIElement::CommandElementVector<Math::Vector2>(mapData.stateElem, mapData.pFrame->GetSize())); } },
			
			{ L"left", [](const MapData& mapData) { mapData.pFrame->SetLeftExtent(mapData.stateElem.val.f); } },
			{ L"right", [](const MapData& mapData) { mapData.pFrame->SetRightExtent(mapData.stateElem.val.f); } },
			{ L"top", [](const MapData& mapData) { mapData.pFrame->SetTopExtent(mapData.stateElem.val.f); } },
			{ L"bottom", [](const MapData& mapData) { mapData.pFrame->SetBottomExtent(mapData.stateElem.val.f); } },

			{ L"minWidth", [](const MapData& mapData) { mapData.pFrame->SetMinWidth(mapData.stateElem.val.f); } },
			{ L"maxWidth", [](const MapData& mapData) { mapData.pFrame->SetMaxWidth(mapData.stateElem.val.f); } },
			{ L"minHeight", [](const MapData& mapData) { mapData.pFrame->SetMinHeight(mapData.stateElem.val.f); } },
			{ L"maxHeight", [](const MapData& mapData) { mapData.pFrame->SetMaxHeight(mapData.stateElem.val.f); } },

			{ L"anchorMin", [](const MapData& mapData) { mapData.pFrame->SetAnchorMin(CUIElement::CommandElementVector<Math::Vector2>(mapData.stateElem, mapData.pFrame->GetAnchorMin())); } },
			{ L"anchorMax", [](const MapData& mapData) { mapData.pFrame->SetAnchorMax(CUIElement::CommandElementVector<Math::Vector2>(mapData.stateElem, mapData.pFrame->GetAnchorMax())); } },
			{ L"pivot", [](const MapData& mapData) { mapData.pFrame->SetPivot(CUIElement::CommandElementVector<Math::Vector2>(mapData.stateElem, mapData.pFrame->GetPivot())); } },

			{ L"scale", [](const MapData& mapData) { mapData.pFrame->SetScale(CUIElement::CommandElementVector<Math::Vector2>(mapData.stateElem, mapData.pFrame->GetScale())); } },
			{ L"rotation", [](const MapData& mapData) { mapData.pFrame->SetRotation(mapData.stateElem.val.f); } },

			{ L"active", [](const MapData& mapData) { mapData.pFrame->SetActive(mapData.stateElem.val.b); } },
			{ L"bAllowOverlap", [](const MapData& mapData) { mapData.pFrame->SetAllowOverlap(mapData.stateElem.val.b); } },
		};
		
		const auto& elem = ELEM_MAP.find(stateElem.GetSubElemAt(0));
		if(elem == ELEM_MAP.end()) return false;

		elem->second({ stateElem, &m_frame, const_cast<ElementData&>(GetElementData()) });
		return true;
	}

	//-----------------------------------------------------------------------------------------------
	// Section methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUIElement::SetDataFromKeyValue(const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::wstring& key, const std::wstring& value, ElementData& data)
	{
		struct MapData
		{
			const CUIScript* pScript;
			const Util::CScriptObject::PropBlock& prop;
			const std::wstring& value;
			ElementData& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> ELEM_MAP {
			{ L"bAllowOverlap", [](const MapData& mapData) { mapData.data.frameData.bAllowOverflow = Util::StringToBool(mapData.value); } },
			{ L"depth", [](const MapData& mapData) { mapData.data.depth = Util::StringToLong(mapData.value); } },
			{ L"regCommand", [](const MapData& mapData) { App::CSceneManager::Instance().UIEventSystem().CommandList().RegisterCommand(mapData.pScript, mapData.prop); } },
		};

		const auto& elem = ELEM_MAP.find(key);
		if(elem == ELEM_MAP.end()) return false;

		elem->second({ pScript, prop, value, data });
		return true;
	}

	void CUIElement::BuildFromSection(const CUIScript::Section* pSection)
	{
		CUIFrame::Data& frameData = const_cast<ElementData&>(GetElementData()).frameData;

		frameData = pSection->frameData;

		if(pSection->parentIndex != SIZE_MAX)
		{
			frameData.pParent = pSection->pScript->GetSection(pSection->parentIndex).pUIElement->GetFrame();
		}
		
		if(pSection->typeProp)
		{
			ElementData& data = const_cast<ElementData&>(GetElementData());
			
			if(pSection->typeProp)
			{
				if(pSection->cls.size())
				{
					CUIScript::ProcessBlock(*pSection->pScript->GetClass(pSection->cls).typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
						SetDataFromKeyValue(pSection->pScript, prop, str[0], prop.propList[1], data);
					});
				}

				CUIScript::ProcessBlock(*pSection->typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
					SetDataFromKeyValue(pSection->pScript, prop, str[0], prop.propList[1], data);
				});
			}
		}
	}
};
