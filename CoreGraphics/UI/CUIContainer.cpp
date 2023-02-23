//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIContainer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIContainer.h"
#include <unordered_map>

namespace UI
{
	CUIContainer::CUIContainer(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection) : 
		CUIElement(pObject, pCanvas, pSection)
	{
	}
		
	CUIContainer::~CUIContainer()
	{
	}
	
	//-----------------------------------------------------------------------------------------------
	// Adjustment methods.
	//-----------------------------------------------------------------------------------------------

	void CUIContainer::OnUpdate()
	{
	}
	
	//-----------------------------------------------------------------------------------------------
	// Section methods.
	//-----------------------------------------------------------------------------------------------

	bool CUIContainer::SetDataFromKeyValue(const CUIScript* pScript, const std::wstring& key, const std::wstring& value, Data& data)
	{
		struct MapData
		{
			const CUIScript* pScript;
			const std::wstring& prop;
			Data& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> CONTAINER_MAP {
		};

		const auto& elem = CONTAINER_MAP.find(key);
		if(elem == CONTAINER_MAP.end()) return false;

		elem->second({ pScript, value, data });
		return true;
	}
	
	void CUIContainer::BuildFromSection(const CUIScript::Section* pSection)
	{
		CUIElement::BuildFromSection(pSection);

		if(pSection->typeProp)
		{
			if(pSection->cls.size())
			{
				CUIScript::ProcessBlock(*pSection->pScript->GetClass(pSection->cls).typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
					SetDataFromKeyValue(pSection->pScript, str[0], prop.propList[1], m_data);
				});
			}

			CUIScript::ProcessBlock(*pSection->typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
				SetDataFromKeyValue(pSection->pScript, str[0], prop.propList[1], m_data);
			});
		}
	}
};
