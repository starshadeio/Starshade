//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel App
//
// File: Application/CAppData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAPPDATA_H
#define CAPPDATA_H

#include <Globals/CGlobals.h>
#include <Math/CMathFNV.h>

namespace App
{
	// Scene.
	const wchar_t SCENE_NAME_GLOBAL[] = L"global";
	const u32 SCENE_HASH_GLOBAL = Math::FNV1a_32(SCENE_NAME_GLOBAL);
	
	// Input.
	const wchar_t INPUT_LAYOUT_PLAYER[] = L"player";
	const u32 INPUT_HASH_LAYOUT_PLAYER = Math::FNV1a_32(INPUT_LAYOUT_PLAYER);

	const char INPUT_KEY_QUIT[] = "quit";
	const char INPUT_KEY_SCREENSHOT[] = "screenshot";
};

#endif
