//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDebugDraw.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDEBUGDRAW_H
#define CDEBUGDRAW_H

#include "CDebugRect.h"
#include <Objects/CVObject.h>

namespace Graphics
{
	class CDebugDraw : public CVObject
	{
	public:
		friend class CRenderingSystem;

	public:
		CDebugDraw();
		~CDebugDraw();
		CDebugDraw(const CDebugDraw&) = delete;
		CDebugDraw(CDebugDraw&&) = delete;
		CDebugDraw& operator = (const CDebugDraw&) = delete;
		CDebugDraw& operator = (CDebugDraw&&) = delete;

		void Initialize() final;
		void Release() final;

		inline CDebugRect& Rect() { return m_debugRect; }

	private:
		void Render();

	private:
		CDebugRect m_debugRect;

		class CMaterial* m_pMaterial3D;
		class CMaterial* m_pMaterial2D;
	};
};

#endif
