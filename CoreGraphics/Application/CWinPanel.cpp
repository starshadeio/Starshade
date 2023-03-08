//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CWinPanel.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef PLATFORM_WINDOWS

#include "CWinPanel.h"
#include "CWinPlatform.h"
#include "../Factory/CFactory.h"
#include <Utilities/CDebugError.h>

namespace App
{
	CWinPanel::CWinPanel() :
		m_bMoving(false),
		m_bResizing(false),
		m_hWnd(nullptr) {
	}

	CWinPanel::~CWinPanel() { }

	void CWinPanel::Initialize()
	{
		m_pWinPlatform = dynamic_cast<CWinPlatform*>(CFactory::Instance().GetPlatform());

		WNDCLASSEX wc = { 0 };

		// Setup the window class structure.
		wc.cbSize = sizeof(WNDCLASSEX);

		wc.style = CS_HREDRAW | CS_VREDRAW;
		if(m_data.bAllowDoubleClick) wc.style |= CS_DBLCLKS;

		wc.lpszClassName = m_data.title.c_str();
		wc.lpfnWndProc = CWinPanel::DisplayWinProc;
		wc.hInstance = m_pWinPlatform->GetWinData().hInstance;
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		
		if(m_data.icon.size())
		{
			wc.hIcon = (HICON)LoadImage(nullptr, m_data.icon.c_str(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		}
		else
		{
			wc.hIcon = nullptr;
		}

		// Register class for window creation prep.
		RegisterClassEx(&wc);

		DWORD style = CalculateStyle();
		RECT styleRect = { 0, 0, m_data.targetDims.x, m_data.targetDims.y };
		ASSERT(AdjustWindowRect(&styleRect, style, false));

		int width = styleRect.right - styleRect.left;
		int height = styleRect.bottom - styleRect.top;
		{ // Update target with style.
			const Math::Rect& bounds = m_pWinPlatform->GetMonitor(m_data.targetMonitor).bounds;
			m_panelRect.x = (bounds.w >> 1) - (width >> 1);
			m_panelRect.y = (bounds.h >> 1) - (height >> 1);
		}

		// Create the window handle.
		m_hWnd = CreateWindowExW(
			0,
			m_data.title.c_str(),
			m_data.title.c_str(),
			style,
			m_panelRect.x,
			m_panelRect.y,
			width,
			height,
			m_data.pParent ? ((CWinPanel*)m_data.pParent)->m_hWnd : nullptr,
			nullptr,
			m_pWinPlatform->GetWinData().hInstance,
			nullptr
		);

		ASSERT_R(m_hWnd);

		// Calculate adjusted bounds.
		m_panelRect.x = m_panelRect.x - styleRect.left;
		m_panelRect.y = m_panelRect.y - styleRect.top;
		m_panelRect.w = m_data.targetDims.x;
		m_panelRect.h = m_data.targetDims.y;
	
		// Display the newly created window.
		SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);
		ShowWindow(m_hWnd, m_pWinPlatform->GetWinData().nCmdShow);
		SetForegroundWindow(m_hWnd);
	}

	void CWinPanel::Release()
	{
		if(m_hWnd)
		{
			DestroyWindow(m_hWnd);
			m_hWnd = nullptr;
		}

		UnregisterClassW(m_data.title.c_str(), m_pWinPlatform->GetWinData().hInstance);
	}

	//-----------------------------------------------------------------------------------------------
	// Window utility methods.
	//-----------------------------------------------------------------------------------------------

	// Method for calculating the bounds of the monitor in which the panel inhabits.
	bool CWinPanel::CalculateMonitorBounds(Math::Rect& bounds)
	{
		MONITORINFO minfo;
		minfo.cbSize = sizeof(minfo);
		if(GetMonitorInfoW(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), &minfo))
		{
			bounds.x = minfo.rcMonitor.left;
			bounds.y = minfo.rcMonitor.top;
			bounds.w = minfo.rcMonitor.right - minfo.rcMonitor.left;
			bounds.h = minfo.rcMonitor.bottom - minfo.rcMonitor.top;
			return true;
		}

		return false;
	}

