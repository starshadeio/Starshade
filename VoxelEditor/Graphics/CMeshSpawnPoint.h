//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Graphics/CMeshSpawnPoint.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMESHSPAWNPOINT_H
#define CMESHSPAWNPOINT_H

#include <Graphics/CMesh.h>

namespace Graphics
{
	class CMeshSpawnPoint
	{
	public:
		struct Data
		{
			float baseRadiusInner = 0.375f;
			float baseRadiusOuter = 0.5f;
			float baseHeight = 0.1f;
			float baseBevel = 0.025f;
			u32 baseIterations = 24;

			float dialHalfWidth = 0.05f;
			float dialHalfLength = 0.3f;
			float dialHeight = 0.4f;
		};
		
	public:
		CMeshSpawnPoint();
		~CMeshSpawnPoint();
		CMeshSpawnPoint(const CMeshSpawnPoint&) = delete;
		CMeshSpawnPoint(CMeshSpawnPoint&&) = delete;
		CMeshSpawnPoint& operator = (const CMeshSpawnPoint&) = delete;
		CMeshSpawnPoint& operator = (CMeshSpawnPoint&&) = delete;

		void Initialize();
		void Release();

		// Accessors.
		inline const CMesh* GetMesh() const { return &m_mesh; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		Data m_data;

		CMesh m_mesh;
	};
};

#endif
