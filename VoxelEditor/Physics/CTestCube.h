//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Physics/CTestCube.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CTESTCUBE_H
#define CTESTCUBE_H

#include <Objects/CVObject.h>
#include <Physics/CVolumeOBB.h>
#include <Physics/CVolumeSphere.h>
#include <Graphics/CDebugOBB.h>
#include <Logic/CTransform.h>

namespace Physics
{
	class CTestCube : public CVObject
	{
	public:
		CTestCube(const wchar_t* pName, u32 viewHash);
		~CTestCube();

		CTestCube(const CTestCube&) = delete;
		CTestCube(CTestCube&&) = delete;
		CTestCube& operator = (const CTestCube&) = delete;
		CTestCube& operator = (CTestCube&&) = delete;
		
		void Initialize() final;
		void Release() final;

	private:
		Logic::CTransform m_transform;

		CVolumeOBB m_volume;
		Graphics::CDebugOBB m_debugOBB;
	};
};

#endif
