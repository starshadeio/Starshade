//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CChunkNode.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCHUNKNODE_H
#define CCHUNKNODE_H

#include "CChunkData.h"
#include "../Physics/CVolumeChunk.h"
#include <Logic/CTransform.h>
#include <Logic/CCallback.h>
#include <Math/CMathVectorInt3.h>
#include <Math/CMathVector3.h>
#include <Globals/CGlobals.h>
#include <Objects/CVObject.h>

namespace Universe
{
	class CChunkNode : public CVObject
	{
	public:
		struct Data
		{
			float blockSize = 1.0f;
			u32 chunkWidth = 32;
			u32 chunkHeight = 32;
			u32 chunkLength = 32;
			u32 chunksX = 1;
			u32 chunksY = 1;
			u32 chunksZ = 1;
		};

	public:
		CChunkNode(const wchar_t* pName, u32 viewHash);
		~CChunkNode();
		CChunkNode(const CChunkNode&) = delete;
		CChunkNode(CChunkNode&&) = delete;
		CChunkNode& operator = (const CChunkNode&) = delete;
		CChunkNode& operator = (CChunkNode&&) = delete;
		
		void Initialize();
		void Release();

		void Save(const std::wstring& path) const;
		void Save(std::ofstream& file) const;
		void Load(const std::wstring& path);
		void Load(std::ifstream& file);
		
		void Reset();
		void Clear();

		std::pair<Math::VectorInt3, u32> GetIndex(const Math::Vector3& coord) const;
		Block GetBlock(const Math::Vector3& coord, std::pair<Math::VectorInt3, u32>* pIndices = nullptr) const;
		Math::Vector3 GetPosition(const std::pair<Math::VectorInt3, u32>& indices) const;

		// Accessors.
		inline const Math::Vector3& GetMinExtents() const { return m_minExtents; }
		inline const Math::Vector3& GetMaxExtents() const { return m_maxExtents; }

		inline float GetChunkWidth() const { return static_cast<float>(m_data.chunkWidth) * m_data.blockSize; }
		inline float GetChunkHeight() const { return static_cast<float>(m_data.chunkHeight) * m_data.blockSize; }
		inline float GetChunkLength() const { return static_cast<float>(m_data.chunkLength) * m_data.blockSize; }
		inline float GetBlockSize() const { return m_data.blockSize; }
		
		inline Math::Vector3 GetChunkSize() const
		{
			return Math::Vector3(
				static_cast<float>(m_data.chunkWidth) * m_data.blockSize,
				static_cast<float>(m_data.chunkHeight) * m_data.blockSize,
				static_cast<float>(m_data.chunkLength) * m_data.blockSize
			);
		}

		// Modifier.
		inline void SetData(const Data& data) { m_data = data; }
		
	private:
		bool InteractCallback(void* pVal);
		
		std::pair<Math::VectorInt3, u32> internalGetIndex(const Math::Vector3& coord, class CChunk** ppChunk) const;

	private:
		Math::Vector3 m_minExtents;
		Math::Vector3 m_maxExtents;
		Math::Vector3 m_halfChunkSize;
		Math::Vector3 m_halfChunkSpan;

		Data m_data;

		Logic::CTransform m_transform;
		Physics::CVolumeChunk m_volume;
		Logic::CCallback m_callback;

		class CChunk* m_pChunkList[4][4][4];
	};
};

#endif
