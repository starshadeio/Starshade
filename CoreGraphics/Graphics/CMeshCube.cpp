//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMeshCube.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMeshCube.h"
#include <Math/CMathVector2.h>

namespace Graphics
{
	CMeshCube::CMeshCube()
	{
	}

	CMeshCube::~CMeshCube()
	{
	}

	void CMeshCube::Initialize()
	{
		struct Vertex
		{
			Math::Vector3 position;
			Math::Vector3 normal;
		};

		struct VertexTex
		{
			Math::Vector3 position;
			Math::Vector3 normal;
			Math::Vector2 texCoord;
		};

		typedef u16 Index;

		{ // Setup the mesh.
			CMesh::Data data { };
			data.vertexCount = 24;
			data.vertexStride = m_data.bTextured ? sizeof(VertexTex) : sizeof(Vertex);
			data.indexCount = 36;
			data.indexStride = sizeof(Index);
			data.topology = PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			data.pData = nullptr;
			m_mesh.SetData(data);
			m_mesh.Initialize();
		}

		u16 vertex = 0;
		u16 index = 0;

		auto AddQuad = [&](const Math::Vector3& offset, const Math::Vector3& normal, const Math::Vector3& forward, const Math::Vector3& right, const Math::Vector3& up){
			*(Index*)m_mesh.GetIndexAt(index++) = vertex;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 1;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 2;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 2;
			*(Index*)m_mesh.GetIndexAt(index++) = vertex + 3;

			if(m_data.bTextured)
			{
				*(VertexTex*)m_mesh.GetVertexAt(vertex++) = { { offset + (forward - right - up) }, normal, { 0.0f, 0.0f } };
				*(VertexTex*)m_mesh.GetVertexAt(vertex++) = { { offset + (forward + right - up) }, normal, { 1.0f, 0.0f } };
				*(VertexTex*)m_mesh.GetVertexAt(vertex++) = { { offset + (forward + right + up) }, normal, { 1.0f, 1.0f } };
				*(VertexTex*)m_mesh.GetVertexAt(vertex++) = { { offset + (forward - right + up) }, normal, { 0.0f, 1.0f } };
			}
			else
			{
				*(Vertex*)m_mesh.GetVertexAt(vertex++) = { { offset + (forward - right - up) }, normal };
				*(Vertex*)m_mesh.GetVertexAt(vertex++) = { { offset + (forward + right - up) }, normal };
				*(Vertex*)m_mesh.GetVertexAt(vertex++) = { { offset + (forward + right + up) }, normal };
				*(Vertex*)m_mesh.GetVertexAt(vertex++) = { { offset + (forward - right + up) }, normal };
			}
		};

		const Math::Vector3 right = Math::VEC3_RIGHT * m_data.halfSize.x;
		const Math::Vector3 up = Math::VEC3_UP * m_data.halfSize.y;
		const Math::Vector3 forward = Math::VEC3_FORWARD * m_data.halfSize.z;

		AddQuad(0.0f, Math::VEC3_RIGHT, right, up, -forward);
		AddQuad(0.0f, Math::VEC3_LEFT, -right, up, forward);

		AddQuad(0.0f, Math::VEC3_DOWN, -up, right, -forward);
		AddQuad(0.0f, Math::VEC3_UP, up, right, forward);

		AddQuad(0.0f, Math::VEC3_FORWARD, forward, up, right);
		AddQuad(0.0f, Math::VEC3_BACKWARD, -forward, up, -right);
	}

	void CMeshCube::Release()
	{
		m_mesh.Release();
	}
};
