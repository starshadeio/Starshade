//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CNodeGrid.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODEGRID_H
#define CNODEGRID_H

#include <Globals/CGlobals.h>
#include <Objects/CVObject.h>
#include <Math/CMathVector3.h>
#include <Graphics/CMeshData_.h>
#include <Logic/CTransform.h>

namespace Graphics
{
	class CMeshRenderer_;
	class CMaterial;
};

namespace Universe
{
	class CNodeGrid : public CVObject
	{
	public:
		struct Data
		{
			u32 width;
			u32 height;
			u32 length;
			Math::Vector3 cellSize;
			Math::Vector3 right;
			Math::Vector3 up;
			Math::Vector3 forward;
		};

	public:
		CNodeGrid(const wchar_t* pName, u32 viewHash);
		~CNodeGrid();
		CNodeGrid(const CNodeGrid&) = delete;
		CNodeGrid(CNodeGrid&&) = delete;
		CNodeGrid& operator = (const CNodeGrid&) = delete;
		CNodeGrid& operator = (CNodeGrid&&) = delete;

		void Initialize() final;
		void LateUpdate() final;
		void Release() final;

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetActive(bool bActive) { m_bActive = bActive; }

	private:
		void PreRender();

	private:
		bool m_bActive;
		Data m_data;

		Logic::CTransform m_transform;

		Graphics::CMeshData_ m_meshData;
		Graphics::CMeshRenderer_* m_pMeshRenderer;
		Graphics::CMaterial* m_pMaterial;
	};
};

#endif
