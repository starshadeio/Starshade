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
#include <vector>

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
			std::function<void(class CMaterial*)> onPreRender;
			class CMeshData_* pMeshData;
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
		inline bool IsActiveAt(size_t materialIndex) const final { return CRenderer_::IsActiveAt(materialIndex) && m_pMaterialList.size() > materialIndex; }

		inline class CMaterial* GetMaterialAt(size_t index) final { return m_pMaterialList[index]; }
		inline size_t GetMaterialCount() const final { return m_pMaterialList.size(); }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

		inline void AddMaterial(class CMaterial* pMaterial) { m_pMaterialList.push_back(pMaterial); }
		inline void PopMaterial() { m_pMaterialList.pop_back(); }
		inline void ClearMaterials() { m_pMaterialList.clear(); }

	protected:
		virtual void Render() final;
		void RenderWithMaterial(size_t materialIndex) override;

	protected:
		Data m_data;
		std::vector<class CMaterial*> m_pMaterialList;
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
