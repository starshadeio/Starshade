//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshRenderer_.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMESHRENDERER__H
#define CMESHRENDERER__H

#include "CRenderer_.h"
#include <functional>

namespace Graphics
{
	class CMeshForceRender_;
	
	// This is legacy to CMeshRenderer, and will eventually be replaced by it.
	class CMeshRenderer_ : public CRenderer_
	{
	public:
		friend class CRenderingSystem;
		friend class CMeshContainer_;
		friend class CMeshForceRender_;

	public:
		struct Data
		{
			bool bSkipRegistration;
			bool bDynamicInstances;
			bool bDynamicInstanceCount;
			std::function<void()> onPreRender;
			class CMeshData_* pMeshData;
			class CMaterial* pMaterial;
		};

	protected:
		CMeshRenderer_(const CVObject* pObject);
		CMeshRenderer_(const CMeshRenderer_&) = delete;
		CMeshRenderer_(CMeshRenderer_&&) = delete;
		CMeshRenderer_& operator = (const CMeshRenderer_&) = delete;
		CMeshRenderer_& operator = (CMeshRenderer_&&) = delete;

	public:
		virtual ~CMeshRenderer_();

		void Initialize() override;
		void Release() override;

		// Accessors.
		inline class CMaterial* GetMaterial() const final { return m_data.pMaterial; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		//inline void SetMaterial(class CMaterial* pMaterial) { m_data.pMaterial = pMaterial; }

	protected:
		virtual void PostInitialize() = 0;
		virtual void Render() override;

	protected:
		Data m_data;
	};

	class CMeshForceRender_
	{
	public:
		CMeshForceRender_(CMeshRenderer_* pRenderer)
		{
			pRenderer->Render();
		}
	};
};

#endif
