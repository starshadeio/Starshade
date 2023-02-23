//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshCube.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMESHCUBE_H
#define CMESHCUBE_H

#include "CMesh.h"
#include <Math/CMathVector3.h>

namespace Graphics
{
	class CMeshCube
	{
	public:
		struct Data
		{
			bool bTextured = false;
			Math::Vector3 halfSize = 0.5f;
		};

	public:
		CMeshCube();
		~CMeshCube();
		CMeshCube(const CMeshCube&) = delete;
		CMeshCube(CMeshCube&&) = delete;
		CMeshCube& operator = (const CMeshCube&) = delete;
		CMeshCube& operator = (CMeshCube&&) = delete;

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
