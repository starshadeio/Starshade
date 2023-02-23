//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshRenderer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMESHRENDERER_H
#define CMESHRENDERER_H

#include "CRenderer.h"
#include <Math/CMathFNV.h>
#include <unordered_map>
#include <functional>

namespace Graphics
{
	class CMeshRenderer : public CRenderer
	{
	public:
		static const u32 HASH = Math::FNV1a_32(L"Mesh Renderer");

	public:
		struct Data
		{
		public:
			Data(u64 thisHash) : m_this(thisHash) { }
			virtual ~Data() { }
			Data(const Data&) = default;
			Data(Data&&) = default;
			Data& operator = (const Data&) = default;
			Data& operator = (Data&&) = default;

			virtual void Initialize();
			virtual void Render();

			// Accessors.
			inline bool IsActive() const { return m_bActive; }

			// Modifiers.
			inline void UseDynamicInstanceData(bool bDynamic) { bDynamicInstanceData = bDynamic; }
			inline void UseDynamicInstanceCount(bool bDynamic) { bDynamicInstanceCount = bDynamic; }

			inline void SetMaterialHash(u64 hash) { m_matHash = hash; }
			inline void SetPreRenderer(std::function<void(class CMaterial*)> func) { m_onPreRender = func; }
			
			inline void SetActive(bool bActive) { m_bActive = bActive; }

		protected:
			// Accessors.
			inline u64 GetThis() const { return m_this; }

			inline bool IsUsingDynamicInstanceData() const { return bDynamicInstanceData; }
			inline bool IsUsingDynamicInstanceCount() const { return bDynamicInstanceCount; }

			inline class CMaterial* GetMaterial() const { return m_pMaterial; }

		private:
			bool m_bActive = true;

			bool bDynamicInstanceData = false;
			bool bDynamicInstanceCount = false;

			u32 m_viewHash;
			u64 m_this;
			u64 m_matHash;
			
			std::function<void(class CMaterial*)> m_onPreRender;

			class CMaterial* m_pMaterial = nullptr;
		};

	public:
		CMeshRenderer(u32 hash);
		~CMeshRenderer();
		CMeshRenderer(const CMeshRenderer&) = delete;
		CMeshRenderer(CMeshRenderer&&) = delete;
		CMeshRenderer& operator = (const CMeshRenderer&) = delete;
		CMeshRenderer& operator = (CMeshRenderer&&) = delete;

		static CMeshRenderer& Get();
		virtual void Register() = 0;
	};
};

#endif
