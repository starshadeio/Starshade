//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CEditorKeybinds.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CEditorKeybinds.h"
#include "CAppData.h"
#include "CEditor.h"
#include <Application/CInput.h>
#include <Application/CInputDeviceList.h>
#include <Application/CInputKeyboard.h>
#include <Application/CInputMouse.h>
#include <Application/CInputGamepad.h>
#include <Application/CCommandManager.h>
#include <Graphics/CGraphicsAPI.h>
#include <Factory/CFactory.h>
#include <Resources/CResourceManager.h>

namespace App
{
	const wchar_t CEditorKeybinds::INPUT_KEY_MOVE_UP[] = L"move_up";
	const wchar_t CEditorKeybinds::INPUT_KEY_MOVE_DOWN[] = L"move_down";

	const wchar_t CEditorKeybinds::INPUT_KEY_LOOK[] = L"look";
	const wchar_t CEditorKeybinds::INPUT_KEY_PAN[] = L"pan";
	const wchar_t CEditorKeybinds::INPUT_KEY_ZOOM[] = L"zoom";

	const wchar_t CEditorKeybinds::INPUT_KEY_GIZMO_SELECT[] = L"gizmo_select";
	const wchar_t CEditorKeybinds::INPUT_KEY_GIZMO_TRANSLATE[] = L"gizmo_translate";
	const wchar_t CEditorKeybinds::INPUT_KEY_GIZMO_ROTATE[] = L"gizmo_rotate";
	const wchar_t CEditorKeybinds::INPUT_KEY_GIZMO_SCALE[] = L"gizmo_scale";
	const wchar_t CEditorKeybinds::INPUT_KEY_WORKSPACE_TOGGLE[] = L"workspace_toggle";
	const wchar_t CEditorKeybinds::INPUT_KEY_WORKSPACE_EDIT[] = L"workspace_edit";
	const wchar_t CEditorKeybinds::INPUT_KEY_WORKSPACE_RESET[] = L"workspace_reset";

	const wchar_t CEditorKeybinds::INPUT_KEY_MODE_ERASE[] = L"mode_erase";
	const wchar_t CEditorKeybinds::INPUT_KEY_MODE_FILL[] = L"mode_fill";
	const wchar_t CEditorKeybinds::INPUT_KEY_MODE_PAINT[] = L"mode_paint";
	const wchar_t CEditorKeybinds::INPUT_KEY_BRUSH_1[] = L"brush_1";
	const wchar_t CEditorKeybinds::INPUT_KEY_BRUSH_2[] = L"brush_2";
	const wchar_t CEditorKeybinds::INPUT_KEY_BRUSH_4[] = L"brush_4";
	const wchar_t CEditorKeybinds::INPUT_KEY_PLAY_SPAWN[] = L"play_spawn";
	const wchar_t CEditorKeybinds::INPUT_KEY_PLAY_AT[] = L"play_at";

	const wchar_t CEditorKeybinds::INPUT_KEY_NEW_PROJECT[] = L"new_project";
	const wchar_t CEditorKeybinds::INPUT_KEY_OPEN_PROJECT[] = L"open_project";
	const wchar_t CEditorKeybinds::INPUT_KEY_SAVE_PROJECT[] = L"save_project";
	const wchar_t CEditorKeybinds::INPUT_KEY_SAVE_PROJECT_AS[] = L"save_project_as";
	const wchar_t CEditorKeybinds::INPUT_KEY_BUILD_PROJECT[] = L"build_project";

	const wchar_t CEditorKeybinds::INPUT_KEY_UNDO[] = L"undo";
	const wchar_t CEditorKeybinds::INPUT_KEY_REDO[] = L"redo";

	const wchar_t CEditorKeybinds::INPUT_KEY_CUT[] = L"cut";
	const wchar_t CEditorKeybinds::INPUT_KEY_COPY[] = L"copy";
	const wchar_t CEditorKeybinds::INPUT_KEY_PASTE[] = L"paste";
	const wchar_t CEditorKeybinds::INPUT_KEY_DELETE[] = L"delete";
	const wchar_t CEditorKeybinds::INPUT_KEY_FILL[] = L"fill";
	const wchar_t CEditorKeybinds::INPUT_KEY_PAINT[] = L"paint";

	const wchar_t CEditorKeybinds::INPUT_KEY_QUIT[] = L"quit";
	const wchar_t CEditorKeybinds::INPUT_KEY_SCREENSHOT[] = L"screenshot";
	
#ifndef PRODUCTION_BUILD
	const wchar_t CEditorKeybinds::INPUT_KEY_LOD_DOWN[] = L"lod_down";
	const wchar_t CEditorKeybinds::INPUT_KEY_LOD_UP[] = L"lod_up";
#endif

	CEditorKeybinds::CEditorKeybinds()
	{
		m_inputHashDefault = INPUT_HASH_LAYOUT_PLAYER;
	}

	CEditorKeybinds::~CEditorKeybinds()
	{
	}

