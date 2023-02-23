//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Application/CCompManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCOREMANAGER_H
#define CCOREMANAGER_H

#include "../Logic/CTransform.h"
#include "../Math/CMathRect.h"
#include "../Objects/CNodeRegistry.h"
#include <fstream>

namespace App
{
	class CCoreManager
	{
	public:
		static CCoreManager& Instance()
		{
			static CCoreManager instance;
			return instance;
		}

	private:
		CCoreManager();
		~CCoreManager();
		CCoreManager(const CCoreManager&) = delete;
		CCoreManager(CCoreManager&&) = delete;
		CCoreManager& operator = (const CCoreManager&) = delete;
		CCoreManager& operator = (CCoreManager&&) = delete;
		
	public:
		void Initialize();
		void PostInitialize();
		void Update();
		void LateUpdate();
		void Release();

		void OnMove(const Math::Rect& rect);
		void OnResize(const Math::Rect& rect);
	
		void SaveToFile(std::ofstream& file) const;
		void LoadFromFile(std::ifstream& file);

		inline void QueueTransformUpdate(Logic::CTransform* pTransform) { m_transformQueue.Push(pTransform, pTransform->GetLayer()); }

		// Accessors.
		inline CNodeRegistry& NodeRegistry() { return m_nodeRegistry; }

	private:
		Logic::CTransformQueue m_transformQueue;
		CNodeRegistry m_nodeRegistry;
	};
};

#endif
