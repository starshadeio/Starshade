//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoLine.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMOLINE_H
#define CGIZMOLINE_H

#include "CGizmoData.h"
#include <Graphics/CMeshData_.h>
#include <Objects/CVComponent.h>
#include <vector>
#include <deque>

namespace Graphics
{
	class CMeshRenderer_;
	class CMaterial;
	class CGraphicsAPI;
};

namespace App
{
	class CGizmoLine : public CVComponent
	{
	public:
		friend class CGizmoDraw;

	private:
		struct Data
		{
			u32 instanceMax;
			u64 matHash;
		};

	private:
		CGizmoLine(const CVObject* pObject);
		~CGizmoLine();
		CGizmoLine(const CGizmoLine&) = delete;
		CGizmoLine(CGizmoLine&&) = delete;
		CGizmoLine& operator = (const CGizmoLine&) = delete;
		CGizmoLine& operator = (CGizmoLine&&) = delete;

		void Initialize();
		void PreRender();
		void Render();
		void Release();

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	public:
		void Draw(const Math::SIMDVector& origin, const Math::SIMDVector& offset, const Math::SIMDVector& dir, float length, float width, const Math::Color& color);

	private:
		Data m_data;

		std::deque<GizmoDrawLine> m_drawQueue;

		Graphics::CMeshData_ m_meshData;
		Graphics::CMeshRenderer_* m_pMeshRenderer;
		Graphics::CMaterial* m_pMaterial;

		Graphics::CGraphicsAPI* m_pGraphicsAPI;
	};
};

#endif