	void CEditorKeybinds::RegisterKeybinds()
	{
		m_configFilename = *reinterpret_cast<std::wstring*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_CONFIG, Math::FNV1a_64(L"CONFIG_KEYBINDS")));

		CKeybinds::RegisterKeybinds();

		// Movement.
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_MOVE_FORWARD, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_UP), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_MOVE_BACKWARD, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_DOWN), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_MOVE_UP, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_PRIOR), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_MOVE_DOWN, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_NEXT), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_MOVE_LEFT, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_LEFT), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_MOVE_RIGHT, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_RIGHT), InputBinding(INPUT_DEVICE_NONE, 0));

		// Look.
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_LOOK_H, nullptr, 
			InputBinding(INPUT_DEVICE_MOUSE, VM_MOUSE_DELTA_X), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_LOOK_V, nullptr, 
			InputBinding(INPUT_DEVICE_MOUSE, VM_MOUSE_DELTA_Y), InputBinding(INPUT_DEVICE_NONE, 0));
		

		// Actions.
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_SELECT, nullptr, 
			InputBinding(INPUT_DEVICE_MOUSE, VM_MOUSE_LEFT), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_LOOK, nullptr, 
			InputBinding(INPUT_DEVICE_MOUSE, VM_MOUSE_RIGHT), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_PAN, nullptr, 
			InputBinding(INPUT_DEVICE_MOUSE, VM_MOUSE_MIDDLE), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_ZOOM, nullptr, 
			InputBinding(INPUT_DEVICE_MOUSE, VM_MOUSE_WHEEL), InputBinding(INPUT_DEVICE_NONE, 0));

		// Gizmos.
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_GIZMO_SELECT, std::bind(&CEditorKeybinds::GizmoSelect, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_Q), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_GIZMO_TRANSLATE, std::bind(&CEditorKeybinds::GizmoTranslate, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_W), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_GIZMO_ROTATE, std::bind(&CEditorKeybinds::GizmoRotate, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_E), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_GIZMO_SCALE, std::bind(&CEditorKeybinds::GizmoScale, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_R), InputBinding(INPUT_DEVICE_NONE, 0));

		// Workspace.
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_WORKSPACE_TOGGLE, std::bind(&CEditorKeybinds::WorkspaceToggle, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_T), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_WORKSPACE_EDIT, std::bind(&CEditorKeybinds::WorkspaceEdit, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_T, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_NONE, 0));

		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_WORKSPACE_RESET, std::bind(&CEditorKeybinds::WorkspaceReset, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_T, INPUT_MODIFIER_FLAG_CTRL_SHIFT), InputBinding(INPUT_DEVICE_NONE, 0));
		
		// Tools.
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_MODE_ERASE, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_D), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_MODE_FILL, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_F), InputBinding(INPUT_DEVICE_NONE, 0));

		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_MODE_PAINT, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_G), InputBinding(INPUT_DEVICE_NONE, 0));

		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_BRUSH_1, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_1), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_BRUSH_2, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_2), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_BRUSH_4, nullptr, 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_3), InputBinding(INPUT_DEVICE_NONE, 0));
		
		// Project Actions.
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_NEW_PROJECT, std::bind(&CEditorKeybinds::NewProject, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_N, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_OPEN_PROJECT, std::bind(&CEditorKeybinds::OpenProject, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_O, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_SAVE_PROJECT, std::bind(&CEditorKeybinds::SaveProject, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_S, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_SAVE_PROJECT_AS, std::bind(&CEditorKeybinds::SaveProjectAs, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_S, INPUT_MODIFIER_FLAG_CTRL_ALT), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_BUILD_PROJECT, std::bind(&CEditorKeybinds::BuildProject, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_B, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_NONE, 0));

		// Edit Actions.
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_UNDO, std::bind(&CEditorKeybinds::Undo, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_Z, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_REDO, std::bind(&CEditorKeybinds::Redo, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_Y, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_CUT, std::bind(&CEditorKeybinds::Cut, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_X, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_COPY, std::bind(&CEditorKeybinds::Copy, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_C, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_PASTE, std::bind(&CEditorKeybinds::Paste, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_V, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_DELETE, std::bind(&CEditorKeybinds::Delete, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_D, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_DELETE));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_FILL, std::bind(&CEditorKeybinds::Fill, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_F, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_PAINT, std::bind(&CEditorKeybinds::Paint, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_G, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_NONE, 0));
		
		// Global Actions.
		const u32 allLayouts[] = { INPUT_HASH_LAYOUT_EDITOR, INPUT_HASH_LAYOUT_PLAYER };
		
		RegisterKeybinding(allLayouts, 2, INPUT_KEY_PLAY_SPAWN, std::bind(&CEditorKeybinds::PlaySpawn, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_P, INPUT_MODIFIER_FLAG_CTRL), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(allLayouts, 2, INPUT_KEY_PLAY_AT, std::bind(&CEditorKeybinds::PlayAt, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_P), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(allLayouts, 2, INPUT_KEY_QUIT, std::bind(&CEditorKeybinds::Quit, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_ESCAPE), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(allLayouts, 2, INPUT_KEY_SCREENSHOT, std::bind(&CEditorKeybinds::Screenshot, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_F5), InputBinding(INPUT_DEVICE_NONE, 0));
		
#ifndef PRODUCTION_BUILD
		// Prototype.
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_LOD_DOWN, std::bind(&CEditorKeybinds::LODDown, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_OEM_LBRACKET), InputBinding(INPUT_DEVICE_NONE, 0));
		
		RegisterKeybinding(&INPUT_HASH_LAYOUT_EDITOR, 1, INPUT_KEY_LOD_UP, std::bind(&CEditorKeybinds::LODUp, this, std::placeholders::_1), 
			InputBinding(INPUT_DEVICE_KEYBOARD, VK_KB_OEM_RBRACKET), InputBinding(INPUT_DEVICE_NONE, 0));
#endif
	}

	//-----------------------------------------------------------------------------------------------
	// Input callbacks.
	//-----------------------------------------------------------------------------------------------
	
	// Gizmos.
	void CEditorKeybinds::GizmoSelect(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CGizmo::CMD_KEY_GIZMO_SELECT);
		}
	}

	void CEditorKeybinds::GizmoTranslate(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CGizmo::CMD_KEY_GIZMO_TRANSLATE);
		}
	}

	void CEditorKeybinds::GizmoRotate(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CGizmo::CMD_KEY_GIZMO_ROTATE);
		}
	}

	void CEditorKeybinds::GizmoScale(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CGizmo::CMD_KEY_GIZMO_SCALE);
		}
	}

	// Workspace.
	void CEditorKeybinds::WorkspaceToggle(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(Universe::CChunkWorkspace::CMD_KEY_WORKSPACE_TOGGLE);
		}
	}

	void CEditorKeybinds::WorkspaceEdit(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CGizmo::CMD_KEY_GIZMO_WORKSPACE);
		}
	}

	void CEditorKeybinds::WorkspaceReset(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(Universe::CChunkWorkspace::CMD_KEY_WORKSPACE_RESET);
		}
	}

	// File Actions.
	void CEditorKeybinds::NewProject(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CProjectManager::CMD_KEY_NEW_PROJECT);
		}
	}

	void CEditorKeybinds::OpenProject(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CProjectManager::CMD_KEY_OPEN_PROJECT);
		}
	}

	void CEditorKeybinds::SaveProject(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CProjectManager::CMD_KEY_SAVE_PROJECT);
		}
	}

	void CEditorKeybinds::SaveProjectAs(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CProjectManager::CMD_KEY_SAVE_PROJECT_AS);
		}
	}

	void CEditorKeybinds::BuildProject(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CProjectManager::CMD_KEY_BUILD_PROJECT);
		}
	}

	// Edit Action.
	void CEditorKeybinds::Undo(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CCommandManager::CMD_KEY_UNDO);
		}
	}

	void CEditorKeybinds::Redo(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CCommandManager::CMD_KEY_REDO);
		}
	}

	void CEditorKeybinds::Cut(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(Universe::CChunkEditor::CMD_KEY_CUT);
		}
	}

	void CEditorKeybinds::Copy(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(Universe::CChunkEditor::CMD_KEY_COPY);
		}
	}

	void CEditorKeybinds::Paste(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(Universe::CChunkEditor::CMD_KEY_PASTE);
		}
	}

	void CEditorKeybinds::Delete(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(Universe::CChunkEditor::CMD_KEY_DELETE);
		}
	}

	void CEditorKeybinds::Fill(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(Universe::CChunkEditor::CMD_KEY_FILL);
		}
	}

	void CEditorKeybinds::Paint(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(Universe::CChunkEditor::CMD_KEY_PAINT);
		}
	}

	// Scene Action.
	void CEditorKeybinds::PlaySpawn(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CAppState::CMD_KEY_TOGGLE_PLAY_SPAWN);
		}
	}

	void CEditorKeybinds::PlayAt(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CAppState::CMD_KEY_TOGGLE_PLAY_AT);
		}
	}

	void CEditorKeybinds::Quit(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(CAppState::CMD_KEY_EDIT);
		}
	}

	void CEditorKeybinds::Screenshot(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			const std::time_t result = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			

			static char buffer[32];
			ctime_s(buffer, 32, &result);

			std::wstringstream wss;
			wss << L"./Screenshots/";
			for(size_t i = 0; buffer[i] != '\n'; ++i)
			{
				if(buffer[i] == ' ' || buffer[i] == ':') wss << L'-';
				else wss << wchar_t(buffer[i]);
			}

			wss << L".png";

			CFactory::Instance().GetGraphicsAPI()->Screenshot(wss.str().c_str());
		}
	}
	
#ifndef PRODUCTION_BUILD
	// Prototype.	
	void CEditorKeybinds::LODDown(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(Universe::CChunkEditor::CMD_KEY_LOD_DOWN);
		}
	}

	void CEditorKeybinds::LODUp(const InputCallbackData& callback)
	{
		if(callback.bPressed)
		{
			CCommandManager::Instance().Execute(Universe::CChunkEditor::CMD_KEY_LOD_UP);
		}
	}
#endif
};
