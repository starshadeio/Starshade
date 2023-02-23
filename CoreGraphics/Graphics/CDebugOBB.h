//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CDebugOBB.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDEBUGOBB_H
#define CDEBUGOBB_H

#include <Objects/CVObject.h>
#include <Logic/CTransform.h>
#include "../Graphics/CMeshData_.h"
#include <Math/CMathVector3.h>
#include <Math/CMathVector2.h>

namespace Graphics
{
	class CDebugOBB : public CVObject
	{
	public:
		struct Data
		{
			Math::Vector3 halfSize;
			Math::Vector3 offset;
			u64 matHash;
		};

	public:
		CDebugOBB(const wchar_t* pName, u32 viewHash);
		~CDebugOBB();

		CDebugOBB(const CDebugOBB&) = delete;
		CDebugOBB(CDebugOBB&&) = delete;
		CDebugOBB& operator = (const CDebugOBB&) = delete;
		CDebugOBB& operator = (CDebugOBB&&) = delete;

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
