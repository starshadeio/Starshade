//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoNode.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMONODE_H
#define CGIZMONODE_H

#include "CGizmoPivot.h"
#include "CGizmoTranslate.h"
#include "CGizmoRotate.h"
#include "CGizmoScale.h"
#include "CNodeObjectEx.h"
#include "CNodeEx.h"
#include <Logic/CNodeTransform.h>
#include <Graphics/CPrimCube.h>
#include <Math/CMathFNV.h>

namespace App
{
	class CGizmoNode
	{
	public:
		static const u64 CMD_KEY_TRANSLATE = Math::FNV1a_64(L"node_translate");
		static const u64 CMD_KEY_ROTATE = Math::FNV1a_64(L"node_rotate");
		static const u64 CMD_KEY_SCALE = Math::FNV1a_64(L"node_scale");

	public:
		struct Data
		{
			float coneSize = 0.175f;
			float planeSize = 0.2625f;

			float axisWidth = 2.0f;
			float axisHoveredWidth = 3.0f;
			float coneHoveredScale = 1.25f;
		};

	public:
		CGizmoNode();
		~CGizmoNode();
		CGizmoNode(const CGizmoNode&) = delete;
		CGizmoNode(CGizmoNode&&) = delete;
		CGizmoNode& operator = (const CGizmoNode&) = delete;
		CGizmoNode& operator = (CGizmoNode&&) = delete;

		void Initialize();
		void Update();
		void LateUpdate();

		// Accessors.
		inline const Data& GetData() const { return m_data; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetNode(CNodeEx* pNode) { m_gizmoPivot.SetNode(pNode); }

	private:
		// Modifiers.
		bool Translate(const void* param, bool bInverse);
		bool Rotate(const void* param, bool bInverse);
		bool Scale(const void* param, bool bInverse);
		
	private:
		Data m_data;

		CGizmoTranslate m_gizmoTranslate;
		CGizmoRotate m_gizmoRotate;
		CGizmoScale m_gizmoScale;

		CGizmoPivot m_gizmoPivot;
	};
};

#endif