	// Window style calculation methods.
	DWORD CWinPanel::CalculateStyle() const
	{
		if(m_data.bAllowResize) 
		{
			return WS_VISIBLE | WS_OVERLAPPEDWINDOW;
		}
		else
		{
			return WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Window procedures.
	//-----------------------------------------------------------------------------------------------

	// Static window procedure used to attach the member procedure to this application window.
	LRESULT CALLBACK CWinPanel::DisplayWinProc(HWND hWnd, u32 msg, WPARAM wParam, LPARAM lParam)
	{
		if(msg == WM_CREATE)
		{
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
			return 0;
		}
		else
		{
			CWinPanel* pDestination = reinterpret_cast<CWinPanel*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			if(pDestination)
			{
				return pDestination->WinProc(hWnd, msg, wParam, lParam);
			}
			else
			{
				return DefWindowProc(hWnd, msg, wParam, lParam);
			}
		}
	}

	// Member window procedure method.
	LRESULT CALLBACK CWinPanel::WinProc(HWND hWnd, u32 msg, WPARAM wParam, LPARAM lParam)
	{
		switch(msg)
		{
			case WM_CLOSE:
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;

				// Settings.
			case WM_GETMINMAXINFO: {
				LPMINMAXINFO pMMI = (LPMINMAXINFO)lParam;
				pMMI->ptMinTrackSize.x = m_data.minimumDims.x;
				pMMI->ptMinTrackSize.y = m_data.minimumDims.y;
			} return 0;

				// Focus.
			case WM_KILLFOCUS:
				OnFocus(false);
				return 0;
			case WM_SETFOCUS:
				OnFocus(true);
				return 0;

				// Hover.
			case WM_MOUSEMOVE:
			{
				if(OnHover(true))
				{
					// Make sure that an on mouse leave tracker is active when the cursor is in the client area.
					TRACKMOUSEEVENT trackEvent;
					trackEvent.cbSize = sizeof(trackEvent);
					trackEvent.dwFlags = TME_LEAVE;
					trackEvent.dwHoverTime = HOVER_DEFAULT;
					trackEvent.hwndTrack = m_hWnd;
					TrackMouseEvent(&trackEvent);
				}
			} return 0;
			case WM_MOUSELEAVE:
			{
				OnHover(false);
			} return 0;

			// Adjust.
			case WM_MOVE:
			{ // Update inhabited monitor bounds and panel position.
				m_bMoving = true;
				m_panelRect.x = static_cast<int>(LOWORD(lParam));
				m_panelRect.y = static_cast<int>(HIWORD(lParam));
			} return 0;
			case WM_SIZE:
			{ // If the window has been resized.
				const int width = static_cast<int>(LOWORD(lParam));
				const int height = static_cast<int>(HIWORD(lParam));

				switch(wParam)
				{ // Detect type of size change.
					case SIZE_MINIMIZED:
					{
						m_screenState = PSS_MINIMIZED;
					}	break;
					case SIZE_MAXIMIZED:
					{
						if(m_screenState != PSS_MAXIMIZED)
						{
							m_screenState = PSS_MAXIMIZED;
							if(width != m_panelRect.w || height != m_panelRect.h)
							{
								m_panelRect.w = width; m_panelRect.h = height;
								OnResize();
							}
						}
					}	break;
					case SIZE_RESTORED:
					{
						const bool bResized = width != m_panelRect.w || height != m_panelRect.h;
						if(m_screenState != PSS_UNKNOWN)
						{
							if(m_screenState != PSS_NORMAL)
							{
								m_screenState = PSS_NORMAL;
								if(bResized)
								{
									m_panelRect.w = width; m_panelRect.h = height;
									OnResize();
								}
							}
							else
							{
								if(bResized)
								{ // Resize is occurring via edge/corner drag.
									m_panelRect.w = width; m_panelRect.h = height;
									OnResize();
								}
							}
						}
						else
						{
							m_screenState = PSS_NORMAL;
							m_panelRect.w = width; m_panelRect.h = height;
							OnResize();
						}

					}	break;
				}

			} return 0;
			case WM_EXITSIZEMOVE:
			{
				if(m_bMoving)
				{
					if(!m_bResizing)
					{
						OnMove();
					}

					m_bMoving = false;
				}

				if(m_bResizing)
				{
					OnResize();
					m_bResizing = false;
				}
			} return 0;

			default:
				return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}
};

#endif
