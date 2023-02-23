//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPostProcessor.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPOSTPROCESSOR_H
#define CPOSTPROCESSOR_H

#include "CRenderTexture.h"
#include <vector>
#include <string>

namespace Graphics
{
	class CPostProcessor
	{
	public:
		CPostProcessor(const wchar_t* pName);
		~CPostProcessor();
		CPostProcessor(const CPostProcessor&) = delete;
		CPostProcessor(CPostProcessor&&) = delete;
		CPostProcessor& operator = (const CPostProcessor&) = delete;
		CPostProcessor& operator = (CPostProcessor&&) = delete;

		void Initialize();
		void PostInitialize();
		void BeginRenderTo(CLEAR_MODE clearMode, u32 numRects, const Math::RectLTRB* pRects);
		void EndRenderTo();
		void RenderQuad();
		void OnResize(const Math::Rect& rect);
		void Release();

		void RegisterPostEffect(class CPostEffect* pPostEffect);

		// Accessors.
		inline const std::wstring& GetName() const { return m_name; }
		inline const CRenderTexture& GetRenderTexture() const { return m_renderTexture; }

		inline size_t GetPostEffectCount() const { return m_postEffectList.size(); }
		inline const class CPostEffect* GetPostEffect(size_t index) const { return m_postEffectList[index]; }

		// Modifiers.
		inline void SetData(const CRenderTexture::Data& data) { m_renderTextureData = data; }

	private:
		void PostProcess();

	private:
		const std::wstring m_name;

		CRenderTexture m_renderTexture;
		CRenderTexture::Data m_renderTextureData;

		std::vector<class CPostEffect*> m_postEffectList;
		
		class CMaterial* m_pPostMat;
		class CGraphicsAPI* m_pGraphicsAPI;
	};
};

#endif
