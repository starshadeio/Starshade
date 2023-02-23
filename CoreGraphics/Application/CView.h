//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CView.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CVIEW_H
#define CVIEW_H

#include "../Graphics/CPostProcessor.h"
#include <Globals/CGlobals.h>
#include <Math/CMathRect.h>
#include <fstream>

namespace App
{
	class CView
	{
	protected:
		friend class CSceneManager;

	protected:
		CView(const wchar_t* pName);
		virtual ~CView();
		CView(const CView&) = delete;
		CView(CView&&) = delete;
		CView& operator = (const CView&) = delete;
		CView& operator = (CView&&) = delete;
		
	protected:
		virtual void Initialize();
		virtual void PostInitialize();
		virtual bool Load();
		virtual bool Unload();

		virtual void Update() { }
		virtual void LateUpdate() { }
		virtual bool OnQuit(int exitCode) { return true; }
		virtual void Release();

		virtual void SaveToFile(std::ofstream& file) const { }
		virtual void LoadFromFile(std::ifstream& file) { }

		virtual void OnMove(const Math::Rect& rect) { }
		virtual void OnResize(const Math::Rect& rect);
		
	public:
		void QueueRender();
		
		// Accessors.
		inline bool IsLoaded() const { return m_bLoaded; }

		inline const wchar_t* GetName() const { return m_pName; }
		inline u32 GetHash() const { return m_hash; }

		inline const Graphics::CPostProcessor& GetPostProcessor() const { return m_postProcessor; }

		// Modifiers.
		inline void SetPostProcessorData(const Graphics::CRenderTexture::Data& data) { m_postProcessor.SetData(data); }
		inline void RegisterPostEffect(Graphics::CPostEffect* pEffect) { m_postProcessor.RegisterPostEffect(pEffect); }

	private:
		bool m_bLoaded;
		const wchar_t* m_pName;
		const u32 m_hash;

		Graphics::CPostProcessor m_postProcessor;
	};
};

#endif
