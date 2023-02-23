//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CChunkEditor.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCHUNKEDITOR_H
#define CCHUNKEDITOR_H

#include <Math/CMathFNV.h>
#include <Globals/CGlobals.h>
#include <fstream>

namespace App
{
	class CNodeEx;
};

namespace Universe
{
	class CChunkEditor
	{
	public:
		static const u64 CMD_KEY_CHUNK_RESET = Math::FNV1a_64("chunk_reset");
		static const u64 CMD_KEY_CHUNK_CLEAR = Math::FNV1a_64("chunk_clear");

	public:
		CChunkEditor();
		~CChunkEditor();
		CChunkEditor(const CChunkEditor&) = delete;
		CChunkEditor(CChunkEditor&&) = delete;
		CChunkEditor& operator = (const CChunkEditor&) = delete;
		CChunkEditor& operator = (CChunkEditor&&) = delete;

		void Initialize();
		void LateUpdate();
		void Release();

		// Modifiers.
		inline void SetNode(App::CNodeEx* pNode) { m_pNode = pNode; }
		
	private:
		bool ChunkReset(const void* param);
		bool ChunkClear(const void* param);

	private:
		App::CNodeEx* m_pNode;
	};
};

#endif
