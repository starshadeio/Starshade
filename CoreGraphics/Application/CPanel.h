//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CPanel.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPANEL_H
#define CPANEL_H

#include "CPanelData.h"
#include <Globals/CGlobals.h>
#include <Math/CMathVectorInt2.h>
#include <Math/CMathRect.h>
#include <string>

namespace App
{
	class CPanel
	{
	public:
		struct Data
		{
			bool bMain;

			bool bAllowDoubleClick;
			bool bAllowResize;
			bool bUseVersionNumber = true;

			u8 targetMonitor;
			PANEL_SCREEN_MODE targetScreenMode;
			Math::VectorInt2 targetDims;
			Math::VectorInt2 minimumDims = { 0, 0 };

			std::wstring icon;
			std::wstring title;
			const CPanel* pParent;

			PanelAdjustFunc funcOnMove;
			PanelAdjustFunc funcOnResize;
		};

	protected:
		CPanel();
		CPanel(const CPanel&) = delete;
		CPanel(CPanel&&) = delete;
		CPanel& operator = (const CPanel&) = delete;
		CPanel& operator = (CPanel&&) = delete;

	public:
		virtual ~CPanel();

		virtual void Initialize() = 0;
		virtual void Release() = 0;

		// Accessors.
		inline bool IsHovered() const { return m_bIsHovered; }
		inline bool InFocus() const { return m_bInFocus; }

		inline PANEL_SCREEN_MODE GetScreenMode() const { return m_screenMode; }
		inline PANEL_SCREEN_STATE GetScreenState() const { return m_screenState; }
		inline const Math::Rect& GetRect() const { return m_panelRect; }
		
		inline const Data& GetData() const { return m_data; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		
		virtual inline void SetTitle(const wchar_t* pTitle) const = 0;

	protected:
		void OnMove();
		void OnResize();

		bool OnHover(bool bHovered);
		bool OnFocus(bool bFocused);

	private:
		bool m_bIsHovered;
		bool m_bInFocus;

		PANEL_SCREEN_MODE m_screenMode;

	protected:
		PANEL_SCREEN_STATE m_screenState;
		Math::Rect m_panelRect;

		Data m_data;
	};
};

#endif
