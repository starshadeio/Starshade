//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIQuad.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUIQUAD_H
#define CUIQUAD_H

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
	class CTexture;
	class CGraphicsAPI;
};

namespace UI
{
	class CUIQuad : public CVComponent
	{
	public:
		struct Vertex
		{
			Math::Vector2 position;
			Math::Vector2 texCoord;
		};
			
		struct Instance
		{
			Math::Vector4 texCoord;
			Math::Color color;
			Math::Matrix3x2 world;
		};

		typedef u16 Index;

	public:
		struct Data
		{
			u64 matHash;
			u64 texHash;
		};

	public:
		CUIQuad(const CVObject* pObject);
		~CUIQuad();
		CUIQuad(const CUIQuad&) = delete;
		CUIQuad(CUIQuad&&) = delete;
		CUIQuad& operator = (const CUIQuad&) = delete;
		CUIQuad& operator = (CUIQuad&&) = delete;

		void Initialize() final;
		void Render();
		void Release() final;
		
		void RegisterRenderer(class CUIRenderer* pRenderer);
		void RegisterText(class CUIText* pText);
		void AddDirty(class CUIRenderer* pRenderer);
		Instance* PullInstance();

		// Accessors.
		inline size_t GetRendererCount() const { return m_rendererList.size(); }
		inline const class CUIRenderer* GetRendererAt(size_t index) const { return m_rendererList[index]; }

		inline const Graphics::CTexture* GetTexture() const { return m_pTexture; }
		
		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void ForceReset() { m_bForceReset = true; }

	private:
		void PreRender();

	private:
		bool m_bForceReset;
		u32 m_instanceMax;

		Data m_data;
		
		std::vector<class CUIRenderer*> m_rendererList;
		std::deque<class CUIRenderer*> m_dirtyQueue;

		Graphics::CMeshData_ m_meshData;
		Graphics::CMeshRenderer_* m_pMeshRenderer;
		Graphics::CMaterial* m_pMaterial;
		Graphics::CTexture* m_pTexture;

		Graphics::CGraphicsAPI* m_pGraphicsAPI;
	};
};

#endif
