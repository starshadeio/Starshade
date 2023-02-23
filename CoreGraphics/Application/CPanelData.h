//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CPanelData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPANELDATA_H
#define CPANELDATA_H

#include <functional>

namespace App
{
	typedef std::function<void(const class CPanel*)> PanelAdjustFunc;

	enum PANEL_SCREEN_MODE
	{
		PSM_WINDOWED,
		PSM_BORDERLESS,
		PSM_FULLSCREEN,
		PSM_UNKNOWN,
	};

	enum PANEL_SCREEN_STATE
	{
		PSS_NORMAL,
		PSS_MINIMIZED,
		PSS_MAXIMIZED,
		PSS_UNKNOWN,
	};
};

#endif
