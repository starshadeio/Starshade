//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CEditorKeybinds.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CEDITORKEYBINDS_H
#define CEDITORKEYBINDS_H

#include <Application/CKeybinds.h>

namespace App
{
	class CEditorKeybinds : public CKeybinds
	{
	public:
		static const wchar_t INPUT_KEY_MOVE_UP[];
		static const wchar_t INPUT_KEY_MOVE_DOWN[];

		static const wchar_t INPUT_KEY_LOOK[];
		static const wchar_t INPUT_KEY_PAN[];
		static const wchar_t INPUT_KEY_ZOOM[];

		static const wchar_t INPUT_KEY_GIZMO_SELECT[];
		static const wchar_t INPUT_KEY_GIZMO_TRANSLATE[];
		static const wchar_t INPUT_KEY_GIZMO_ROTATE[];
		static const wchar_t INPUT_KEY_GIZMO_SCALE[];
		static const wchar_t INPUT_KEY_WORKSPACE_TOGGLE[];
		static const wchar_t INPUT_KEY_WORKSPACE_EDIT[];
		static const wchar_t INPUT_KEY_WORKSPACE_RESET[];

		static const wchar_t INPUT_KEY_MODE_ERASE[];
		static const wchar_t INPUT_KEY_MODE_FILL[];
		static const wchar_t INPUT_KEY_MODE_PAINT[];
		static const wchar_t INPUT_KEY_BRUSH_1[];
		static const wchar_t INPUT_KEY_BRUSH_2[];
		static const wchar_t INPUT_KEY_BRUSH_4[];
		static const wchar_t INPUT_KEY_PLAY_SPAWN[];
		static const wchar_t INPUT_KEY_PLAY_AT[];

		static const wchar_t INPUT_KEY_NEW_PROJECT[];
		static const wchar_t INPUT_KEY_OPEN_PROJECT[];
		static const wchar_t INPUT_KEY_SAVE_PROJECT[];
		static const wchar_t INPUT_KEY_SAVE_PROJECT_AS[];
		static const wchar_t INPUT_KEY_BUILD_PROJECT[];

		static const wchar_t INPUT_KEY_UNDO[];
		static const wchar_t INPUT_KEY_REDO[];

		static const wchar_t INPUT_KEY_CUT[];
		static const wchar_t INPUT_KEY_COPY[];
		static const wchar_t INPUT_KEY_PASTE[];
		static const wchar_t INPUT_KEY_DELETE[];
		static const wchar_t INPUT_KEY_FILL[];
		static const wchar_t INPUT_KEY_PAINT[];

		static const wchar_t INPUT_KEY_QUIT[];
		static const wchar_t INPUT_KEY_SCREENSHOT[];
	
#ifndef PRODUCTION_BUILD
		static const wchar_t INPUT_KEY_LOD_DOWN[];
		static const wchar_t INPUT_KEY_LOD_UP[];
#endif

	public:
		CEditorKeybinds();
		~CEditorKeybinds();
		CEditorKeybinds(const CEditorKeybinds&) = delete;
		CEditorKeybinds(CEditorKeybinds&&) = delete;
		CEditorKeybinds& operator = (const CEditorKeybinds&) = delete;
		CEditorKeybinds& operator = (CEditorKeybinds&&) = delete;

	protected:
		void RegisterKeybinds() final;

	private:
		void GizmoSelect(const InputCallbackData& callback);
		void GizmoTranslate(const InputCallbackData& callback);
		void GizmoRotate(const InputCallbackData& callback);
		void GizmoScale(const InputCallbackData& callback);
		void WorkspaceToggle(const InputCallbackData& callback);
		void WorkspaceEdit(const InputCallbackData& callback);
		void WorkspaceReset(const InputCallbackData& callback);

		void NewProject(const InputCallbackData& callback);
		void OpenProject(const InputCallbackData& callback);
		void SaveProject(const InputCallbackData& callback);
		void SaveProjectAs(const InputCallbackData& callback);
		void BuildProject(const InputCallbackData& callback);

		void Undo(const InputCallbackData& callback);
		void Redo(const InputCallbackData& callback);

		void Cut(const InputCallbackData& callback);
		void Copy(const InputCallbackData& callback);
		void Paste(const InputCallbackData& callback);
		void Delete(const InputCallbackData& callback);
		void Fill(const InputCallbackData& callback);
		void Paint(const InputCallbackData& callback);

		void PlaySpawn(const InputCallbackData& callback);
		void PlayAt(const InputCallbackData& callback);
		void Quit(const InputCallbackData& callback);
		void Screenshot(const InputCallbackData& callback);
		
#ifndef PRODUCTION_BUILD
		void LODDown(const InputCallbackData& callback);
		void LODUp(const InputCallbackData& callback);
#endif

	private:
	};
};

#endif
