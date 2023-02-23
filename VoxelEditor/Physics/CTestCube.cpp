//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Physics/CTestCube.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CTestCube.h"
#include <Application/CSceneManager.h>

namespace Physics
{
	CTestCube::CTestCube(const wchar_t* pName, u32 viewHash) : 
		CVObject(pName, viewHash),
		m_transform(this),
		m_volume(this),
		m_debugOBB(L"Test Debug OBB", viewHash)
	{
	}

	CTestCube::~CTestCube()
	{
	}
	
	void CTestCube::Initialize()
	{
		m_transform.SetPosition(Math::SIMD_VEC_UP * 5.0f);

		{
			// Setup physics volume.
			CVolumeOBB::Data data { };
			data.halfSize = Math::VEC3_ONE * 0.5f;
			data.colliderType = ColliderType::None;
			m_volume.SetData(data);
			m_volume.Register(m_transform.GetWorldMatrix());
		}

		{ // Setup debug cube.
			Graphics::CDebugOBB::Data data { };
			data.offset = 0.0f;
			data.halfSize = Math::VEC3_ONE * 0.5f;
			data.matHash = Math::FNV1a_64("MATERIAL_DEBUG_GREEN");
			m_debugOBB.SetData(data);
			m_debugOBB.Initialize();
			m_debugOBB.GetTransform()->SetParent(&m_transform);
		}
	}
	
	void CTestCube::Release()
	{
		m_volume.Deregister();
		
		m_debugOBB.Release();
	}
};
