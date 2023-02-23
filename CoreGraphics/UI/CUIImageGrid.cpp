//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIImageGrid.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIImageGrid.h"
#include <Application/CCommandManager.h>
#include <Math/CMathFNV.h>
#include <Utilities/CConvertUtil.h>
#include <unordered_map>

namespace UI
{
	CUIImageGrid::CUIImageGrid(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection) : 
		CUIRenderer(pObject, pCanvas, pSection),
		m_instanceCount(0)
	{
	}

	CUIImageGrid::~CUIImageGrid()
	{
	}

	void CUIImageGrid::Initialize()
	{

		CUIRenderer::Initialize();
		
		m_instanceCount = m_data.cells.x * m_data.cells.y;
		m_indexList.resize(m_instanceCount, 0);
		
		m_cellSize = Math::Vector2(1.0f / m_data.uvCells.x, 1.0f / m_data.uvCells.y);
	}

	void CUIImageGrid::DrawSetup()
	{
		CUIRenderer::DrawSetup();
		
		if(!IsActive())
		{
			return;
		}

		const Math::Vector2 size = GetFrame()->GetBounds().size;
		const Math::Matrix3x2 world = Math::Matrix3x2::Scale(Math::Vector2(1.0f / size.x, 1.0f / size.y)) * GetFrame()->GetWorldMatrix();
		
		const float stepX = std::max(0.0f, size.x) / m_data.cells.x;
		const float stepY = std::max(0.0f, size.y) / m_data.cells.y;
		float offsetX = 0.0f;
		float offsetY = 0.0f;

		u32 index = 0;
		for(int j = 0; j < m_data.cells.y; ++j)
		{
			offsetX = 0.0f;
			for(int i = 0; i < m_data.cells.x; ++i)
			{
				CUIQuad::Instance& instance = *PullInstance(index);
				instance.color = m_data.color;

				const u32 x = m_indexList[index] % m_data.uvCells.x;
				const u32 y = m_indexList[index] / m_data.uvCells.x;
				instance.texCoord = Math::Vector4(m_cellSize.x * x, m_cellSize.y * y, m_cellSize.x, m_cellSize.y);

				instance.world = Math::Matrix3x2::Scale(Math::Vector2(stepX, stepY)) * Math::Matrix3x2::Translate(Math::Vector2(offsetX, offsetY)) * world;

				offsetX += stepX;
				++index;
			}

			offsetY += stepY;
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// CUICommand methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUIImageGrid::CommandElement(const CUICommandList::StateElem& stateElem)
	{
		if(CUIRenderer::CommandElement(stateElem))
		{
			return true;
		}

		struct MapData
		{
			const CUICommandList::StateElem& stateElem;
			CUIImageGrid* pGrid;
			Data& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> GRID_MAP {
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
	
	bool CUIImageGrid::SetFullDataFromKeyValue(CUIImageGrid* pImageGrid, const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::wstring& key, const std::wstring& value, Data& data)
	{
		if(CUIElement::SetDataFromKeyValue(pScript, prop, key, value, data)) return true;
		if(CUIRenderer::SetDataFromKeyValue(pScript, key, value, data)) return true;
		if(CUIImageGrid::SetDataFromKeyValue(pImageGrid, pScript, prop, key, value, data)) return true;
		return false;
	}

	bool CUIImageGrid::SetDataFromKeyValue(CUIImageGrid* pImageGrid, const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::wstring& key, const std::wstring& value, Data& data)
	{
		struct MapData
		{
			CUIImageGrid* pImageGrid;
			const CUIScript* pScript;
			const Util::CScriptObject::PropBlock& prop;
			const std::wstring& value;
			Data& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> IMAGE_MAP {
			{ L"uvCells", [](const MapData& mapData) { mapData.data.uvCells = Util::StringToVectorInt<Math::VectorInt2>(mapData.value); } },
			{ L"cells", [](const MapData& mapData) { mapData.data.cells = Util::StringToVectorInt<Math::VectorInt2>(mapData.value); } },
			{ L"index", [](const MapData& mapData) {
				const Math::VectorInt2 vec = Util::StringToVectorInt<Math::VectorInt2>(mapData.value);
				if(mapData.pImageGrid->m_indexList.size() <= static_cast<size_t>(vec.x)) mapData.pImageGrid->m_indexList.resize(static_cast<size_t>(vec.x + 1));
				mapData.pImageGrid->m_indexList[vec.x] = vec.y;
			}},

			{ L"material", [](const MapData& mapData) { mapData.data.matHash = Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()); } },
			{ L"texture", [](const MapData& mapData) { mapData.data.texHash = Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()); } },
			
			{ L"actionCellIndexChange", [](const MapData& mapData) { App::CCommandManager::Instance().RegisterAction(Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()), 
				std::bind(&CUIImageGrid::OnCellIndexChange, mapData.pImageGrid, std::placeholders::_1)); } },
		};

		const auto& elem = IMAGE_MAP.find(key);
		if(elem == IMAGE_MAP.end()) return false;

		elem->second({ pImageGrid, pScript, prop, value, data });
		return true;
	}

	void CUIImageGrid::BuildFromSection(const CUIScript::Section* pSection)
	{
		CUIRenderer::BuildFromSection(pSection);
		
		if(pSection->cls.size())
		{
			CUIScript::ProcessBlock(*pSection->pScript->GetClass(pSection->cls).typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
				SetDataFromKeyValue(this, pSection->pScript, prop, str[0], prop.propList[1], m_data);
			});
		}

		CUIScript::ProcessBlock(*pSection->typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
			SetDataFromKeyValue(this, pSection->pScript, prop, str[0], prop.propList[1], m_data);
		});
	}
	
	//-----------------------------------------------------------------------------------------------
	// Command methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUIImageGrid::OnCellIndexChange(const void* params)
	{
		std::pair<u32, u32> indexVal = *reinterpret_cast<const std::pair<u32, u32>*>(params);
		SetCellIndex(indexVal.first, indexVal.second);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUIImageGrid::SetCellIndex(u32 cell, u32 index)
	{
		m_indexList[cell] = index;
		MarkAsDirty();
	}
};
