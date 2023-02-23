//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoQuad.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMOQUAD_H
#define CGIZMOQUAD_H

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
	class CGizmoQuad : public CVComponent
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
		CGizmoQuad(const CVObject* pObject);
		~CGizmoQuad();
		CGizmoQuad(const CGizmoQuad&) = delete;
		CGizmoQuad(CGizmoQuad&&) = delete;
		CGizmoQuad& operator = (const CGizmoQuad&) = delete;
		CGizmoQuad& operator = (CGizmoQuad&&) = delete;

		void Initialize();
		void PreRender();
		void Render();
		void Release();

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	public:
		void Draw(const Math::SIMDVector& origin, const Math::SIMDVector& offset, const Math::SIMDQuaternion& rotation, const Math::SIMDVector& scale, const Math::Color& color);

	private:
		Data m_data;

		std::deque<GizmoDrawGeo> m_drawQueue;

		Graphics::CMeshData_ m_meshData;
		Graphics::CMeshRenderer_* m_pMeshRenderer;
		Graphics::CMaterial* m_pMaterial;

		Graphics::CGraphicsAPI* m_pGraphicsAPI;
	};
};

#endif
