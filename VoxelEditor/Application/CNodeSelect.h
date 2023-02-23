//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CNodeSelect.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODESELECT_H
#define CNODESELECT_H

#include "CBlockMarker.h"
#include <Logic/CCallback.h>
#include <Graphics/CPrimQuad.h>
#include <Globals/CGlobals.h>
#include <Physics/CPhysicsData.h>
#include <Math/CMathFNV.h>
#include <shared_mutex>
#include <mutex>

namespace App
{
	class CNodeSelect
	{
	public:
		static const u64 CMD_KEY_TOGGLE_SELECT_MODE = Math::FNV1a_64("toggle_select_mode");
		static const u64 CMD_KEY_TOGGLE_BRUSH_SIZE = Math::FNV1a_64("toggle_brush_size");
		
		static const u64 CMD_KEY_SELECT_ERASE = Math::FNV1a_64("select_erase");
		static const u64 CMD_KEY_SELECT_FILL = Math::FNV1a_64("select_fill");
		static const u64 CMD_KEY_SELECT_PAINT = Math::FNV1a_64("select_paint");
		
		static const u64 CMD_KEY_SELECT_ERASE_OFF = Math::FNV1a_64("select_erase_off");
		static const u64 CMD_KEY_SELECT_FILL_OFF = Math::FNV1a_64("select_fill_off");
		static const u64 CMD_KEY_SELECT_PAINT_OFF = Math::FNV1a_64("select_paint_off");
		
		static const u64 CMD_KEY_SELECT_BRUSH_1 = Math::FNV1a_64("select_brush_1");
		static const u64 CMD_KEY_SELECT_BRUSH_2 = Math::FNV1a_64("select_brush_2");
		static const u64 CMD_KEY_SELECT_BRUSH_4 = Math::FNV1a_64("select_brush_4");
		
		static const u64 CMD_KEY_SELECT_BRUSH_1_OFF = Math::FNV1a_64("select_brush_1_off");
		static const u64 CMD_KEY_SELECT_BRUSH_2_OFF = Math::FNV1a_64("select_brush_2_off");
		static const u64 CMD_KEY_SELECT_BRUSH_4_OFF = Math::FNV1a_64("select_brush_4_off");

		static const u64 CMD_KEY_SELECT_ACTION = Math::FNV1a_64("select_action");
		static const u64 CMD_KEY_SELECT_PRESSED = Math::FNV1a_64("select_pressed");
		static const u64 CMD_KEY_SELECT_RELEASED = Math::FNV1a_64("select_released");

	public:
		struct Data
		{
			float maxDistance = 100.0f;
			class CGizmo* pGizmo;
		};
		
		struct PickedInfo
		{
			u32 val;
			Physics::RaycastInfo info;
			Math::SIMDVector coord;
			CNodeSelect* pNodeSelector;
		};

		enum class SelectionType
		{
			Erase,
			Fill,
			Paint,
			COUNT,
		};

		enum class BrushSize
		{
			_1,
			_2,
			_4,
		};

	public:
		CNodeSelect();
		~CNodeSelect();
		CNodeSelect(const CNodeSelect&) = delete;
		CNodeSelect(CNodeSelect&&) = delete;
		CNodeSelect& operator = (const CNodeSelect&) = delete;
		CNodeSelect& operator = (CNodeSelect&&) = delete;
		
		void Initialize();
		void Update();
		void PhysicsUpdate();
		void Release();

		void OnPlay();
		void OnEdit();
		
		// Accessors.
		inline SelectionType GetSelectionType() const { return m_selectionType; }

		inline Math::CSIMDRay GetScreenRay() const
		{
			std::shared_lock<std::shared_mutex> lk(m_rayMutex);
			return m_ray;
		}

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		bool ToggleMode();
		bool ToggleSize();
		bool EraseMode();
		bool FillMode();
		bool PaintMode();
		bool Brush1();
		bool Brush2();
		bool Brush4();
		bool Select(const void* param, bool bInverse);

		void UpdateSelectionQuadState(bool bEnabled);

	private:
		mutable std::mutex m_pickingMutex;
		mutable std::shared_mutex m_rayMutex;

		bool m_bActive;

		Data m_data;
		SelectionType m_selectionType;
		BrushSize m_brushSize;
		Math::CSIMDRay m_ray;
		Math::SIMDVector m_markerPoint;
		
		CBlockMarker m_markerErase;
		CBlockMarker m_markerFill;
		CBlockMarker m_markerPaint;

		PickedInfo m_pickedInfo;
		Logic::CCallback* m_pPickedCallback;
	};
};

#endif
