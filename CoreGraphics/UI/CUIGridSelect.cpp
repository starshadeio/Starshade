//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIGridSelect.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIGridSelect.h"
#include "CUICanvas.h"
#include <Application/CCommandManager.h>
#include <Utilities/CConvertUtil.h>
#include <unordered_map>

namespace UI
{
	CUIGridSelect::CUIGridSelect(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection) : 
		CUISelect(pObject, pCanvas, pSection),
		m_index(0),
		m_background(pObject, pCanvas),
		m_selector(pObject, pCanvas),
		m_grid(pObject, pCanvas)
	{
	}

	CUIGridSelect::~CUIGridSelect()
	{
	}
	
	void CUIGridSelect::OnRegister(class CUICanvas* pCanvas)
	{
		CUISelect::OnRegister(pCanvas);
		
		m_cellSize = Math::Vector2(1.0f / m_data.cells.x, 1.0f / m_data.cells.y);
		
		CUIFrame::Data frameData { };
		frameData.anchorMin = 0.0f;
		frameData.anchorMax = 1.0f;
		frameData.pivot = 0.5f;
		frameData.pParent = GetFrame();
		
		{ // Setup the background.
			m_data.bgData.frameData = frameData;
			m_data.bgData.cells = m_data.bSeparateCells ? m_data.cells : Math::VectorInt2(1, 1);
			m_background.SetData(m_data.bgData);
			m_background.Initialize();

			pCanvas->RegisterElement(&m_background);
		}

		if(m_data.bUseSelector)
		{ // Setup the selector.
			m_data.selectorData.frameData = frameData;
			m_data.selectorData.frameData.anchorMin = Math::Vector2(0.0f, 1.0f);
			m_data.selectorData.frameData.anchorMax = Math::Vector2(0.0f, 1.0f);
			m_data.selectorData.frameData.pivot = Math::Vector2(0.0f, 1.0f);
			m_data.selectorData.depth = m_data.bgData.depth + 2;
			m_selector.SetData(m_data.selectorData);
			
			CalculateSelectorPosition(m_index);
			
			m_selector.Initialize();
			pCanvas->RegisterElement(&m_selector);
		}
		
		{ // Setup the grid.
			m_data.gridData.frameData = frameData;
			m_data.gridData.depth = m_data.bgData.depth + 1;
			m_data.gridData.cells = m_data.cells;
			m_grid.SetData(m_data.gridData);
			m_grid.Initialize();

			pCanvas->RegisterElement(&m_grid);
		}
	}

	void CUIGridSelect::OnUpdate()
	{
		CUISelect::OnUpdate();

		if(m_data.bUseSelector)
		{
			CalculateSelectorPosition(m_index);
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// CUICommand methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUIGridSelect::CommandElement(const CUICommandList::StateElem& stateElem)
	{
		if(CUISelect::CommandElement(stateElem))
		{
			return true;
		}

		struct MapData
		{
			const CUICommandList::StateElem& stateElem;
			CUIGridSelect* pGrid;
			Data& data;
		};

		typedef std::function<void(const MapData&)> GRID_FUNC;
		static const std::unordered_map<std::wstring, GRID_FUNC> GRID_MAP {
			{ L"bg", [](const MapData& mapData) { mapData.pGrid->m_background.CommandElement(mapData.stateElem(1)); } },
			{ L"selector", [](const MapData& mapData) { mapData.pGrid->m_selector.CommandElement(mapData.stateElem(1)); } },
			{ L"grid", [](const MapData& mapData) { mapData.pGrid->m_grid.CommandElement(mapData.stateElem(1)); } },
			
			{ L"onEnter", [](const MapData& mapData) { mapData.data.onEnterHash = Math::FNV1a_64(mapData.stateElem.valStr.c_str(), mapData.stateElem.valStr.size()); } },
			{ L"onExit", [](const MapData& mapData) { mapData.data.onExitHash = Math::FNV1a_64(mapData.stateElem.valStr.c_str(), mapData.stateElem.valStr.size()); } },
			{ L"onPress", [](const MapData& mapData) { mapData.data.onPressHash = Math::FNV1a_64(mapData.stateElem.valStr.c_str(), mapData.stateElem.valStr.size()); } },
			{ L"onRelease", [](const MapData& mapData) { mapData.data.onReleaseHash = Math::FNV1a_64(mapData.stateElem.valStr.c_str(), mapData.stateElem.valStr.size()); } },
		};
		
		const auto& elem = GRID_MAP.find(stateElem.GetSubElemAt(0));
		if(elem == GRID_MAP.end()) return false;

		elem->second({ stateElem, this, m_data });

		return true;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Section methods.
	//-----------------------------------------------------------------------------------------------

	bool CUIGridSelect::SetDataFromKeyValue(CUIGridSelect* pGridSelect, const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::vector<std::wstring>& keys, const std::wstring& value, Data& data)
	{
		struct MapData
		{
			CUIGridSelect* pGrid;
			const CUIScript* pScript;
			const Util::CScriptObject::PropBlock& prop;
			const std::vector<std::wstring>& keys;
			const std::wstring& value;
			Data& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> GRID_MAP {
			{ L"bSeparateCells", [](const MapData& mapData) { mapData.data.bSeparateCells = Util::StringToBool(mapData.value); } },
			{ L"bUseSelector", [](const MapData& mapData) { mapData.data.bUseSelector = Util::StringToBool(mapData.value); } },
			{ L"cells", [](const MapData& mapData) { mapData.data.cells = Util::StringToVectorInt<Math::VectorInt2>(mapData.value); } },

			{ L"bg", [](const MapData& mapData) { CUIImageGrid::SetFullDataFromKeyValue(&mapData.pGrid->m_background, mapData.pScript, mapData.prop, mapData.keys[1], mapData.value, mapData.data.bgData); } },
			{ L"selector", [](const MapData& mapData) { CUIGrid::SetFullDataFromKeyValue(mapData.pScript, mapData.prop, mapData.keys[1], mapData.value, mapData.data.selectorData); } },
			{ L"grid", [](const MapData& mapData) { CUIGrid::SetFullDataFromKeyValue(mapData.pScript, mapData.prop, mapData.keys[1], mapData.value, mapData.data.gridData); } },
			
			{ L"onEnter", [](const MapData& mapData) { mapData.data.onEnterHash = Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()); } },
			{ L"onExit", [](const MapData& mapData) { mapData.data.onExitHash = Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()); } },
			{ L"onPress", [](const MapData& mapData) { mapData.data.onPressHash = Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()); } },
			{ L"onRelease", [](const MapData& mapData) { mapData.data.onReleaseHash = Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()); } },
			
			{ L"actionIndexChange", [](const MapData& mapData) { App::CCommandManager::Instance().RegisterAction(Math::FNV1a_64(mapData.value.c_str(), mapData.value.size()), 
				std::bind(&CUIGridSelect::OnIndexChange, mapData.pGrid, std::placeholders::_1)); } },
		};

		const auto& elem = GRID_MAP.find(keys[0]);
		if(elem == GRID_MAP.end()) return false;

		elem->second({ pGridSelect, pScript, prop, keys, value, data });
		return true;
	}

