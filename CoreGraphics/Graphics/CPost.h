//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPost.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPOST_H
#define CPOST_H

#include <vector>

namespace App
{
	class CPanel;
};

namespace Graphics
{
	class CPost
	{
	public:
		CPost();
		~CPost();
		CPost(const CPost&) = delete;
		CPost(CPost&&) = delete;
		CPost& operator = (const CPost&) = delete;
		CPost& operator = (CPost&&) = delete;

		void PostInitialize();
		void RenderQuad();

		// Modifier methods.
		inline void AddPostProcessor(class CPostProcessor* pPostProcessor) { m_postProcessorList[m_postProcessorCount++] = pPostProcessor; }

		inline void SetPostProcessorCount(size_t count) { m_postProcessorCount = count; }
		inline void SetPostProcessor(size_t index, class CPostProcessor* pPostProcessor) { m_postProcessorList[index] = pPostProcessor; }

	private:
		size_t m_postProcessorCount;
		class CPostProcessor* m_postProcessorList[8];

		class CMaterial* m_pPostMat;
		class CGraphicsAPI* m_pGraphicsAPI;
	};
};

#endif
