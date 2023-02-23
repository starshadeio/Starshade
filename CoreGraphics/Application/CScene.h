//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CScene.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSCENE_H
#define CSCENE_H

#include <Math/CMathRect.h>
#include <Globals/CGlobals.h>
#include <fstream>

namespace App
{
	enum class SceneType
	{
		Default,
		Global,
		Unloaded,
	};

	class CScene
	{
	protected:
		friend class CSceneManager;
		friend class CSceneFileIO;

	protected:
		CScene(const wchar_t* pName, SceneType sceneType);
		virtual ~CScene();
		CScene(const CScene&) = delete;
		CScene(CScene&&) = delete;
		CScene& operator = (const CScene&) = delete;
		CScene& operator = (CScene&&) = delete;

	protected:
		virtual void Initialize() { }
		virtual void PostInitialize() { }
		virtual void Load() { }
		virtual void Unload() { }
		virtual void SaveToFile(std::ofstream& file) const { }
		virtual void LoadFromFile(std::ifstream& file) { }

		virtual void Update() { }
		virtual void LateUpdate() { }
		virtual bool OnQuit(int exitCode) { return true; }
		virtual void Release() { }

		virtual void OnMove(const Math::Rect& rect) { }
		virtual void OnResize(const Math::Rect& rect) { }

		// Accessors.
		inline const wchar_t* GetName() const { return m_pName; }
		inline u32 GetHash() const { return m_hash; }

	private:
		const wchar_t* m_pName;
		const u32 m_hash;
	};
};

#endif
