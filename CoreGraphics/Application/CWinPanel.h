//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CWinPanel.h
//
//-------------------------------------------------------------------------------------------------

#include "CPanel.h"
#include <Windows.h>

namespace App
{
	class CWinPanel : public CPanel
	{
	public:
		CWinPanel();
		~CWinPanel();
		CWinPanel(const CWinPanel&) = delete;
		CWinPanel(CWinPanel&&) = delete;
		CWinPanel& operator = (const CWinPanel&) = delete;
		CWinPanel& operator = (CWinPanel&&) = delete;

		void Initialize() final;
		void Release() final;

	public:
		// Accessors.
		inline HWND GetWindowHandle() const { return m_hWnd; }

		// Modifiers.
		inline void SetTitle(const wchar_t* pTitle) const final { SetWindowTextW(m_hWnd, pTitle); }

	private:
		bool CalculateMonitorBounds(Math::Rect& bounds);
		DWORD CalculateStyle() const;

		static LRESULT CALLBACK DisplayWinProc(HWND hWnd, u32 msg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK WinProc(HWND hWnd, u32 msg, WPARAM wParam, LPARAM lParam);

	private:
		bool m_bMoving;
		bool m_bResizing;

		HWND m_hWnd;
		class CWinPlatform* m_pWinPlatform;
	};
};