	void CUIGridSelect::BuildFromSection(const CUIScript::Section* pSection)
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
	}
	
	//-----------------------------------------------------------------------------------------------
	// State methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUIGridSelect::OnEnter()
	{
		if(m_data.onEnterHash)
		{
			App::CCommandManager::Instance().Execute(m_data.onEnterHash);
		}
	}

	void CUIGridSelect::OnExit()
	{
		if(m_data.onExitHash)
		{
			App::CCommandManager::Instance().Execute(m_data.onExitHash);
		}
	}

	bool CUIGridSelect::OnSelect(const App::InputCallbackData& callback)
	{
		u32 index = CalculateCursorIndex();
		if(callback.bPressed)
		{
			if(m_data.onPressHash)
			{
				App::CCommandManager::Instance().Execute(m_data.onPressHash, &index);
			}
		}
		else
		{
			if(m_data.onReleaseHash)
			{
				App::CCommandManager::Instance().Execute(m_data.onReleaseHash, &index);
			}
		}

		return true;
	}

	//-----------------------------------------------------------------------------------------------
	// Command methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUIGridSelect::OnIndexChange(const void* params)
	{
		m_index = *reinterpret_cast<const u32*>(params);

		if(m_data.bUseSelector)
		{
			CalculateSelectorPosition(m_index);
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	u32 CUIGridSelect::CalculateCursorIndex()
	{
		const Math::Vector2 size = GetFrame()->GetBounds().size;
		const Math::Vector2 pt = GetRelativeCursorPoint();

		const int i = static_cast<int>(floorf((pt.x / size.x) * m_data.cells.x));
		const int j = static_cast<int>(floorf((1.0f - pt.y / size.y) * m_data.cells.y));

		assert(i >= 0 && j >= 0);

		const int index = j * m_data.cells.x + i;

		assert(index < m_data.cells.x * m_data.cells.y);

		return static_cast<u32>(index);
	}

	void CUIGridSelect::CalculateSelectorPosition(u32 index)
	{
		const Math::Vector2 size = GetFrame()->GetBounds().size - m_data.gridData.thickness;

		const u32 i = index % m_data.cells.x;
		const u32 j = index / m_data.cells.x;
		
		m_selector.GetFrame()->SetPosition(Math::Vector2::PointwiseProduct(Math::Vector2(m_cellSize.x * i, -m_cellSize.y * j), size) + Math::Vector2(0.5f, -0.5f) * m_data.gridData.thickness);
		m_selector.GetFrame()->SetSize(Math::Vector2::PointwiseProduct(m_cellSize, size));
	}
};
