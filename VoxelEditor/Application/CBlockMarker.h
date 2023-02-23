//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CBlockMarker.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CBLOCKMARKER_H
#define CBLOCKMARKER_H

#include <Graphics/CMeshData_.h>
#include <Objects/CVObject.h>
#include <Logic/CTransform.h>

namespace Graphics
{
	class CMeshRenderer_;
	class CMaterial;
	class CTexture;
};

namespace App
{
	class CBlockMarker : public CVObject
	{
	public:
		struct Data
		{
			u64 matHash;
		};

	public:
		CBlockMarker(const wchar_t* pName, u32 viewHash);
		~CBlockMarker();
		CBlockMarker(const CBlockMarker&) = delete;
		CBlockMarker(CBlockMarker&&) = delete;
		CBlockMarker& operator = (const CBlockMarker&) = delete;
		CBlockMarker& operator = (CBlockMarker&&) = delete;

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

		Graphics::CMeshData_ m_meshData;
		Graphics::CMeshRenderer_* m_pMeshRenderer;
		Graphics::CMaterial* m_pMaterial;
	};
};

#endif
