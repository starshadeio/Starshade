//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUISelect.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUISelect.h"
#include "CUICanvas.h"
#include "CUIFrame.h"
#include "../Application/CSceneManager.h"
#include <Utilities/CConvertUtil.h>
#include <unordered_map>

namespace UI
{
	CUISelect::CUISelect(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection) : 
		CUIElement(pObject, pCanvas, pSection),
		m_bHovered(false),
		m_pCanvas(pCanvas)
	{
	}

	CUISelect::~CUISelect()
	{
	}
	
	//-----------------------------------------------------------------------------------------------
	// CUICommand methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUISelect::CommandElement(const CUICommandList::StateElem& stateElem)
	{
		if(CUIElement::CommandElement(stateElem))
		{
			return true;
		}

		struct MapData
		{
			const CUICommandList::StateElem& stateElem;
			CUISelect* pImage;
			SelectData& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> SELECT_MAP {
		};
		
		const auto& elem = SELECT_MAP.find(stateElem.GetSubElemAt(0));
		if(elem == SELECT_MAP.end()) return false;

		elem->second({ stateElem, this, const_cast<SelectData&>(GetSelectData()) });

		return true;
	}

	//-----------------------------------------------------------------------------------------------
	// Section methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUISelect::SetDataFromKeyValue(const CUIScript* pScript, const std::wstring& key, const std::wstring& value, SelectData& data)
	{
		struct MapData
		{
			const CUIScript* pScript;
			const std::wstring& prop;
			SelectData& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> RENDERER_MAP {
			{ L"tooltip", [](const MapData& mapData) { mapData.data.bHasTooltip = true; mapData.data.tooltipHash = Math::FNV1a_64(mapData.prop.c_str(), mapData.prop.size()); } },
			{ L"keybind", [](const MapData& mapData) {
				mapData.data.bHasKeybinding = true;
				
				Util::SplitString(mapData.prop, L':', [&](u32 index, const std::wstring& str){
					switch(index)
					{
						case 0:
							mapData.data.inputLayoutHash = Math::FNV1a_32(str.c_str(), str.size()); 
							break;
						case 1:
							mapData.data.keybindHash = Math::FNV1a_32(str.c_str(), str.size()); 
							break;
						default:
							break;
					}
				});
			} },
		};

		const auto& elem = RENDERER_MAP.find(key);
		if(elem == RENDERER_MAP.end()) return false;

		elem->second({ pScript, value, data });
		return true;
	}

	void CUISelect::BuildFromSection(const CUIScript::Section* pSection)
	{
		CUIElement::BuildFromSection(pSection);

		if(pSection->typeProp)
		{
			SelectData& data = const_cast<SelectData&>(GetSelectData());
			
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
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUISelect::TestPoint(const Math::Vector2& point)
	{
		bool bHovered = false;

		auto pLastSelected = App::CSceneManager::Instance().UIEventSystem().GetHovered();
		if((!pLastSelected || pLastSelected->GetElementData().depth < GetElementData().depth) && GetFrame()->IsIntersecting(point, &m_relativeCursorPt))
		{
			pLastSelected = this;
			bHovered = true;
		}

		return bHovered;
	}
};
