//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Actors/CUser.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUSER_H
#define CUSER_H

#include "CMotorMonitor.h"
#include <Actors/CPlayer.h>

namespace Actor
{
	class CUser : public CPlayer
	{
	public:
		CUser(const wchar_t* pName, u32 viewHash);
		~CUser();
		CUser(const CUser&) = delete;
		CUser(CUser&&) = delete;
		CUser& operator = (const CUser&) = delete;
		CUser& operator = (CUser&&) = delete;
		
		void Initialize() final;
		void PostInitialize() final;
		void Release() final;

		void OnPlay(bool bSpawn);
		void OnEdit();

		void Save() const;
		void Load();

	protected:
		void MoveUp(const App::InputCallbackData& callback);
		void MoveDown(const App::InputCallbackData& callback);

		void Select(const App::InputCallbackData& callback);
		void Look(const App::InputCallbackData& callback);
		void Pan(const App::InputCallbackData& callback);
		void Zoom(const App::InputCallbackData& callback);

		void GizmoSelect(const App::InputCallbackData& callback);
		void GizmoTranslate(const App::InputCallbackData& callback);
		void GizmoRotate(const App::InputCallbackData& callback);
		void GizmoScale(const App::InputCallbackData& callback);

		void ModeErase(const App::InputCallbackData& callback);
		void ModeFill(const App::InputCallbackData& callback);
		void ModePaint(const App::InputCallbackData& callback);
		void Brush1(const App::InputCallbackData& callback);
		void Brush2(const App::InputCallbackData& callback);
		void Brush4(const App::InputCallbackData& callback);

		void PlaySpawn(const App::InputCallbackData& callback);
		void PlayAt(const App::InputCallbackData& callback);

		void NewProject(const App::InputCallbackData& callback);
		void OpenProject(const App::InputCallbackData& callback);
		void SaveProject(const App::InputCallbackData& callback);
		void SaveProjectAs(const App::InputCallbackData& callback);
		void BuildProject(const App::InputCallbackData& callback);

		void Undo(const App::InputCallbackData& callback);
		void Redo(const App::InputCallbackData& callback);

		void Quit(const App::InputCallbackData& callback);
		void Screenshot(const App::InputCallbackData& callback);
		
	private:
		CMotorMonitor m_motorMonitor;
	};
};

#endif
