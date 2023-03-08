//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CChunkGrid.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCHUNKGRID_H
#define CCHUNKGRID_H

#include <Globals/CGlobals.h>
#include <Objects/CVObject.h>
#include <Math/CMathVector3.h>
#include <Graphics/CMeshData_.h>
#include <Logic/CTransform.h>

namespace Graphics
{
	class CMeshRenderer_;
	class CMaterial;
	class CGraphicsAPI;
};

namespace Universe
{
	class CChunkGrid : public CVObject
	{
	private:
		struct InstanceLine
		{
			Math::Color color;
			Math::SIMDMatrix world;
			Math::Vector3 normal;
			float width;
		};

	public:
		struct Data
		{
			Math::Vector3 startingHalfExtents = 4.0f;
			Math::Vector3 startingOffset = 0.0f;
		};

	public:
		CChunkGrid();
		~CChunkGrid();
		CChunkGrid(const CChunkGrid&) = delete;
		CChunkGrid(CChunkGrid&&) = delete;
		CChunkGrid& operator = (const CChunkGrid&) = delete;
		CChunkGrid& operator = (CChunkGrid&&) = delete;

		void Initialize() final;
		void PostInitialize() final;
		void Release() final;

		void SetExtents(const Math::Vector3& minExtents, const Math::Vector3& maxExtents);
		void SetOffset(const Math::Vector3& offset);
		void SetActive(bool bActive);
		void SetEdit(bool bEdit);

		// Accessors.
		inline Math::Vector3 GetStartingExtents() const { return m_data.startingHalfExtents * 2.0f; }
		inline const Math::Vector3& GetStartingHalfExtents() const { return m_data.startingHalfExtents; }
		inline const Math::Vector3& GetStartingOffset() const { return m_data.startingOffset; }

		inline const Math::Vector3& GetScale() const { return m_scale; }
		inline const Math::SIMDVector& GetOffset() const { return m_offset; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void PreRender(Graphics::CMaterial* pMaterial);
		void OnUpdateScale();

	private:
		Data m_data;

		Math::Vector3 m_scale;
		Math::SIMDVector m_offset;
		Logic::CTransform m_transform;

		Graphics::CMeshData_ m_meshData;
		Graphics::CMeshRenderer_* m_pMeshRenderer;
		Graphics::CMaterial* m_pMaterial;
		Graphics::CMaterial* m_pMaterialUnderlay;

		Graphics::CGraphicsAPI* m_pGraphicsAPI;
	};
};

#endif
