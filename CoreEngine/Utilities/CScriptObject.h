//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CScriptObject.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSCRIPTOBJECT_H
#define CSCRIPTOBJECT_H

#include "CDetectComment.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace Util
{
	class CScriptObject
	{
	public:
		typedef std::function<bool(CScriptObject*)> StateFunc;
		typedef std::function<bool(CScriptObject*, const std::wstring&)> StateNullFunc;
		typedef std::unordered_map<std::wstring, StateFunc> StateMap;

		// State class for scriptable object.
		class State
		{
		public:
			State(const StateMap* pMap, const StateNullFunc null) :
				m_pMap(pMap),
				m_null(null) {
			}

			bool ProccessWord(CScriptObject* pObj, const std::wstring& word) const
			{
				if(m_pMap != nullptr)
				{
					auto elem = m_pMap->find(word);
					if(elem != m_pMap->end())
					{
						return elem->second(pObj);
					}
				}

				if(m_null != nullptr)
				{
					return m_null(pObj, word);
				}

				return false;
			}

		private:
			const StateMap* m_pMap;
			const StateNullFunc m_null;
		};

		// Data containers.
		enum class BlockType
		{
			None,
			PropBlock,
		};

		struct PropBlock
		{
		public:
			friend class CScriptObject;

		private:
			bool bAwaitingNext = true;

		public:
			std::vector<std::wstring> propList;
			std::vector<PropBlock> children;
			PropBlock* pParent = nullptr;
		};

	public:
		struct Data
		{
			std::wstring filename;
		};

	public:
		CScriptObject();
		~CScriptObject();
		CScriptObject(const CScriptObject&) = delete;
		CScriptObject(CScriptObject&&) = delete;
		CScriptObject& operator = (const CScriptObject&) = delete;
		CScriptObject& operator = (CScriptObject&&) = delete;

		void Build();
		static std::wstring FormatScript(const std::wstring& script);

		// Accessors.
		inline const std::vector<PropBlock>& GetPropBlockList() const { return m_propBlockList; }
		inline const std::wstring& GetCode() const { return m_code; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		inline bool IsAwaitingValue() const { return m_pPropBlock->bAwaitingNext; }
		inline void SetAwaitingValue(bool bAwaitingNext) { m_pPropBlock->bAwaitingNext = bAwaitingNext; }

	private:
		friend bool CreatePropBlock(CScriptObject* pObj);
		friend bool SetVal(CScriptObject* pObj, const std::wstring& word);
		friend bool NextVal(CScriptObject* pObj);
		friend bool NextProp(CScriptObject* pObj);
		friend bool NextSubProp(CScriptObject* pObj);
		friend bool ClosePropBlock(CScriptObject* pObj);

	private:
		Data m_data;
		std::wstring m_code;

		CompilerWord<std::wstring, wchar_t> m_word;

		BlockType m_blockType;

		std::vector<PropBlock> m_propBlockList;
		PropBlock* m_pPropBlock;

		const State* m_pState;
	};
};

#endif
