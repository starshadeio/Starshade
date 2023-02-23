//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CGraphicsAPI.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGRAPHICSAPI_H
#define CGRAPHICSAPI_H

#include "CGraphicsData.h"
#include "CGraphicsData.h"
#include <Math/CMathRect.h>
#include <vector>
#include <functional>

namespace App {
	class CPanel;
};

namespace Graphics {
	class CGraphicsAPI {
	public:
		struct RenderTargetData {
			u32 targetCount;
			void* pRtvHandle[8];
			void* pDsvHandle;
			Viewport viewport;
			Math::RectLTRB scissorRect;
		};

	public:
		struct Data {
			u8 bufferCount;
			u8 vBlanks;
			ClearValue clearValue;
			std::function<void()> onInit;
			std::function<void()> onCompute;
			std::function<void()> onRender;
			App::CPanel* pPanel;
		};

	protected:
		CGraphicsAPI();
		CGraphicsAPI(const CGraphicsAPI&) = delete;
		CGraphicsAPI(CGraphicsAPI&&) = delete;
		CGraphicsAPI& operator = (const CGraphicsAPI&) = delete;
		CGraphicsAPI& operator = (CGraphicsAPI&&) = delete;

	public:
		virtual ~CGraphicsAPI();

		virtual void Initialize() = 0;
		virtual void Render() = 0;
		virtual void Sync() = 0;
		virtual void Release() = 0;
		
		virtual void ForceClear(CLEAR_MODE clearMode, const Math::Color* clearColorList, const DepthStencil& clearDepthStencil, u32 numRects, const Math::RectLTRB* pRects) { }
		virtual void SetRenderTargets(const RenderTargetData& rtData) { }
		virtual void ResetRenderTargets() { }
		
		virtual void RenderEmptyBuffer(PRIMITIVE_TOPOLOGY topology, u32 vertexCount, u32 instanceCount) { }

		virtual void AdjustToResize() = 0;
		virtual void FinalizeResize() = 0;

		bool Screenshot(const wchar_t* filename);

		// Accessors.
		inline const Data& GetData() const { return m_data; }

		inline u8 GetBufferCount() const { return m_data.bufferCount; }
		virtual inline u32 GetFrameIndex() const = 0;
		virtual inline u64 GetFrame() const = 0;
		virtual inline float GetAspectRatio() const = 0;
		virtual inline float GetWidth() const = 0;
		virtual inline float GetHeight() const = 0;

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	protected:
		virtual bool Screenshot(const wchar_t* filename, IMAGE_FILETYPE type) = 0;

	protected:
		Data m_data;
		
		std::vector<AdapterData> m_adapterDataList;
	};
};

#endif
