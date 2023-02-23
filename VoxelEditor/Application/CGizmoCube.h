//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoCube.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMOCUBE_H
#define CGIZMOCUBE_H

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
	class CGizmoCube : public CVComponent
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
		CGizmoCube(const CVObject* pObject);
		~CGizmoCube();
		CGizmoCube(const CGizmoCube&) = delete;
		CGizmoCube(CGizmoCube&&) = delete;
		CGizmoCube& operator = (const CGizmoCube&) = delete;
		CGizmoCube& operator = (CGizmoCube&&) = delete;

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
