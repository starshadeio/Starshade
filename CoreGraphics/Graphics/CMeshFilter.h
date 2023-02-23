//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshFilter.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMESHDATA_H
#define CMESHDATA_H

#include "CGraphicsData.h"
#include <Objects/CNodeComponent.h>
#include <Math/CMathFNV.h>
#include <unordered_map>

namespace Graphics
{
	class CMeshFilter : public CNodeComponent
	{
	public:
		static const u32 HASH = Math::FNV1a_32(L"Mesh Filter");

	public:
		struct Data
		{
		public:
			friend class CMeshFilter;

		public:
			Data(u64 thisHash);
			~Data();
			Data(const Data&) = default;
			Data(Data&&) = default;
			Data& operator = (const Data&) = default;
			Data& operator = (Data&&) = default;

			// Accessors.
			inline u32 GetInstanceMax() const { return m_instanceMax; }
			inline u32 GetInstanceCount() const { return m_instanceCount; }
			inline u32 GetInstanceStride() const { return m_instanceStride; }
			inline u32 GetInstanceSizeMax() const { return m_instanceSize; }
			inline u32 GetInstanceSize() const { return m_instanceCount * m_instanceStride; }

			inline const u8* GetInstanceList() const { return m_pInstanceList; }

			inline void ForceInstancesDirty() { m_bInstanceDirty = true; }
			inline bool ResetInstanceDirty() { const bool bDirty = m_bInstanceDirty; m_bInstanceDirty = false; return bDirty; }
			inline void ResetInstances() { m_instanceCount = 0; }
			inline void SetInstanceCount(u32 count) { m_instanceCount = count; }
			inline u8* GetInstanceNext() { m_bInstanceDirty = true; return m_pInstanceList + m_instanceCount++ * m_instanceStride; }
			inline u8* GetInstanceAt(u32 offset) { m_bInstanceDirty = true; return m_pInstanceList + offset * m_instanceStride; }
			inline void ReplaceInstanceList(u8* pList) { m_pInstanceList = pList; } // Important for dynamic instance buffers.
			
			inline const class CMesh* GetMesh() const { return m_pMesh; }
			
			// Modifiers.
			inline void SetInstanceMax(u32 instanceMax) { m_instanceMax = instanceMax; }
			inline void SetInstanceStride(u32 instanceStride) { m_instanceStride = instanceStride; }

			inline void SetMesh(const class CMesh* pMesh) { m_pMesh = pMesh; }

		protected:
			void Initialize();
			void Release();

		private:
			bool m_bInstanceDirty;

			u32 m_instanceMax;
			u32 m_instanceStride;
			u32 m_instanceSize;
			u32 m_instanceCount;

			u64 m_this;

			u8* m_pInstanceList;
			const class CMesh* m_pMesh;
		};

	private:
		CMeshFilter();
		~CMeshFilter();
		CMeshFilter(const CMeshFilter&) = delete;
		CMeshFilter(CMeshFilter&&) = delete;
		CMeshFilter& operator = (const CMeshFilter&) = delete;
		CMeshFilter& operator = (CMeshFilter&&) = delete;
		
	public:
		static CMeshFilter& Get() { static CMeshFilter comp; return comp; }
		void Register();

	private:
		void Initialize() final;
		void Release() final;

		void* AddToObject(u64 objHash) final;
		void* GetFromObject(u64 objHash) final;
		bool TryToGetFromObject(u64 objHash, void** ppData) final;
		void RemoveFromObject(u64 objHash) final;

	private:
		std::unordered_map<u64, Data> m_dataMap;
	};
};

#endif
