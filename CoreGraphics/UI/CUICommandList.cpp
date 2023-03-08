//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUICommandList.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUICommandList.h"
#include "CUIScript.h"
#include "CUIElement.h"
#include <Application/CCommandManager.h>
#include <Math/CMathFNV.h>
#include <Utilities/CMemoryFree.h>
#include <unordered_map>
#include <sstream>
#include <string>

namespace UI
{
	static const std::unordered_map<u64, std::function<void(u64)>> ELEM_MAP = {
		{ Math::FNV1a_64(L"cmd"), [](u64 hash){ App::CCommandManager::Instance().Execute(hash); } }
	};


	//-----------------------------------------------------------------------------------------------
	// CUICommandList::State
	//-----------------------------------------------------------------------------------------------

	void CUICommandList::State::Activate(const CUIScript* pScript) const
	{
		for(auto& elem : elemList)
		{
			const auto& em = ELEM_MAP.find(elem.elemHash);
			if(em != ELEM_MAP.end())
			{
				if(App::CCommandManager::Instance().IsReady())
				{
					em->second(elem.val.h);
				}
			}
			else
			{
				pScript->GetElement(elem.elemHash)->CommandElement(elem(0));
			}
		}
	}

	//-----------------------------------------------------------------------------------------------
	// CUICommandList
	//-----------------------------------------------------------------------------------------------

	CUICommandList::CUICommandList()
	{
	}

	CUICommandList::~CUICommandList()
	{
	}

	void CUICommandList::PostInitialize()
	{
		for(auto& toggle : m_commandToggleList)
		{
			if(Util::StringToBool(toggle->state[0].name))
			{
				toggle->bState = !toggle->bState;
			}
			
			toggle->state[toggle->bState].Activate(toggle->pScript);
		}
	}

	void CUICommandList::Release()
	{
		for(auto& action : m_commandActionList)
		{
			SAFE_DELETE(action);
		}

		for(auto& toggle : m_commandToggleList)
		{
			SAFE_DELETE(toggle);
		}
	}

	void CUICommandList::RegisterCommand(const CUIScript* pScript, const Util::CScriptObject::PropBlock& propBlock)
	{
		m_command.cmdHash = Math::FNV1a_64(propBlock.propList[1].c_str(), propBlock.propList[1].size());
		m_command.pScript = pScript;

		struct MapData
		{
			CUICommandList* pCommandList;
			const Util::CScriptObject::PropBlock& prop;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> ELEM_MAP {
			{ L"type", [](const MapData& mapData) { mapData.pCommandList->ProcessType(mapData.prop); } },
		};
		
		CUIScript::ProcessBlock(propBlock, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
			const auto& elem = ELEM_MAP.find(str[0]);
			if(elem != ELEM_MAP.end())
			{
				elem->second({ this, prop });
			}
		});

		// Finalize command.
		switch(m_command.type)
		{
			case CommandType::Action: {
				m_commandActionList.push_back(new CommandAction(m_command.pScript, m_command.stateList[0]));
				App::CCommandManager::Instance().RegisterCommand(m_command.cmdHash, std::bind(&CommandAction::Execute, m_commandActionList.back()));
			} break;
			case CommandType::Toggle: {
				m_commandToggleList.push_back(new CommandToggle(m_command.pScript, m_command.defaultState, m_command.stateList[0], m_command.stateList[1]));
				App::CCommandManager::Instance().RegisterCommand(m_command.cmdHash, std::bind(&CommandToggle::Execute, m_commandToggleList.back()));
			} break;
			default:
				break;
		}

		m_command.Reset();
	}

	//-----------------------------------------------------------------------------------------------
	// Processing methods.
	//-----------------------------------------------------------------------------------------------

	void CUICommandList::ProcessType(const Util::CScriptObject::PropBlock& propBlock)
	{
		// Process type.
		static const std::unordered_map<std::wstring, CommandType> TYPE_MAP {
			{ L"action", CommandType::Action },
			{ L"toggle", CommandType::Toggle },
		};

		m_command.type = TYPE_MAP.find(propBlock.propList[1])->second;

		// Process type contents.
		struct MapData
		{
			CUICommandList* pCommandList;
			const Util::CScriptObject::PropBlock& prop;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> ELEM_MAP {
			{ L"default", [](const MapData& mapData) { mapData.pCommandList->m_command.defaultState = mapData.prop.propList[1]; } },
			{ L"state", [](const MapData& mapData) { mapData.pCommandList->ProcessState(mapData.prop); } },
		};
		
		CUIScript::ProcessBlock(propBlock, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
			const auto& elem = ELEM_MAP.find(str[0]);
			if(elem != ELEM_MAP.end())
			{
				elem->second({ this, prop });
			}
		});
	}

	void CUICommandList::ProcessState(const Util::CScriptObject::PropBlock& propBlock)
	{
		// Process state.
		State& state = m_command.stateList.emplace_back();
		state.name = propBlock.propList[1];

		// Process state contents.
		for(auto& elem : propBlock.children)
		{
			StateElem& stateElem = state.elemList.emplace_back();

			Util::SplitString(elem.propList[0], L'.', [&stateElem](u32 index, const std::wstring& str){
				if(index == 0)
				{
					stateElem.elemHash = Math::FNV1a_64(str.c_str(), str.size());
				}
				else
				{
					stateElem.AddSubElem(str);
				}
			});
			
			stateElem.valStr = elem.propList[1];

			if(ELEM_MAP.find(stateElem.elemHash) != ELEM_MAP.end())
			{
				stateElem.val.h = Math::FNV1a_64(elem.propList[1].c_str(), elem.propList[1].size());
			}
			else
			{
				if(Util::IsLong(elem.propList[1], &stateElem.val.l)) { }
				else if(Util::IsFloat(elem.propList[1], &stateElem.val.f)) { }
				else if(Util::IsBool(elem.propList[1], &stateElem.val.b)) { }
			}
		}
	}
};
