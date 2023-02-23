//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Graphics/CMeshSpawnPoint.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMeshSpawnPoint.h"

namespace Graphics
{
	CMeshSpawnPoint::CMeshSpawnPoint()
	{
	}

	CMeshSpawnPoint::~CMeshSpawnPoint()
	{
	}

	void CMeshSpawnPoint::Initialize()
	{
		struct Vertex
		{
			Math::Vector3 position;
			Math::Vector3 normal;
		};

		typedef u16 Index;

		const u32 baseVertexCount = m_data.baseIterations * 8;

		{ // Setup the mesh.
			CMesh::Data data { };
			data.vertexCount = baseVertexCount + 18;
			data.vertexStride = sizeof(Vertex);
			data.indexCount = m_data.baseIterations * 24 + 24;
			data.indexStride = sizeof(Index);
			data.topology = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.pData = nullptr;
			m_mesh.SetData(data);
			m_mesh.Initialize();
		}

		u32 vertex = 0;
		u32 index = 0;

		// Build base.
		const float stepSize = Math::g_2Pi / m_data.baseIterations;
		float step = 0.0f;
		for(u32 i = 0; i < m_data.baseIterations; ++i)
		{
			const float x = cosf(step);
			const float z = sinf(step);

			const Index vertAdj = (vertex + 8) % baseVertexCount;


			for(u32 j = 0; j < 8; j += 2)
			{
				*(Index*)m_mesh.GetIndexAt(index++) = vertex  + j;
				*(Index*)m_mesh.GetIndexAt(index++) = vertex  + j + 1;
				*(Index*)m_mesh.GetIndexAt(index++) = vertAdj + j + 1;
				*(Index*)m_mesh.GetIndexAt(index++) = vertex  + j;
				*(Index*)m_mesh.GetIndexAt(index++) = vertAdj + j + 1;
				*(Index*)m_mesh.GetIndexAt(index++) = vertAdj + j;
			}

			const Math::Vector3 v0 = Math::Vector3(x, 0, z) * m_data.baseRadiusInner;
			const Math::Vector3 v1 = Math::Vector3(x, 0, z) * (m_data.baseRadiusInner + m_data.baseBevel) + Math::VEC3_UP * m_data.baseHeight;
			const Math::Vector3 v2 = Math::Vector3(x, 0, z) * (m_data.baseRadiusOuter - m_data.baseBevel) + Math::VEC3_UP * m_data.baseHeight;
			const Math::Vector3 v3 = Math::Vector3(x, 0, z) * m_data.baseRadiusOuter;

			const Math::Vector3 e0 = v1 - v0;
			const Math::Vector3 e1 = v2 - v3;

			const Math::Vector3 n0 = Math::Vector3::Cross(e0, Math::Vector3::Cross(Math::VEC3_UP, e0)).Normalized();
			const Math::Vector3 n1 = Math::Vector3::Cross(e1, Math::Vector3::Cross(Math::VEC3_UP, e1)).Normalized();

			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v0, n0 };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v1, n0 };

			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v1, Math::VEC3_UP };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v2, Math::VEC3_UP };
			
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v3, Math::VEC3_DOWN };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v0, Math::VEC3_DOWN };

			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v2, n1 };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v3, n1 };

			step += stepSize;
		}

		{ // Build dial.
			const Math::Vector3 v0 = Math::Vector3(-m_data.dialHalfWidth, 0, -m_data.dialHalfLength);
			const Math::Vector3 v1 = Math::Vector3(-m_data.dialHalfWidth, 0,  m_data.dialHalfLength);
			const Math::Vector3 v2 = Math::Vector3(-m_data.dialHalfWidth, m_data.dialHeight,  m_data.dialHalfLength);

			const Math::Vector3 v3 = Math::Vector3( m_data.dialHalfWidth, 0, -m_data.dialHalfLength);
			const Math::Vector3 v4 = Math::Vector3( m_data.dialHalfWidth, 0,  m_data.dialHalfLength);
			const Math::Vector3 v5 = Math::Vector3( m_data.dialHalfWidth, m_data.dialHeight,  m_data.dialHalfLength);

			// Left.
			*(Index*)m_mesh.GetIndexAt(index++) = vertex;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 2;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 1;

			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v0, Math::VEC3_LEFT };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v1, Math::VEC3_LEFT };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v2, Math::VEC3_LEFT };

			// Right.
			*(Index*)m_mesh.GetIndexAt(index++) = vertex;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 1;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 2;

			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v3, Math::VEC3_RIGHT };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v4, Math::VEC3_RIGHT };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v5, Math::VEC3_RIGHT };

			// Front.
			*(Index*)m_mesh.GetIndexAt(index++) = vertex;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 2;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 1;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 3;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 2;

			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v4, Math::VEC3_FORWARD };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v5, Math::VEC3_FORWARD };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v2, Math::VEC3_FORWARD };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v1, Math::VEC3_FORWARD };

			// Bottom.
			*(Index*)m_mesh.GetIndexAt(index++) = vertex;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 1;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 3;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 1;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 2;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 3;

			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v3, Math::VEC3_DOWN };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v0, Math::VEC3_DOWN };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v1, Math::VEC3_DOWN };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v4, Math::VEC3_DOWN };

			// Top
			Math::Vector3 e0 = v2 - v0;
			Math::Vector3 n0 = Math::Vector3::Cross(e0, Math::VEC3_RIGHT).Normalized();
			
			*(Index*)m_mesh.GetIndexAt(index++) = vertex;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 1;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 2;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 2;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 3;
			
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v0, n0 };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v3, n0 };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v5, n0 };
			*(Vertex*)m_mesh.GetVertexAt(vertex++) = { v2, n0 };
		}
	}

	void CMeshSpawnPoint::Release()
	{
		m_mesh.Release();
	}
};
