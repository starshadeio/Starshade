//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
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
	// Scenes.
	const wchar_t SCENE_NAME_GLOBAL[] = L"global";
	const wchar_t SCENE_NAME_NODE[] = L"node";

	const u32 SCENE_HASH_GLOBAL = Math::FNV1a_32(SCENE_NAME_GLOBAL);
	const u32 SCENE_HASH_NODE = Math::FNV1a_32(SCENE_NAME_NODE);
	
	// Views.
	const wchar_t VIEW_NAME_MAIN[] = L"main";
	const wchar_t VIEW_NAME_OVERLAY[] = L"overlay";

	const u32 VIEW_HASH_MAIN = Math::FNV1a_32(VIEW_NAME_MAIN);
	const u32 VIEW_HASH_OVERLAY = Math::FNV1a_32(VIEW_NAME_OVERLAY);

	// Input.
	const wchar_t INPUT_LAYOUT_EDITOR[] = L"editor";
	const wchar_t INPUT_LAYOUT_PLAYER[] = L"player";

	const u32 INPUT_HASH_LAYOUT_EDITOR = Math::FNV1a_32(INPUT_LAYOUT_EDITOR);
	const u32 INPUT_HASH_LAYOUT_PLAYER = Math::FNV1a_32(INPUT_LAYOUT_PLAYER);
};

#endif
