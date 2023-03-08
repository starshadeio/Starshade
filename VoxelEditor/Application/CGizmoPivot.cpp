//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoPivot.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmoPivot.h"
#include "CAppData.h"
#include "CEditor.h"
#include <Application/CCommandManager.h>
#include <Application/CSceneManager.h>
#include <Factory/CFactory.h>
#include <Graphics/CGraphicsAPI.h>

namespace App
{
	CGizmoPivot::CGizmoPivot() :
		m_screenScale(0.0f),
		m_startScale(0.0f),
		m_pNode(nullptr)
	{
	}

	CGizmoPivot::~CGizmoPivot()
	{
	}

	void CGizmoPivot::Update()
	{
		CalculateScreenScale();

		m_right = Math::SIMD_VEC_RIGHT;
		m_up = Math::SIMD_VEC_UP;
		m_forward = Math::SIMD_VEC_FORWARD;
	}

	void CGizmoPivot::LateUpdate()
	{
		m_rightLocal = m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetRight();
		m_upLocal = m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetUp();
		m_forwardLocal = m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetForward();
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CGizmoPivot::SetPosition(const Math::SIMDVector& position, bool bSetLast)
	{
		m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->SetPosition(position);
		if(bSetLast)
		{
			m_lastPosition = position;
			m_startScale = CalculateDistanceScale(position);
		}
	}

	void CGizmoPivot::SetRotation(const Math::SIMDQuaternion& rotation, bool bSetLast)
	{
		m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->SetRotation(rotation);
		if(bSetLast)
		{
			m_lastRotation = rotation;
		}
	}

	void CGizmoPivot::SetScale(const Math::SIMDVector& scale, bool bSetLast)
	{
		m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->SetLocalScale(scale);
		if(bSetLast)
		{
			m_lastScale = scale;
		}
	}

	void CGizmoPivot::GatherStart()
	{
		m_lastPosition = m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetPosition();
		m_lastRotation = m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetRotation();
		m_lastScale = m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetLocalScale();

		m_startScale = CalculateDistanceScale(m_lastPosition);
	}
	
	void CGizmoPivot::CalculateScreenScale()
	{
		const float invH = 1.0f / CFactory::Instance().GetGraphicsAPI()->GetHeight();
		m_screenScale = m_data.scale * invH;
	}
	
	float CGizmoPivot::CalculateDistanceScale(const Math::SIMDVector& pt)
	{
		const auto pos = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition();
		const auto f = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetForward();
		return _mm_cvtss_f32(Math::SIMDVector::Dot(pt - pos, f));
	}
};
