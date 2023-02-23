//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIGrid.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIGrid.h"
#include <Math/CMathFNV.h>
#include <Utilities/CConvertUtil.h>
#include <unordered_map>

namespace UI
{
	CUIGrid::CUIGrid(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection) : 
		CUIRenderer(pObject, pCanvas, pSection),
		m_instanceCount(0)
	{
	}

	CUIGrid::~CUIGrid()
	{
	}

	void CUIGrid::Initialize()
	{
		m_instanceCount = (m_data.cells.x + 1) + (m_data.cells.y + 1);
		CUIRenderer::Initialize();
	}

	void CUIGrid::DrawSetup()
	{
		CUIRenderer::DrawSetup();
		
		if(!IsActive())
		{
			return;
		}

		const Math::Vector2 size = GetFrame()->GetBounds().size;
		const Math::Matrix3x2 world = Math::Matrix3x2::Scale(Math::Vector2(1.0f / size.x, 1.0f / size.y)) * GetFrame()->GetWorldMatrix();

		const float stepX = std::max(0.0f, size.x - m_data.thickness) / m_data.cells.x;
		const float stepY = std::max(0.0f, size.y - m_data.thickness) / m_data.cells.y;
		float offsetX = m_data.thickness * 0.0f;
		float offsetY = m_data.thickness * 0.0f;

		u32 index = 0;

		// Vertical lines.
		for(int i = 0; i <= m_data.cells.x; ++i)
		{
			CUIQuad::Instance& instance = *PullInstance(index++);
			instance.color = m_data.color;
			instance.texCoord = Math::Vector4(0.0f, 0.0f, 1.0f, 1.0f);

			instance.world = Math::Matrix3x2::Scale(Math::Vector2(m_data.thickness, size.y)) * Math::Matrix3x2::Translate(Math::Vector2(offsetX, 0.0f)) * world;
			offsetX += stepX;
		}

		// Horizontal lines.
		for(int i = 0; i <= m_data.cells.y; ++i)
		{
			CUIQuad::Instance& instance = *PullInstance(index++);
			instance.color = m_data.color;
			instance.texCoord = Math::Vector4(0.0f, 0.0f, 1.0f, 1.0f);

			instance.world = Math::Matrix3x2::Scale(Math::Vector2(size.x, m_data.thickness)) * Math::Matrix3x2::Translate(Math::Vector2(0.0f, offsetY)) * world;
			offsetY += stepY;
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// CUICommand methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUIGrid::CommandElement(const CUICommandList::StateElem& stateElem)
	{
		if(CUIRenderer::CommandElement(stateElem))
		{
			return true;
		}

		struct MapData
		{
			const CUICommandList::StateElem& stateElem;
			CUIGrid* pGrid;
			Data& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> GRID_MAP {
			{ L"thickness", [](const MapData& mapData) { mapData.data.thickness = mapData.stateElem.val.f; } },
		};
		
		const auto& elem = GRID_MAP.find(stateElem.GetSubElemAt(0));
		if(elem == GRID_MAP.end()) return false;

		elem->second({ stateElem, this, m_data });
		MarkAsDirty();

		return true;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Section methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUIGrid::SetFullDataFromKeyValue(const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::wstring& key, const std::wstring& value, Data& data)
	{
		if(CUIElement::SetDataFromKeyValue(pScript, prop, key, value, data)) return true;
		if(CUIRenderer::SetDataFromKeyValue(pScript, key, value, data)) return true;
		if(CUIGrid::SetDataFromKeyValue(pScript, key, value, data)) return true;
		return false;
	}

	bool CUIGrid::SetDataFromKeyValue(const CUIScript* pScript, const std::wstring& key, const std::wstring& value, Data& data)
	{
		struct MapData
		{
			const CUIScript* pScript;
			const std::wstring& prop;
			Data& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> IMAGE_MAP {
			{ L"thickness", [](const MapData& mapData) { mapData.data.thickness = Util::StringToFloat(mapData.prop); } },
			{ L"cells", [](const MapData& mapData) { mapData.data.cells = Util::StringToVectorInt<Math::VectorInt2>(mapData.prop); } },

			{ L"material", [](const MapData& mapData) { mapData.data.matHash = Math::FNV1a_64(mapData.prop.c_str(), mapData.prop.size()); } },
			{ L"texture", [](const MapData& mapData) { mapData.data.texHash = Math::FNV1a_64(mapData.prop.c_str(), mapData.prop.size()); } },
		};

		const auto& elem = IMAGE_MAP.find(key);
		if(elem == IMAGE_MAP.end()) return false;

		elem->second({ pScript, value, data });
		return true;
	}

	void CUIGrid::BuildFromSection(const CUIScript::Section* pSection)
	{
		CUIRenderer::BuildFromSection(pSection);
		
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
};
