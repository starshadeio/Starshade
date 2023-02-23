//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CDebugGrid.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDEBUGGRID_H
#define CDEBUGGRID_H

#include "../Graphics/CMeshData_.h"
#include <Objects/CVObject.h>
#include <Logic/CTransform.h>
#include <Math/CMathVector3.h>
#include <Math/CMathVector2.h>

namespace Graphics
{
	class CDebugGrid : public CVObject
	{
	public:
		struct Data
		{
			u32 width;
			u32 length;
			Math::Vector2 cellSize;
			Math::Vector3 right;
			Math::Vector3 forward;
			u64 matHash;
		};

	public:
		CDebugGrid(const wchar_t* pName, u32 viewHash);
		~CDebugGrid();
		CDebugGrid(const CDebugGrid&) = delete;
		CDebugGrid(CDebugGrid&&) = delete;
		CDebugGrid& operator = (const CDebugGrid&) = delete;
		CDebugGrid& operator = (CDebugGrid&&) = delete;

		void Initialize() final;
		void Release() final;

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		
	private:
		void PreRender();

	private:
		Data m_data;

		Logic::CTransform m_transform;

		CMeshData_ m_meshData;
		class CMeshRenderer_* m_pMeshRenderer;
		class CMaterial* m_pMaterial;
	};
};

#endif
