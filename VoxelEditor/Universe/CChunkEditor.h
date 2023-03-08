//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CChunkEditor.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCHUNKEDITOR_H
#define CCHUNKEDITOR_H

#include "CChunkWorkspace.h"
#include <Universe/CChunkData.h>
#include <Math/CMathFNV.h>
#include <Globals/CGlobals.h>
#include <fstream>
#include <vector>

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

		static const u64 CMD_KEY_CUT = Math::FNV1a_64("cut");
		static const u64 CMD_KEY_COPY = Math::FNV1a_64("copy");
		static const u64 CMD_KEY_PASTE = Math::FNV1a_64("paste");
		static const u64 CMD_KEY_DELETE = Math::FNV1a_64("delete");
		static const u64 CMD_KEY_FILL = Math::FNV1a_64("fill");
		static const u64 CMD_KEY_PAINT = Math::FNV1a_64("paint");

		static const u64 CMD_KEY_LOD_DOWN = Math::FNV1a_64("lod_down");
		static const u64 CMD_KEY_LOD_UP = Math::FNV1a_64("lod_up");

	private:
		static const u64 CMD_KEY_CHUNK_CUT = Math::FNV1a_64("chunk_cut");
		static const u64 CMD_KEY_CHUNK_COPY = Math::FNV1a_64("chunk_copy");
		static const u64 CMD_KEY_CHUNK_PASTE = Math::FNV1a_64("chunk_paste");
		static const u64 CMD_KEY_CHUNK_DELETE = Math::FNV1a_64("chunk_delete");
		static const u64 CMD_KEY_CHUNK_FILL = Math::FNV1a_64("chunk_fill");
		static const u64 CMD_KEY_CHUNK_PAINT = Math::FNV1a_64("chunk_paint");

		struct OpExtents
		{
			Math::Vector3 mn;
			Math::Vector3 mx;
		};

		struct OpExtentsFill : OpExtents
		{
			u32 id;
		};
		
		union BlockFloat {
			Block b;
			float f;

			BlockFloat(Block b) : b(b) { }
			BlockFloat(float f) : f(f) { }
		};

	public:
		CChunkEditor();
		~CChunkEditor();
		CChunkEditor(const CChunkEditor&) = delete;
		CChunkEditor(CChunkEditor&&) = delete;
		CChunkEditor& operator = (const CChunkEditor&) = delete;
		CChunkEditor& operator = (CChunkEditor&&) = delete;

		void Initialize();
		void PostInitialize();
		void Update();
		void LateUpdate();
		void Release();

		void Save() const;
		void Load();

		void OnPlay();
		void OnEdit();

		// Accessors.
		inline const CChunkWorkspace& GetWorkspace() const { return m_workspace; }
		inline App::CNodeEx* GetNode() const { return m_pNode; }

		// Modifiers.
		void SetNode(App::CNodeEx* pNode);
		
	private:
		bool ChunkReset();
		bool ChunkClear();

		bool Cut();
		bool Copy();
		bool Paste();
		bool Delete();
		bool Fill();
		bool Paint();

		bool LODDown();
		bool LODUp();

		bool ChunkCut(const void* param, bool bInverse);
		bool ChunkCopy(const void* param);
		bool ChunkPaste(const void* param, bool bInverse);
		bool ChunkDelete(const void* param, bool bInverse);
		bool ChunkFill(const void* param, bool bInverse);
		bool ChunkPaint(const void* param, bool bInverse);
		
		void OnCmdWorkspaceOff();
		void OnCmdWorkspaceOn();

	private:
		CChunkWorkspace m_workspace;

		std::vector<BlockFloat> m_clipboard;
		App::CNodeEx* m_pNode;
	};
};

#endif
