//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoCone.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMOCONE_H
#define CGIZMOCONE_H

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
	class CGizmoCone : public CVComponent
	{
	public:
		friend class CGizmoDraw;

	private:
		struct Data
		{
			float height = 1.5f;
			float radius = 0.5f;
			u32 iterations = 16;
			u32 instanceMax;
			u64 matHash;
		};

	private:
		CGizmoCone(const CVObject* pObject);
		~CGizmoCone();
		CGizmoCone(const CGizmoCone&) = delete;
		CGizmoCone(CGizmoCone&&) = delete;
		CGizmoCone& operator = (const CGizmoCone&) = delete;
		CGizmoCone& operator = (CGizmoCone&&) = delete;

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
