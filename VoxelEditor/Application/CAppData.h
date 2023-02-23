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
	const wchar_t SCENE_NAME_GLOBAL[] = L"Global";
	const wchar_t SCENE_NAME_NODE[] = L"Node";

	const u32 SCENE_HASH_GLOBAL = Math::FNV1a_32(SCENE_NAME_GLOBAL);
	const u32 SCENE_HASH_NODE = Math::FNV1a_32(SCENE_NAME_NODE);
	
	// Views.
	const wchar_t VIEW_NAME_MAIN[] = L"Main";
	const wchar_t VIEW_NAME_OVERLAY[] = L"Overlay";

	const u32 VIEW_HASH_MAIN = Math::FNV1a_32(VIEW_NAME_MAIN);
	const u32 VIEW_HASH_OVERLAY = Math::FNV1a_32(VIEW_NAME_OVERLAY);

	// Input.
	const wchar_t INPUT_LAYOUT_EDITOR[] = L"editor";
	const wchar_t INPUT_LAYOUT_PLAYER[] = L"player";

	const u32 INPUT_HASH_LAYOUT_EDITOR = Math::FNV1a_32(INPUT_LAYOUT_EDITOR);
	const u32 INPUT_HASH_LAYOUT_PLAYER = Math::FNV1a_32(INPUT_LAYOUT_PLAYER);

	const char INPUT_KEY_MOVE_FORWARD[] = "move_forward";
	const char INPUT_KEY_MOVE_BACKWARD[] = "move_backward";
	const char INPUT_KEY_MOVE_UP[] = "move_up";
	const char INPUT_KEY_MOVE_DOWN[] = "move_down";
	const char INPUT_KEY_MOVE_LEFT[] = "move_left";
	const char INPUT_KEY_MOVE_RIGHT[] = "move_right";

	const char INPUT_KEY_LOOK_H[] = "look_horizontal";
	const char INPUT_KEY_LOOK_V[] = "look_vertical";
	
	const char INPUT_KEY_JUMP[] = "jump";
	
	const char INPUT_KEY_SELECT[] = "select";
	const char INPUT_KEY_LOOK[] = "look";
	const char INPUT_KEY_PAN[] = "pan";
	const char INPUT_KEY_ZOOM[] = "zoom";

	const char INPUT_KEY_GIZMO_SELECT[] = "gizmo_select";
	const char INPUT_KEY_GIZMO_TRANSLATE[] = "gizmo_translate";
	const char INPUT_KEY_GIZMO_ROTATE[] = "gizmo_rotate";
	const char INPUT_KEY_GIZMO_SCALE[] = "gizmo_scale";

	const char INPUT_KEY_MODE_ERASE[] = "mode_erase";
	const char INPUT_KEY_MODE_FILL[] = "mode_fill";
	const char INPUT_KEY_MODE_PAINT[] = "mode_paint";
	const char INPUT_KEY_BRUSH_1[] = "brush_1";
	const char INPUT_KEY_BRUSH_2[] = "brush_2";
	const char INPUT_KEY_BRUSH_4[] = "brush_4";
	const char INPUT_KEY_PLAY_SPAWN[] = "play_spawn";
	const char INPUT_KEY_PLAY_AT[] = "play_at";

	const char INPUT_KEY_NEW_PROJECT[] = "new_project";
	const char INPUT_KEY_OPEN_PROJECT[] = "open_project";
	const char INPUT_KEY_SAVE_PROJECT[] = "save_project";
	const char INPUT_KEY_SAVE_PROJECT_AS[] = "save_project_as";
	const char INPUT_KEY_BUILD_PROJECT[] = "build_project";

	const char INPUT_KEY_UNDO[] = "undo";
	const char INPUT_KEY_REDO[] = "redo";

	const char INPUT_KEY_QUIT[] = "quit";

	const char INPUT_KEY_SCREENSHOT[] = "screenshot";
};

#endif
