//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRenderingSystem.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CRENDERINGSYSTEM_H
#define CRENDERINGSYSTEM_H

#include <Objects/CVObject.h>
#include <Logic/CTransform.h>
#include <unordered_map>
#include <vector>
#include <queue>
#include <functional>

namespace UI
{
	class CUICanvas;
};

namespace Graphics
{
	class CRenderingSystem
	{
	private:
		struct RenderData
		{
			class CPostProcessor* pPostProcessor;
			u32 viewHash;
		};

		struct RendererData_
		{
			size_t materialIndex;
			class CRenderer_* pRenderer;

			inline class CRenderer_* operator -> () { return pRenderer; }
			inline const class CRenderer_* operator -> () const { return pRenderer; }

			inline bool operator == (const RendererData_& b) const { return materialIndex == b.materialIndex && pRenderer == b.pRenderer; }
			inline bool operator != (const RendererData_& b) const { return materialIndex != b.materialIndex && pRenderer != b.pRenderer; }
		};

	public:
		CRenderingSystem();
		~CRenderingSystem();
		CRenderingSystem(const CRenderingSystem&) = delete;
		CRenderingSystem(CRenderingSystem&&) = delete;
		CRenderingSystem& operator = (const CRenderingSystem&) = delete;
		CRenderingSystem& operator = (CRenderingSystem&&) = delete;

		void PostInitialize();

		void QueueRender(class CPostProcessor* pPostProcessor, u32 viewHash);
		void RenderAll();

		// (De)registration methods.
		void Register(class CRenderer* pRenderer);
		void Deregister(class CRenderer* pRenderer);
		
		void Register(class CRenderer_* pRenderer, size_t materialIndex = 0);
		void Deregister(class CRenderer_* pRenderer);

		void Register(UI::CUICanvas* pCanvas);
		void Deregister(UI::CUICanvas* pCanvas);

		inline void AddOverlayRenderer(u32 viewHash, std::function<void()> func) { m_overlayList[viewHash].push_back(func); }

	private:
		void RenderWorld(u32 viewHash);
		void RenderUI(u32 viewHash);

	private:
		template<typename T>
		void Register(T* pVal, std::vector<T*>& list, std::function<bool(T*, T*)> comp)
		{
			list.push_back(pVal);

			if(comp)
			{
				// Bubble the newly pushed mesh render to the proper position.
				for(s64 i = static_cast<s64>(list.size()) - 1; i > 0; --i)
				{
					if(comp(list[i - 1], list[i]))
					{
						// Swap the two adjacent values.
						T* tmp = list[i - 1];
						list[i - 1] = list[i];
						list[i] = tmp;
					}
					else
					{ // Pushed value at proper spot.
						break;
					}
				}
			}
		}

		template<typename T>
		void Register(const T& val, std::vector<T>& list, std::function<bool(const T&, const T&)> comp)
		{
			list.push_back(val);

			if(comp)
			{
				// Bubble the newly pushed mesh render to the proper position.
				for(s64 i = static_cast<s64>(list.size()) - 1; i > 0; --i)
				{
					if(comp(list[i - 1], list[i]))
					{
						// Swap the two adjacent values.
						T tmp = list[i - 1];
						list[i - 1] = list[i];
						list[i] = tmp;
					}
					else
					{ // Pushed value at proper spot.
						break;
					}
				}
			}
		}

		template<typename T>
		void Deregister(const T& val, std::vector<T>& list)
		{
			size_t i, j;
			for(i = 0, j = 0; i < list.size(); ++i)
			{
				if(list[i] != val)
				{
					list[j++] = list[i];
				}
			}

			list.resize(j);
		}

	private:
		template<typename T>
		void SortBackToFront(const Math::SIMDVector& camPos, std::vector<T>& list)
		{
			std::sort(list.begin(), list.end(), [&camPos](const T& a, const T& b){
				const float d0 = a->GetObject()->GetTransform() ? _mm_cvtss_f32((a->GetObject()->GetTransform()->GetPosition() - camPos).LengthSq()) : FLT_MAX;
				const float d1 = b->GetObject()->GetTransform() ? _mm_cvtss_f32((b->GetObject()->GetTransform()->GetPosition() - camPos).LengthSq()) : FLT_MAX;

				return d0 > d1;
			});
		}

	private:
		std::queue<RenderData> m_renderQueue;

		std::unordered_map<u32, std::vector<RendererData_>> m_depthlessList;
		std::unordered_map<u32, std::vector<RendererData_>> m_rendererList_;
		std::unordered_map<u32, std::vector<std::function<void()>>> m_overlayList;
		std::unordered_map<u32, std::vector<UI::CUICanvas*>> m_canvasList;
		
		std::vector<class CRenderer*> m_rendererList;
	};
};

#endif
