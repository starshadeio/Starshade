//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CCyberGrid.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCYBERGRID_H
#define CCYBERGRID_H

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
	class CCyberGrid : public CVObject
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
		CCyberGrid(const wchar_t* pName, u32 viewHash);
		~CCyberGrid();
		CCyberGrid(const CCyberGrid&) = delete;
		CCyberGrid(CCyberGrid&&) = delete;
		CCyberGrid& operator = (const CCyberGrid&) = delete;
		CCyberGrid& operator = (CCyberGrid&&) = delete;

		void Initialize() final;
		void Release() final;

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		
	private:
		void PreRender();

	private:
		Data m_data;

		Logic::CTransform m_transform;

		Graphics::CMeshData_ m_meshData;
		Graphics::CMeshRenderer_* m_pMeshRenderer;
		Graphics::CMaterial* m_pMaterial;
	};
};

#endif
