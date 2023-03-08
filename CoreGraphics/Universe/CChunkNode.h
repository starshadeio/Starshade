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
#include <vector>
#include <shared_mutex>

namespace Universe
{
	class CChunkNode : public CVObject
	{
	public:
		struct Data
		{
			float blockSize = 1.0f;
			float physicsExtents = 16.0f;
			float localExtents = 32.0f;
			u32 chunkWidth = 32;
			u32 chunkHeight = 32;
			u32 chunkLength = 32;

			class CChunkGen* pChunkGen = nullptr;
		};

	public:
		CChunkNode(const wchar_t* pName, u32 viewHash);
		~CChunkNode();
		CChunkNode(const CChunkNode&) = delete;
		CChunkNode(CChunkNode&&) = delete;
		CChunkNode& operator = (const CChunkNode&) = delete;
		CChunkNode& operator = (CChunkNode&&) = delete;
		
		void Initialize() final;
		void LateUpdate() final;
		void Release() final;

		void Save(const std::wstring& path) const;
		void Save(std::ofstream& file) const;
		void Load(const std::wstring& path, u32 version = ~0U);
		void Load(std::ifstream& file, u32 version = ~0U);
		
		void Reset();
		void Clear();

		std::pair<Math::VectorInt3, u32> GetIndex(const Math::Vector3& coord) const;
		Block GetBlock(const Math::Vector3& coord, std::pair<Math::VectorInt3, u32>* pIndices = nullptr) const;
		Math::Vector3 GetPosition(const std::pair<Math::VectorInt3, u32>& indices) const;
		
		Block ReadBlock(const std::pair<Math::VectorInt3, u32>& indices) const;
		void WriteBlock(Block block, const std::pair<Math::VectorInt3, u32>& indices);
		
		void ReadBlocksInExtents(const Math::Vector3& mn, const Math::Vector3& mx, std::function<void(u32, Block, const std::pair<Math::VectorInt3, u32>&)> func) const;
		void WriteBlocksInExtents(const Math::Vector3& mn, const Math::Vector3& mx, std::function<bool(u32, const std::pair<Math::VectorInt3, u32>&, Block&)> func);
		
		void ReadWriteBlocksInExtents(const Math::Vector3& mn, const Math::Vector3& mx, std::function<bool(u32, const std::pair<Math::VectorInt3, u32>&, Block&)> funcWrite, 
			std::function<void(u32, Block, const std::pair<Math::VectorInt3, u32>&)> funcRead);
		void ReadPaintBlocksInExtents(const Math::Vector3& mn, const Math::Vector3& mx, std::function<bool(u32, const std::pair<Math::VectorInt3, u32>&, Block&)> funcWrite, 
			std::function<void(u32, Block, const std::pair<Math::VectorInt3, u32>&)> funcRead);

	private:
		void ProcessBlocksInExtents(const Math::Vector3& mn, const Math::Vector3& mx, std::function<void(u32, const std::pair<Math::VectorInt3, u32>&, class CChunk*)> func);
		void ProcessBlocksInExtentsRO(const Math::Vector3& mn, const Math::Vector3& mx, std::function<void(u32, const std::pair<Math::VectorInt3, u32>&, class CChunk*)> func, 
			std::function<class CChunk*(const Math::VectorInt3&)> onChunkNotFound = nullptr) const;
		
		void FrustumCulling(const Math::Vector3& mn, const Math::Vector3& mx) const;
		void SetChunksInExtentsToRender(const Math::VectorInt3& mn, const Math::VectorInt3& mx) const;

	public:
		void FrustumCulling() const;
		class CChunk* CreateChunk(const Math::VectorInt3& coords);

		// Accessors.
		inline void GetExtentsPhysics(Math::Vector3& minExtents, Math::Vector3& maxExtents) const
		{
			std::shared_lock<std::shared_mutex> lk(m_extentMutex);
			minExtents = m_minExtentsPhysics;
			maxExtents = m_maxExtentsPhysics;
		}

		inline void GetExtentsWorkspace(Math::Vector3& minExtentsRay, Math::Vector3& maxExtentsRay) const
		{
			std::shared_lock<std::shared_mutex> lk(m_extentMutex);
			minExtentsRay = m_minExtentsWorkspace;
			maxExtentsRay = m_maxExtentsWorkspace;
		}

		inline void GetExtentsLocal(Math::Vector3& minExtentsRay, Math::Vector3& maxExtentsRay) const
		{
			std::shared_lock<std::shared_mutex> lk(m_extentMutex);
			minExtentsRay = m_minExtentsLocal;
			maxExtentsRay = m_maxExtentsLocal;
		}

		inline void GetExtentsWorking(Math::Vector3& minExtents, Math::Vector3& maxExtents) const
		{
			if(m_bWorkspaceActive)
			{
				GetExtentsWorkspace(minExtents, maxExtents);
			}
			else
			{
				GetExtentsLocal(minExtents, maxExtents);
			}
		}
		
		inline u32 GetBlockCountX() const { return m_data.chunkWidth; }
		inline u32 GetBlockCountY() const { return m_data.chunkHeight; }
		inline u32 GetBlockCountZ() const { return m_data.chunkLength; }

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

		inline void SetExtentsWorkspace(const Math::Vector3& mn, const Math::Vector3& mx)
		{
			std::lock_guard<std::shared_mutex> lk(m_extentMutex);
			m_minExtentsWorkspace = mn;
			m_maxExtentsWorkspace = mx;
		}

		inline void SetWorkspaceActive(bool bActive)
		{
			m_bWorkspaceActive = bActive;
		}

	private:
		bool InteractCallback(void* pVal);
		
		std::pair<Math::VectorInt3, u32> internalGetIndex(const Math::Vector3& coord, class CChunk** ppChunk) const;

	private:
		mutable std::shared_mutex m_extentMutex;

		Abool m_bWorkspaceActive;
		
		Math::Vector3 m_minExtents;
		Math::Vector3 m_maxExtents;
		Math::Vector3 m_minExtentsPhysics;
		Math::Vector3 m_maxExtentsPhysics;
		Math::Vector3 m_minExtentsLocal;
		Math::Vector3 m_maxExtentsLocal;
		Math::Vector3 m_minExtentsWorkspace;
		Math::Vector3 m_maxExtentsWorkspace;
		Math::Vector3 m_halfChunkSize;

		Math::Vector3 m_lastCameraOffset;

		Data m_data;

		Logic::CTransform m_transform;
		Physics::CVolumeChunk m_volume;
		Logic::CCallback m_callback;
	};
};

#endif
