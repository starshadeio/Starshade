//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPrimCube.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPRIMCUBE_H
#define CPRIMCUBE_H

#include "CMeshData_.h"
#include <Objects/CVObject.h>
#include <Logic/CTransform.h>
#include <Math/CMathVector3.h>
#include <Math/CSIMDVector.h>

namespace Graphics
{
	class CPrimCube : public CVObject
	{
	public:
		struct Data
		{
			bool bTextured;
			Math::Vector3 halfSize;
			u64 matHash;
		};

	public:
		CPrimCube(const wchar_t* pName, u32 viewHash);
		~CPrimCube();
		CPrimCube(const CPrimCube&) = delete;
		CPrimCube(CPrimCube&&) = delete;
		CPrimCube& operator = (const CPrimCube&) = delete;
		CPrimCube& operator = (CPrimCube&&) = delete;
		
		void Initialize() final;
		void Release() final;
		
		void SetActive(bool bActive);

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
		class CTexture* m_pTexture;
	};
};

#endif
