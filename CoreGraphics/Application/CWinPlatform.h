//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CWinPlatform.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CWINPLATFORM_H
#define CWINPLATFORM_H

#include "CPlatform.h"
#include <Math/CMathVectorInt2.h>
#include <Windows.h>
#include <functional>

#pragma comment (lib, "User32.lib")

namespace App
{
	class CWinPlatform : public CPlatform
	{
	public:
		struct WinData : Data
		{
			HINSTANCE hInstance;
			int nCmdShow;
		};

	public:
		CWinPlatform();
		~CWinPlatform();
		CWinPlatform(const CWinPlatform&) = delete;
		CWinPlatform(CWinPlatform&&) = delete;
		CWinPlatform& operator = (const CWinPlatform&) = delete;
		CWinPlatform& operator = (CWinPlatform&&) = delete;

		int Run(class CAppBase* pApp, void* pData) final;

		// Callbacks.
		inline void AddMessageCallback(std::function<void(const MSG&)> callback)
		{
			m_msgCallbackList.push_back(callback);
		}

		// Accessors.
		inline const WinData& GetWinData() const { return m_winData; }

	private:
		void UnlockCursor() final;
		void CenterCursor(const class CPanel* pPanel) final;
		void LockCursor(const class CPanel* pPanel) final;
		void ConfineCursor(const class CPanel* pPanel) final;
		void SetCursorVisibility(bool bShow) final;

		MessageBoxState PostMessageBox(const wchar_t* msg, const wchar_t* caption, MessageBoxType type) const final;

		void Quit(int exitCode = 0) final;

	private:
		void PopulateMonitorList();
		void MessageHandler(const MSG& msg);

		static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM lParam);
		BOOL CALLBACK MonitorProc(HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM lParam);

	private:
		std::vector<std::function<void(const MSG&)>> m_msgCallbackList;

		bool m_bCursorPanelInFocus;
		CURSOR_STATE m_cursorState;
		Math::VectorInt2 m_cursorPosition;
		const class CPanel* m_pCursorPanel;

		WinData m_winData;
	};
};

#endif
