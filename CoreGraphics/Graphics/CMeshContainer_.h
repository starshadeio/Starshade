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
#include "CMeshRenderer_.h"
#include <functional>
#include <vector>

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
			bool bSkipRegistration = false;
			std::function<void(class CMaterial*)> onPreRender;
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
		inline class CMaterial* GetMaterialAt(size_t index) final { return m_pMaterialList[index]; }
		inline size_t GetMaterialCount() const final { return m_pMaterialList.size(); }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

		inline void AddMaterial(class CMaterial* pMaterial) { m_pMaterialList.push_back(pMaterial); }
		inline void ClearMaterials() { m_pMaterialList.clear(); }
		
		inline void SetMeshRenderer(class CMeshRenderer_* pMeshRenderer) { m_data.pMeshRenderer = pMeshRenderer; }
		
	protected:
		void Render() final;

	public:
		void RenderWithMaterial(size_t materialIndex) final;

	private:
		Data m_data;
		std::vector<class CMaterial*> m_pMaterialList;
	};
};

#endif
