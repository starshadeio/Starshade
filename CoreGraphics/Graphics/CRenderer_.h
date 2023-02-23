//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRenderer_.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CRENDERER__H
#define CRENDERER__H

#include <Objects/CVComponent.h>

namespace Graphics 
{
	// This is legacy to CRenderer, and will eventually be replaced by it.
	class CRenderer_ : public CVComponent
	{
	public:
		friend class CRenderingSystem;

	protected:
		CRenderer_(const CVObject* pObject);
		CRenderer_(const CRenderer_&) = delete;
		CRenderer_(CRenderer_&&) = delete;
		CRenderer_& operator = (const CRenderer_&) = delete;
		CRenderer_& operator = (CRenderer_&&) = delete;

	public:
		void Initialize() override;
		void Release() override;

		// Accessors.
		virtual inline const CVObject* GetObject() const { return m_pObject; }

		inline bool IsActive() const { return m_bActive; }
		virtual inline class CMaterial* GetMaterial() const = 0;

		// Modifiers.
		inline void SetActive(bool bActive) { m_bActive = bActive; }

	protected:
		virtual void Render() = 0;
		virtual void PostInitialize() { }

	private:
		bool m_bActive;
	};
};

#endif
