//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CWUIScript.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CWUISCRIPT_H
#define CWUISCRIPT_H

#include "CPanelData.h"
#include <Utilities/CScriptObject.h>
#include <string>
#include <unordered_map>

namespace App
{
	class CWUIScript
	{
	public:
		struct Data
		{
			std::wstring filename;
			PanelAdjustFunc platformOnMove;
			PanelAdjustFunc platformOnResize;
		};

	public:
		CWUIScript();
		~CWUIScript();
		CWUIScript(const CWUIScript&) = delete;
		CWUIScript(CWUIScript&&) = delete;
		CWUIScript& operator = (const CWUIScript&) = delete;
		CWUIScript& operator = (CWUIScript&&) = delete;

		void Parse();
		void Release();

		void SaveProductionResources();

		// Accessors.
		inline class CPanel* GetMainPanel() const { return m_pMainPanel; }
		inline class CPanel* GetPanel(const std::wstring& key) const { return m_panelMap.find(key)->second; }
		inline const Util::CScriptObject& GetScriptObject() const { return m_scriptObject; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void OnMove(const class CPanel* pPanel);
		void OnResize(const class CPanel* pPanel);

	private:
		Data m_data;

		Util::CScriptObject m_scriptObject;

		std::unordered_map<std::wstring, class CPanel*> m_panelMap;
		class CPanel* m_pMainPanel;
	};
};

#endif
