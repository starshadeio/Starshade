//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPrimQuad.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPRIMQUAD_H
#define CPRIMQUAD_H

#include "CMeshData_.h"
#include <Objects/CVObject.h>
#include <Logic/CTransform.h>
#include <Math/CMathVector2.h>
#include <Math/CSIMDVector.h>

namespace Graphics
{
	class CPrimQuad : public CVObject
	{
	public:
		struct Data
		{
			Math::Vector2 halfSize;
			Math::Vector3 right;
			Math::Vector3 up;
			u64 matHash;
		};

	public:
		CPrimQuad(const wchar_t* pName, u32 viewHash);
		~CPrimQuad();
		CPrimQuad(const CPrimQuad&) = delete;
		CPrimQuad(CPrimQuad&&) = delete;
		CPrimQuad& operator = (const CPrimQuad&) = delete;
		CPrimQuad& operator = (CPrimQuad&&) = delete;
		
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
