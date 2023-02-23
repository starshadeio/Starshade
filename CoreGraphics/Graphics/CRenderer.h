//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshRenderer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CRENDERER_H
#define CRENDERER_H

#include <Objects/CNodeComponent.h>

namespace Graphics
{
	class CRenderer : public CNodeComponent
	{
	public:
		friend class CRenderingSystem;

	public:
		CRenderer(u32 hash);
		~CRenderer();
		CRenderer(const CRenderer&) = delete;
		CRenderer(CRenderer&&) = delete;
		CRenderer& operator = (const CRenderer&) = delete;
		CRenderer& operator = (CRenderer&&) = delete;
		
		// Accessors.
		inline s32 GetDepth() const { return m_depth; }

	protected:
		virtual void Render(u32 viewHash) = 0;

	protected:
		s32 m_depth = 0;
	};
};

#endif
