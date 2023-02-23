//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoCircle.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMOCIRCLE_H
#define CGIZMOCIRCLE_H

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
	class CGizmoCircle : public CVComponent
	{
	public:
		friend class CGizmoDraw;

	private:
		struct Data
		{
			bool bHalfCircle = false;
			float radius = 0.5f;
			u32 iterations = 64;
			u32 instanceMax;
			u64 matHash;
		};

	private:
		CGizmoCircle(const CVObject* pObject);
		~CGizmoCircle();
		CGizmoCircle(const CGizmoCircle&) = delete;
		CGizmoCircle(CGizmoCircle&&) = delete;
		CGizmoCircle& operator = (const CGizmoCircle&) = delete;
		CGizmoCircle& operator = (CGizmoCircle&&) = delete;

		void Initialize();
		void PreRender();
		void Render();
		void Release();

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	public:
		void Draw(const Math::SIMDVector& origin, const Math::SIMDVector& offset, const Math::SIMDQuaternion& rotation, const Math::SIMDVector& scale, float width, const Math::Color& color);

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
