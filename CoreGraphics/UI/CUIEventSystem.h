//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIEventSystem.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUIEVENTSYSTEM_H
#define CUIEVENTSYSTEM_H

#include "CUICommandList.h"
#include "../Application/CInputData.h"
#include <vector>

namespace UI
{
	class CUIEventSystem
	{
	public:
		CUIEventSystem();
		~CUIEventSystem();
		CUIEventSystem(const CUIEventSystem&) = delete;
		CUIEventSystem(CUIEventSystem&&) = delete;
		CUIEventSystem& operator = (const CUIEventSystem&) = delete;
		CUIEventSystem& operator = (CUIEventSystem&&) = delete;
		
		void PostInitialize();
		void Update();
		void Release();

		bool ProcessInput(u32 code, const App::InputCallbackData& callback);

		void RegisterTooltip(class CUITooltip* pTooltip);
		void DeregisterTooltip(class CUITooltip* pTooltip);

		void RegisterCanvas(class CUICanvas* pCanvas);
		void DeregisterCanvas(class CUICanvas* pCanvas);

		// Accessors.
		inline const class CUISelect* GetHovered() const { return m_pHoveredElement; }

		CUICommandList& CommandList() { return m_commandList; }

	private:
		CUICommandList m_commandList;

		std::vector<class CUICanvas*> m_canvasList;
		std::vector<class CUITooltip*> m_tooltipList;
		class CUISelect* m_pHoveredElement;
	};
};

#endif
