//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CWinPlatform.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef PLATFORM_WINDOWS

#include "CWinPlatform.h"
#include "CPanel.h"
#include "CInput.h"
#include "../Factory/CFactory.h"
#include <Application/CAppBase.h>
#include <unordered_map>

namespace App
{
	CWinPlatform::CWinPlatform() :
		m_bCursorPanelInFocus(false),
		m_cursorPosition(0),
		m_cursorState(CURSOR_STATE_DEFAULT),
		m_pCursorPanel(nullptr) {
	}

	CWinPlatform::~CWinPlatform() { }

	int CWinPlatform::Run(CAppBase* pApp, void* pData)
	{
		CPlatform::Run(pApp, pData);

		m_winData = *reinterpret_cast<WinData*>(pData);
		PopulateMonitorList();

		Initialize(m_winData);

		// Main loop.
		MSG msg { };
		while(true)
		{
			while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				
				MessageHandler(msg);
				for(auto& cb : m_msgCallbackList)
				{
					cb(msg);
				}

				if(msg.message == WM_QUIT)
				{
					if(OnQuit(static_cast<int>(msg.wParam)))
					{
						goto LOOP_END;
					}
				}
			}

			// Cursor state updates. Update only when target panel's focus changes.
			if(m_pCursorPanel && m_pCursorPanel->InFocus() != m_bCursorPanelInFocus)
			{
				m_bCursorPanelInFocus = m_pCursorPanel->InFocus();

				if(m_bCursorPanelInFocus)
				{
					if(m_cursorState == CURSOR_STATE_LOCKED)
					{
						const RECT clipRect = { m_cursorPosition.x, m_cursorPosition.y, m_cursorPosition.x, m_cursorPosition.y };
						ClipCursor(&clipRect);

					}
					else if(m_cursorState == CURSOR_STATE_CONFINED)
					{
						const RECT clipRect = { m_pCursorPanel->GetRect().x, m_pCursorPanel->GetRect().y,
							m_pCursorPanel->GetRect().x + m_pCursorPanel->GetRect().w, m_pCursorPanel->GetRect().y + m_pCursorPanel->GetRect().h };
						ClipCursor(&clipRect);
					}
				}
			}
			
			Frame(GetMainPanel()->InFocus());
		}

LOOP_END:
		CFactory::Instance().Release();
		return static_cast<int>(msg.wParam);
	}

	//-----------------------------------------------------------------------------------------------
	// Cursor methods.
	//-----------------------------------------------------------------------------------------------

	// Method for bring the cursor back to its default state.
	void CWinPlatform::UnlockCursor()
	{
		if(m_cursorState == CURSOR_STATE_DEFAULT) { return; }

		m_pCursorPanel = nullptr;
		ClipCursor(nullptr);

		m_cursorState = CURSOR_STATE_DEFAULT;
	}

	// Method locking the cursor to the center to the specified panel.
	void CWinPlatform::CenterCursor(const CPanel* pPanel)
	{
		if(m_pCursorPanel == pPanel && m_cursorState == CURSOR_STATE_LOCKED) { return; }

		m_pCursorPanel = pPanel;
		m_bCursorPanelInFocus = !m_pCursorPanel->InFocus();

		m_cursorPosition.x = m_pCursorPanel->GetRect().x + (m_pCursorPanel->GetRect().w >> 1);
		m_cursorPosition.y = m_pCursorPanel->GetRect().y + (m_pCursorPanel->GetRect().h >> 1);

		m_cursorState = CURSOR_STATE_LOCKED;
	}

	// Method locking the cursor to its current position.
	void CWinPlatform::LockCursor(const CPanel* pPanel)
	{
		if(m_pCursorPanel == pPanel && m_cursorState == CURSOR_STATE_LOCKED) { return; }

		m_pCursorPanel = pPanel;
		m_bCursorPanelInFocus = !m_pCursorPanel->InFocus();

		POINT pt;
		GetCursorPos(&pt);
		m_cursorPosition.x = pt.x;
		m_cursorPosition.y = pt.y;

		m_cursorState = CURSOR_STATE_LOCKED;
	}

	// Method confining the cursor to the center to the specified panel.
	void CWinPlatform::ConfineCursor(const CPanel* pPanel)
	{
		if(m_pCursorPanel == pPanel && m_cursorState == CURSOR_STATE_CONFINED) { return; }

		m_pCursorPanel = pPanel;
		m_bCursorPanelInFocus = !m_pCursorPanel->InFocus();

		m_cursorState = CURSOR_STATE_CONFINED;
	}

	// Method for hiding or showing the cursor.
	void CWinPlatform::SetCursorVisibility(bool bShow)
	{
		CURSORINFO info { };
		info.cbSize = sizeof(CURSORINFO);
		if(GetCursorInfo(&info))
		{
			if(info.flags & CURSOR_SHOWING)
			{
				if(!bShow)
				{
					while(ShowCursor(false) >= 0) { }
				}
			}
			else
			{
				if(bShow)
				{
					while(ShowCursor(true) < 0) { }
				}
			}
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Platform popup methods.
	//-----------------------------------------------------------------------------------------------

	MessageBoxState CWinPlatform::PostMessageBox(const wchar_t* msg, const wchar_t* caption, MessageBoxType type) const
	{
		static const std::unordered_map<MessageBoxType, UINT> MB_TYPE_MAP = {
			{ MessageBoxType::AbortRetryIgnore, MB_ABORTRETRYIGNORE },
			{ MessageBoxType::CancelRetryContinue, MB_CANCELTRYCONTINUE },
			{ MessageBoxType::Help, MB_HELP },
			{ MessageBoxType::Ok, MB_OK },
			{ MessageBoxType::OkCancel, MB_OKCANCEL },
			{ MessageBoxType::RetryCancel, MB_RETRYCANCEL },
			{ MessageBoxType::YesNo, MB_YESNO },
			{ MessageBoxType::YesNoCancel, MB_YESNOCANCEL }
		};

		static const std::unordered_map<int, MessageBoxState> MB_STATE_MAP = {
			{ IDABORT, MessageBoxState::Abort },
			{ IDCANCEL, MessageBoxState::Cancel },
			{ IDCONTINUE, MessageBoxState::Continue },
			{ IDIGNORE, MessageBoxState::Ignore },
			{ IDNO, MessageBoxState::No },
			{ IDOK, MessageBoxState::Ok },
			{ IDRETRY, MessageBoxState::Retry },
			{ IDTRYAGAIN, MessageBoxState::TryAgain },
			{ IDYES, MessageBoxState::Yes },
		};

		CInput::Instance().Reset();

		return MB_STATE_MAP.find(MessageBoxW(nullptr, msg, caption, MB_TYPE_MAP.find(type)->second))->second;
	}

	//-----------------------------------------------------------------------------------------------
	// Platform State methods.
	//-----------------------------------------------------------------------------------------------

	void CWinPlatform::Quit(int exitCode)
	{
		PostQuitMessage(exitCode);
	}

	//-----------------------------------------------------------------------------------------------
	// Platform utility methods.
	//-----------------------------------------------------------------------------------------------

	// Method for refreshing the list of connected monitors and their rects.
	void CWinPlatform::PopulateMonitorList()
	{
		m_monitorDataList.clear();
		EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(this));
	}

	// Method for handling non-hwnd specific messages.
	void CWinPlatform::MessageHandler(const MSG& msg)
	{
		switch(msg.message)
		{
			case WM_DISPLAYCHANGE:
			{
				PopulateMonitorList();
			} break;
			default:
				break;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Monitor population procedures.
	//-----------------------------------------------------------------------------------------------

	BOOL CALLBACK CWinPlatform::MonitorEnumProc(HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM lParam)
	{
		return reinterpret_cast<CWinPlatform*>(lParam)->MonitorProc(hMonitor, hdc, rect, lParam);
	}

	BOOL CALLBACK CWinPlatform::MonitorProc(HMONITOR hMonitor, HDC hdc, LPRECT rect, LPARAM lParam)
	{
		m_monitorDataList.push_back({ Math::Rect(rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top) });
		return TRUE;
	}
};

#endif
