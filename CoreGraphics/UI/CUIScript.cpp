//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIScript.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIScript.h"
#include "CUICanvas.h"
#include "CUIContainer.h"
#include "CUIImage.h"
#include "CUIGrid.h"
#include "CUIText.h"
#include "CUITooltip.h"
#include "CUIButton.h"
#include "CUIGridSelect.h"
#include <Utilities/CConvertUtil.h>
#include <Utilities/CMemoryFree.h>
#include <Utilities/CDebugError.h>

namespace UI
{
	// Texture.
	typedef std::function<void(const Util::CScriptObject::PropBlock*, CUIScript::Texture&)> TEXTURE_FUNC;

	static const std::unordered_map<std::wstring, TEXTURE_FUNC> TEXTURE_MAP = {
		{ L"texture", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Texture& data) { data.texture = prop; } },
		{ L"cells", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Texture& data) { data.cells = Util::StringToVectorInt<Math::VectorInt2>(prop->propList[1]); } },
	};

	// Section.
	static const std::unordered_map<CUIScript::Type, size_t> TYPE_SIZE_MAP = {
		{ CUIScript::Type::Frame, sizeof(CUIFrame) },
		{ CUIScript::Type::Image, sizeof(CUIImage) },
		{ CUIScript::Type::View, sizeof(CUIImage) },
		{ CUIScript::Type::Grid, sizeof(CUIGrid) },
		{ CUIScript::Type::Text, sizeof(CUIText) },
		{ CUIScript::Type::Tooltip, sizeof(CUITooltip) },
		{ CUIScript::Type::Button, sizeof(CUIButton) },
		{ CUIScript::Type::GridSelect, sizeof(CUIGridSelect) },
	};

	static const std::unordered_map<std::wstring, CUIScript::Type> TYPE_MAP = {
		{ L"frame", CUIScript::Type::Frame },
		{ L"image", CUIScript::Type::Image },
		{ L"view", CUIScript::Type::View },
		{ L"grid", CUIScript::Type::Grid },
		{ L"text", CUIScript::Type::Text },
		{ L"tooltip", CUIScript::Type::Tooltip },
		{ L"button", CUIScript::Type::Button },
		{ L"grid_select", CUIScript::Type::GridSelect },
	};

	typedef std::function<void(const Util::CScriptObject::PropBlock*, CUIScript::Section&)> SECTION_FUNC;

	static const std::unordered_map<std::wstring, SECTION_FUNC> SECTION_MAP = {
		{ L"position", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.position = Util::StringToVector<Math::Vector2>(prop->propList[1]); } },
		{ L"size", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.size = Util::StringToVector<Math::Vector2>(prop->propList[1]); } },

		{ L"left", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.rect[0].x = Util::StringToFloat(prop->propList[1]); } },
		{ L"right", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.rect[0].y = Util::StringToFloat(prop->propList[1]); } },
		{ L"top", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.rect[1].y = Util::StringToFloat(prop->propList[1]); } },
		{ L"bottom", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.rect[1].x = Util::StringToFloat(prop->propList[1]); } },

		{ L"minWidth", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.bUseMinWidth = true; data.frameData.minWidth = Util::StringToFloat(prop->propList[1]); } },
		{ L"maxWidth", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.bUseMaxWidth = true; data.frameData.maxWidth = Util::StringToFloat(prop->propList[1]); } },
		{ L"minHeight", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.bUseMinHeight = true; data.frameData.minHeight = Util::StringToFloat(prop->propList[1]); } },
		{ L"maxHeight", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.bUseMaxHeight = true; data.frameData.maxHeight = Util::StringToFloat(prop->propList[1]); } },
		
		{ L"anchorMin", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.anchorMin = Util::StringToVector<Math::Vector2>(prop->propList[1]); } },
		{ L"anchorMax", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.anchorMax = Util::StringToVector<Math::Vector2>(prop->propList[1]); } },
		{ L"pivot", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.pivot = Util::StringToVector<Math::Vector2>(prop->propList[1]); } },

		{ L"scale", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.scale = Util::StringToVector<Math::Vector2>(prop->propList[1]); } },
		{ L"rotation", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.frameData.rotation = Util::StringToFloat(prop->propList[1]); } },

		{ L"type", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.typeProp = prop; if(prop) { data.type = TYPE_MAP.find(prop->propList[1])->second; } } },
		{ L"frame", [](const Util::CScriptObject::PropBlock* prop, CUIScript::Section& data) { data.children.push_back(prop); } },
	};

	CUIScript::CUIScript(const CVObject* pObject, CUICanvas* pCanvas) :
		m_pObject(pObject),
		m_pCanvas(pCanvas)
	{
	}

	CUIScript::~CUIScript()
	{
	}

	void CUIScript::Build()
	{
		{ // Parse script.
			Util::CScriptObject::Data data { };
			data.filename = m_data.filename;
			m_scriptObject.SetData(data);
			m_scriptObject.Build();
		}

		{ // Create section list and size from pane script data.
			const std::unordered_map<std::wstring, std::function<void(const Util::CScriptObject::PropBlock&)>> PROPBLOCK_MAP = {
				{ L"Texture", std::bind(&CUIScript::ParseTexture, this, std::placeholders::_1) },
				{ L"Class", std::bind(&CUIScript::ParseClass, this, std::placeholders::_1) },
				{ L"UIFrame", std::bind(&CUIScript::ParseFrame, this, std::placeholders::_1, SIZE_MAX) },
			};

			for(const auto& block : m_scriptObject.GetPropBlockList())
			{
				auto elem = PROPBLOCK_MAP.find(block.propList[0]);
				if(elem != PROPBLOCK_MAP.end())
				{
					elem->second(block);
				}
			}
		}

		// Construct UI elements out of section list.
		for(auto& section : m_sectionList)
		{
			CreateElement(section);
		}
	}

	void CUIScript::DebugUpdate()
	{
		for(auto& elem : m_elementMap)
		{
			elem.second->DebugUpdate();
		}
	}

	void CUIScript::Release()
	{
	}
	
	//-----------------------------------------------------------------------------------------------
	// Statics.
	//-----------------------------------------------------------------------------------------------

	void CUIScript::ProcessBlock(const Util::CScriptObject::PropBlock& propBlock, std::function<void(const std::vector<std::wstring>&, const Util::CScriptObject::PropBlock&, const Util::CScriptObject::PropBlock*)> func)
	{
		std::vector<std::wstring> wordList;

		for(size_t i = 0; i < propBlock.children.size(); ++i)
		{
			wordList.clear();

			// Split property string.
			for(size_t l = 0, r = 0; r <= propBlock.children[i].propList[0].size(); ++r)
			{
				if(r == propBlock.children[i].propList[0].size() || propBlock.children[i].propList[0][r] == L'.')
				{
					wordList.push_back(propBlock.children[i].propList[0].substr(l, r - l));
					l = r + 1;
				}
			}

			func(wordList, propBlock.children[i], propBlock.pParent);
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Constuction methods.
	//-----------------------------------------------------------------------------------------------

	void CUIScript::CreateElement(Section& section)
	{
		u64 hash = Math::FNV1a_64(section.name.c_str(), section.name.size());
		std::pair<std::unordered_map<u64, CUIElement*>::iterator, bool> res;
		switch(section.type)
		{
			case Type::Frame:
				res = m_elementMap.insert({ hash, new CUIContainer(m_pObject, m_pCanvas, &section) });
				break;
			case Type::Image:
				res = m_elementMap.insert({ hash, new CUIImage(m_pObject, m_pCanvas, &section) });
				break;
			case Type::View:
				res = m_elementMap.insert({ hash, new CUIImage(m_pObject, m_pCanvas, &section) });
				break;
			case Type::Grid:
				res = m_elementMap.insert({ hash, new CUIGrid(m_pObject, m_pCanvas, &section) });
				break;
			case Type::Text:
				res = m_elementMap.insert({ hash, new CUIText(m_pObject, m_pCanvas, &section) });
				break;
			case Type::Tooltip:
				res = m_elementMap.insert({ hash, new CUITooltip(m_pObject, m_pCanvas, &section) });
				break;
			case Type::Button:
				res = m_elementMap.insert({ hash, new CUIButton(m_pObject, m_pCanvas, &section) });
				break;
			case Type::GridSelect:
				res = m_elementMap.insert({ hash, new CUIGridSelect(m_pObject, m_pCanvas, &section) });
				break;
		}

		assert(res.second);
		
		section.pUIElement = res.first->second;
		section.pUIElement->Initialize();

		m_pCanvas->RegisterElement(section.pUIElement);
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	//
	// Texture.
	//

	void CUIScript::ParseTexture(const Util::CScriptObject::PropBlock& propBlock)
	{
		Texture texture { };
		texture.name = propBlock.propList[1];

		for(size_t i = 0; i < propBlock.children.size(); ++i)
		{
			auto elem = TEXTURE_MAP.find(propBlock.children[i].propList[0]);
			if(elem != TEXTURE_MAP.end())
			{
				elem->second(&propBlock.children[i], texture);
			}
		}

		if(texture.texture)
		{
			ASSERT(m_textureMap.insert({ texture.texture->propList[1], std::move(texture) }).second);
		}
	}

	//
	// Class.
	//

	void CUIScript::ParseClass(const Util::CScriptObject::PropBlock& propBlock)
	{
		Section section { };

		for(size_t i = 0; i < propBlock.children.size(); ++i)
		{
			auto elem = SECTION_MAP.find(propBlock.children[i].propList[0]);
			if(elem != SECTION_MAP.end())
			{
				elem->second(&propBlock.children[i], section);
			}
		}

		Class cls { };
		cls.name = propBlock.propList[1];
		cls.frameData = section.frameData;
		cls.type = section.type;
		cls.typeProp = section.typeProp;

		ASSERT(m_classMap.insert({ cls.name, std::move(cls) }).second);
	}

	//
	// Frame.
	//

	void CUIScript::ParseFrame(const Util::CScriptObject::PropBlock& propBlock, size_t parentIndex)
	{
		Section& section = *m_sectionList.emplace(m_sectionList.end());
		section.name = propBlock.propList[1];
		section.parentIndex = parentIndex;
		section.pScript = this;

		ProcessFrame(propBlock.children, m_sectionList.size() - 1);
	}

	void CUIScript::ProcessFrame(const std::vector<Util::CScriptObject::PropBlock>& propBlockList, size_t sectionIndex)
	{
		Section& section = m_sectionList[sectionIndex];

		// Look for classes.
		for(size_t i = 0; i < propBlockList.size(); ++i)
		{
			if(propBlockList[i].propList[0] == L"class")
			{
				auto elem = m_classMap.find(propBlockList[i].propList[1]);
				assert(elem != m_classMap.end());
				section.cls = elem->first;
				section.frameData = elem->second.frameData;
			}
		}
		
		// Set remaining properties.
		for(size_t i = 0; i < propBlockList.size(); ++i)
		{
			auto elem = SECTION_MAP.find(propBlockList[i].propList[0]);
			if(elem != SECTION_MAP.end())
			{
				elem->second(&propBlockList[i], section);
			}
		}

		if(section.parentIndex == SIZE_MAX)
		{
			section.frameData.pParent = m_data.pParent;
		}

		for(auto& child : section.children)
		{
			ParseFrame(*child, sectionIndex);
		}
	}
};
