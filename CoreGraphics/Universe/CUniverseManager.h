//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CUniverseManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUNIVERSEMANAGER_H
#define CUNIVERSEMANAGER_H

#include "CChunkManager.h"

namespace Universe
{
	class CUniverseManager
	{
	public:
		CUniverseManager();
		~CUniverseManager();
		CUniverseManager(const CUniverseManager&) = delete;
		CUniverseManager(CUniverseManager&&) = delete;
		CUniverseManager& operator = (const CUniverseManager&) = delete;
		CUniverseManager& operator = (CUniverseManager&&) = delete;
		
		void Initialize();
		void Update();
		void LateUpdate();
		void Release();

		// Accessors.
		inline CChunkManager& ChunkManager() { return m_chunkManager; }

	private:
		CChunkManager m_chunkManager;
	};
};

#endif
