//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDebugRect.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDEBUGRECT_H
#define CDEBUGRECT_H

#include "CDebug2D.h"
#include "../Graphics/CMeshData_.h"
#include <Objects/CVComponent.h>
#include <Math/CMathVector2.h>
#include <Math/CMathMatrix3x2.h>
#include <vector>
#include <deque>

namespace Graphics
{
	class CMeshRenderer_;
	class CMaterial;
	class CGraphicsAPI;
};

namespace Graphics
{
	class CDebugRect : public CVComponent
	{
	public:
		friend class CDebugDraw;

	public:
		struct Data
		{
			u32 instanceMax;
			u64 matHash;
		};

	private:
		CDebugRect(const CVObject* pObject);
		~CDebugRect();
		CDebugRect(const CDebugRect&) = delete;
		CDebugRect(CDebugRect&&) = delete;
		CDebugRect& operator = (const CDebugRect&) = delete;
		CDebugRect& operator = (CDebugRect&&) = delete;

		void Initialize();
		void Render();
		void Release();

	public:
		void Draw(const Math::Vector2& position, const Math::Vector2& scale = 1.0f, float rotation = 0.0f, const Math::Color& color = Math::COLOR_WHITE, float time = 0.0f);
		void Draw(const Math::Matrix3x2& mtx, const Math::Color& color = Math::COLOR_WHITE, float time = 0.0f);
		void Draw(const DebugDraw2D& data);
		
	private:
		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void PreRender();

	private:
		Data m_data;

		std::deque<DebugDraw2D> m_drawQueue;

		Graphics::CMeshData_ m_meshData;
		Graphics::CMeshRenderer_* m_pMeshRenderer;
		Graphics::CMaterial* m_pMaterial;

		Graphics::CGraphicsAPI* m_pGraphicsAPI;
	};
};

#endif
