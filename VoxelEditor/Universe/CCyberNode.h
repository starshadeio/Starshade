//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CCyberNode.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCYBERNODE_H
#define CCYBERNODE_H

#include <Globals/CGlobals.h>
#include <Objects/CVObject.h>
#include <Math/CMathVector3.h>
#include <Graphics/CMeshData_.h>
#include <Logic/CTransform.h>
#include <Logic/CCallback.h>
#include <Physics/CVolumeOBB.h>

namespace Graphics
{
	class CMeshRenderer_;
	class CMaterial;
};

namespace Universe
{
	class CCyberNode : public CVObject
	{
	public:
		struct Data
		{
			Math::Vector3 size;
		};

	public:
		CCyberNode(const wchar_t* pName, u32 viewHash);
		~CCyberNode();
		CCyberNode(const CCyberNode&) = delete;
		CCyberNode(CCyberNode&&) = delete;
		CCyberNode& operator = (const CCyberNode&) = delete;
		CCyberNode& operator = (CCyberNode&&) = delete;

		void Initialize() final;
		void Release() final;

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void PreRender();

		bool InteractCallback(void* pVal);

	private:
		Data m_data;
		
		Logic::CCallback m_callback;
		Logic::CTransform m_transform;
		Logic::CTransform m_transformInternal;

		Physics::CVolumeOBB m_volume;

		Graphics::CMeshData_ m_meshData;
		Graphics::CMeshRenderer_* m_pMeshRenderer;
		Graphics::CMaterial* m_pMaterial;
	};
};

#endif
