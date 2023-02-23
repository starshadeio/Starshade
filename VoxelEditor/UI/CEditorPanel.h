//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: UI/CEditorPanel.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CEDITORPANEL_H
#define CEDITORPANEL_H

#include "CPane.h"
#include <UI/CUICanvas.h>
#include <UI/CUIImage.h>
#include <Objects/CVObject.h>

namespace UI
{
	class CEditorPanel : public CVObject
	{
	public:
		CEditorPanel();
		~CEditorPanel();
		CEditorPanel(const CEditorPanel&) = delete;
		CEditorPanel(CEditorPanel&&) = delete;
		CEditorPanel& operator = (const CEditorPanel&) = delete;
		CEditorPanel& operator = (CEditorPanel&&) = delete;

		void Initialize();
		void LateUpdate();
		void Release();
		
		void OnPlay();
		void OnEdit();

		void OnResize(const Math::Rect& rect);

	private:
		CUICanvas m_canvasSceneView;
		CUICanvas m_canvasEditor;

		CPane m_paneSceneView;
		CPane m_paneEditor;
	};
};

#endif
