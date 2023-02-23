//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshContainer_.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMESHCONTAINER__H
#define CMESHCONTAINER__H

#include "CRenderer_.h"
#include <functional>

namespace Graphics
{
	// This is legacy, and will eventually be removed.
	class CMeshContainer_ : public CRenderer_
	{
	public:
		friend class CRenderingSystem;

	public:
		struct Data
		{
			std::function<void()> onPreRender;
			class CMaterial* pMaterial;
			class CMeshRenderer_* pMeshRenderer;
		};

	public:
		CMeshContainer_(const CVObject* pObject);
		CMeshContainer_(const CMeshContainer_&) = delete;
		CMeshContainer_(CMeshContainer_&&) = delete;
		CMeshContainer_& operator = (const CMeshContainer_&) = delete;
		CMeshContainer_& operator = (CMeshContainer_&&) = delete;

	public:
		void Initialize() final;
		void Release() final;
		
		// Accessors.
		virtual inline class CMaterial* GetMaterial() const final { return m_data.pMaterial; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetMeshRenderer(class CMeshRenderer_* pMeshRenderer) { m_data.pMeshRenderer = pMeshRenderer; }
		
	protected:
		virtual void PostInitialize() { }
		virtual void Render() final;

	private:
		Data m_data;
	};
};

#endif
