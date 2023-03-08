//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUICommandList.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUICOMMANDLIST_H
#define CUICOMMANDLIST_H

#include <Globals/CGlobals.h>
#include <Utilities/CScriptObject.h>
#include <Utilities/CConvertUtil.h>

namespace UI
{
	class CUICommandList
	{
	public:
		enum class CommandType
		{
			Unknown,
			Action,
			Toggle,
		};

		struct StateElem
		{
		private:
			std::vector<std::wstring> subElems;
			mutable size_t subElemOffset = 0;

		public:
			const StateElem& operator () (size_t subElemOffset) const {
				this->subElemOffset = subElemOffset;
				return *this;
			}

			const std::wstring& GetSubElemAt(size_t index) const
			{
				return subElems[subElemOffset + index];
			}

			size_t GetSubElemSize() const
			{
				return subElems.size() - subElemOffset;
			}

			void AddSubElem(const std::wstring& str)
			{
				subElems.push_back(str);
			}

			u64 elemHash;

			std::wstring valStr;
			union VAL {
				bool b;
				float f;
				long l;
				u64 h;
			} val;
		};

		struct State
		{
			std::wstring name;
			std::vector<StateElem> elemList;

			void Activate(const class CUIScript* pScript) const;
		};

		struct Command
		{
			u64 cmdHash;
			CommandType type;

			std::wstring defaultState;
			std::vector<State> stateList;

			const class CUIScript* pScript;

			Command()
			{
				Reset();
			}

			void Reset()
			{
				cmdHash = 0;
				type = CommandType::Unknown;
				defaultState.clear();
				stateList.clear();
				pScript = nullptr;
			}
		};

		// Commands.
		struct CommandBase
		{
			const class CUIScript* pScript;
			CommandBase(const class CUIScript* pScript) :
				pScript(pScript) {
			}

			virtual void Reset() = 0;
			virtual bool Execute() = 0;
		};

		struct CommandAction : CommandBase
		{
			State state;

			CommandAction(const class CUIScript* pScript, const State& state) :
				CommandBase(pScript),
				state(state) {
			}

			void Reset() final { }
			bool Execute() final
			{
				state.Activate(pScript);
				return true;
			}
		};
		
		struct CommandToggle : CommandBase
		{
		private:
			bool bInitialState;

		public:
			bool bState;
			State state[2];

			CommandToggle(const class CUIScript* pScript, const std::wstring& defaultState, const State& state0, const State& state1) :
				CommandBase(pScript),
				state{ state0, state1 }
			{
				bInitialState = bState = Util::StringToBool(defaultState);
			}
			
			void Reset() final
			{
				bState = bInitialState;
				state[bState].Activate(pScript);
			}
			
			bool Execute() final
			{
				bState = !bState;
				state[bState].Activate(pScript);
				return true;
			}
		};

	public:
		CUICommandList();
		~CUICommandList();
		CUICommandList(const CUICommandList&) = delete;
		CUICommandList(CUICommandList&&) = delete;
		CUICommandList& operator = (const CUICommandList&) = delete;
		CUICommandList& operator = (CUICommandList&&) = delete;

		void PostInitialize();
		void Release();
		void RegisterCommand(const class CUIScript* pScript, const Util::CScriptObject::PropBlock& propBlock);

	private:
		void ProcessType(const Util::CScriptObject::PropBlock& propBlock);
		void ProcessState(const Util::CScriptObject::PropBlock& propBlock);

	private:
		Command m_command;

		std::vector<CommandAction*> m_commandActionList;
		std::vector<CommandToggle*> m_commandToggleList;
	};
};

#endif
