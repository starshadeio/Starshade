//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CPrimTriangle.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPRIMTRIANGLE_H
#define CPRIMTRIANGLE_H

#include <Objects/CVObject.h>
#include "../Graphics/CMeshData_.h"

namespace Graphics
{
	class CPrimTriangle : public CVObject
	{
	public:
		CPrimTriangle(const wchar_t* pName, u32 viewHash);
		~CPrimTriangle();
		CPrimTriangle(const CPrimTriangle&) = delete;
		CPrimTriangle(CPrimTriangle&&) = delete;
		CPrimTriangle& operator = (const CPrimTriangle&) = delete;
		CPrimTriangle& operator = (CPrimTriangle&&) = delete;

		void Initialize() final;
		void PostInitialize() final;
		void Update() final;
		void Release() final;

	private:
		u8 m_uavBufferCount;

		CMeshData_ m_meshData;
		class CMeshRenderer_* m_pMeshRenderer;
		class CCompute* m_pCompute;
		class CMaterial* m_pMaterial;
		class CMaterial* m_pComputeMaterial;
		class CUAVBuffer** m_pUAVBufferList;
		class CTexture* m_pTexture;

		class CGraphicsAPI* m_pGraphicsAPI;
	};
};

#endif
