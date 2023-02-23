//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshData_.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMESHDATA__H
#define CMESHDATA__H

#include "CGraphicsData.h"
#include <Objects/CVComponent.h>
#include <Globals/CGlobals.h>
#include <functional>

namespace Graphics
{
	// This is legacy, and will eventually be removed.
	class CMeshData_ : public CVComponent
	{
	public:
		struct Data
		{
			PRIMITIVE_TOPOLOGY topology;

			u32 vertexCount;
			u32 instanceMax;
			u32 indexCount;

			u32 vertexStride;
			u32 instanceStride;
			u32 indexStride;
		};

	public:
		CMeshData_(const class CVObject* pObject);
		~CMeshData_();
		CMeshData_(const CMeshData_&) = default;
		CMeshData_(CMeshData_&&) = default;
		CMeshData_& operator = (const CMeshData_&) = default;
		CMeshData_& operator = (CMeshData_&&) = default;

		void Initialize() final;
		void Release() final;

		void ProcessVertexList(std::function<void(u32, u8*)> processor);
		void ProcessIndexList(std::function<void(u32, u8*)> processor);

		// Accessors.
		inline PRIMITIVE_TOPOLOGY GetTopology() const { return m_data.topology; }

		inline u32 GetVertexCount() const { return m_data.vertexCount; }
		inline u32 GetVertexStride() const { return m_data.vertexStride; }
		inline u32 GetVertexSize() const { return m_vertexSize; }
		
		inline u32 GetInstanceMax() const { return m_data.instanceMax; }
		inline u32 GetInstanceCount() const { return m_instanceCount; }
		inline u32 GetInstanceStride() const { return m_data.instanceStride; }
		inline u32 GetInstanceSizeMax() const { return m_instanceSize; }
		inline u32 GetInstanceSize() const { return m_instanceCount * m_data.instanceStride; }

		inline u32 GetIndexCount() const { return m_data.indexCount; }
		inline u32 GetIndexStride() const { return m_data.indexStride; }
		inline u32 GetIndexSize() const { return m_indexSize; }
		
		inline const u8* GetVertexList() const { return m_pVertexList; }
		inline const u8* GetInstanceList() const { return m_pInstanceList; }
		inline const u8* GetIndexList() const { return m_pIndexList; }

		inline u8* GetVertexAt(u32 offset) { return m_pVertexList + offset * m_data.vertexStride; }
		
		inline void ForceInstancesDirty() { m_bInstanceDirty = true; }
		inline bool ResetInstanceDirty() { const bool bDirty = m_bInstanceDirty; m_bInstanceDirty = false; return bDirty; }
		inline void ResetInstances() { m_instanceCount = 0; }
		inline void SetInstanceCount(u32 count) { m_instanceCount = count; }
		inline u8* GetInstanceNext() { m_bInstanceDirty = true; return m_pInstanceList + m_instanceCount++ * m_data.instanceStride; }
		inline u8* GetInstanceAt(u32 offset) { m_bInstanceDirty = true; return m_pInstanceList + offset * m_data.instanceStride; }
		inline void ReplaceInstanceList(u8* pList) { m_pInstanceList = pList; } // Important for dynamic instance buffers.

		inline u8* GetIndexAt(u32 offset) { return m_pIndexList + offset * m_data.indexStride; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		Data m_data;

		bool m_bInstanceDirty;

		u32 m_vertexSize;
		u32 m_instanceSize;
		u32 m_instanceCount;
		u32 m_indexSize;

		u8* m_pVertexList;
		u8* m_pInstanceList;
		u8* m_pIndexList;
		u8* m_pBuffer;
	};
};

#endif
