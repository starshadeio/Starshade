//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoDraw.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMODRAW_H
#define CGIZMODRAW_H

#include "CGizmoLine.h"
#include "CGizmoCircle.h"
#include "CGizmoQuad.h"
#include "CGizmoCube.h"
#include "CGizmoCone.h"
#include <Objects/CVObject.h>

namespace Graphics
{
	class CMaterial;
};

namespace App
{
	class CGizmoDraw : public CVObject
	{
	public:
		CGizmoDraw();
		~CGizmoDraw();
		CGizmoDraw(const CGizmoDraw&) = delete;
		CGizmoDraw(CGizmoDraw&&) = delete;
		CGizmoDraw& operator = (const CGizmoDraw&) = delete;
		CGizmoDraw& operator = (CGizmoDraw&&) = delete;

		void Initialize();
		void Release();

		// Accessors.
		inline CGizmoLine& Line() { return m_gizmoLine; }
		inline CGizmoCircle& Circle() { return m_gizmoCircle; }
		inline CGizmoCircle& HalfCircle() { return m_gizmoHalfCircle; }
		inline CGizmoQuad& Quad() { return m_gizmoQuad; }
		inline CGizmoCube& Cube() { return m_gizmoCube; }
		inline CGizmoCone& Cone() { return m_gizmoCone; }

	private:
		void Render();

	private:
		CGizmoLine m_gizmoLine;
		CGizmoCircle m_gizmoCircle;
		CGizmoCircle m_gizmoHalfCircle;
		CGizmoQuad m_gizmoQuad;
		CGizmoCube m_gizmoCube;
		CGizmoCone m_gizmoCone;

		Graphics::CMaterial* m_pMaterialLine;
		Graphics::CMaterial* m_pMaterialTri;
		Graphics::CMaterial* m_pMaterialTriUnit;
	};
};

#endif
