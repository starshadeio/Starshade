//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoPivot_.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmoPivot_.h"
#include "CAppData.h"
#include "CEditor.h"
#include <Application/CCommandManager.h>
#include <Application/CSceneManager.h>
#include <Factory/CFactory.h>
#include <Graphics/CGraphicsAPI.h>

namespace App
{
	CGizmoPivot_::CGizmoPivot_() :
		m_screenScale(0.0f),
		m_startScale(0.0f),
		m_pObject(nullptr)
	{
	}

	CGizmoPivot_::~CGizmoPivot_()
	{
	}

	void CGizmoPivot_::Update()
	{
		CalculateScreenScale();

		m_right = Math::SIMD_VEC_RIGHT;
		m_up = Math::SIMD_VEC_UP;
		m_forward = Math::SIMD_VEC_FORWARD;
	}

	void CGizmoPivot_::LateUpdate()
	{
		m_rightLocal = m_pObject ? m_pObject->GetTransform()->GetRight() : m_callbacks.getRight();
		m_upLocal = m_pObject ? m_pObject->GetTransform()->GetUp() : m_callbacks.getUp();
		m_forwardLocal = m_pObject ? m_pObject->GetTransform()->GetForward() : m_callbacks.getForward();
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CGizmoPivot_::SetPosition(const Math::SIMDVector& position, bool bSetLast)
	{
		if(m_pObject)
		{
			m_pObject->GetTransform()->SetPosition(position);
		}
		else
		{
			m_callbacks.setPosition(position);
		}

		if(bSetLast)
		{
			m_lastPosition = position;
			m_startScale = CalculateDistanceScale(position);
		}
	}

	void CGizmoPivot_::SetRotation(const Math::SIMDQuaternion& rotation, bool bSetLast)
	{
		if(m_pObject)
		{
			m_pObject->GetTransform()->SetRotation(rotation);
		}
		else
		{
			m_callbacks.setRotation(rotation);
		}

		if(bSetLast)
		{
			m_lastRotation = rotation;
		}
	}

	void CGizmoPivot_::SetScale(const Math::SIMDVector& scale, bool bSetLast)
	{
		if(m_pObject)
		{
			m_pObject->GetTransform()->SetScale(scale);
		}
		else
		{
			m_callbacks.setScale(scale);
		}

		if(bSetLast)
		{
			m_lastScale = scale;
		}
	}

	void CGizmoPivot_::GatherStart()
	{
		m_lastPosition = GetPosition();
		m_lastRotation = GetRotation();
		m_lastScale = GetScale();

		m_startScale = CalculateDistanceScale(m_lastPosition);
	}
	
	void CGizmoPivot_::CalculateScreenScale()
	{
		const float invH = 1.0f / CFactory::Instance().GetGraphicsAPI()->GetHeight();
		m_screenScale = m_data.scale * invH;
	}
	
	float CGizmoPivot_::CalculateDistanceScale(const Math::SIMDVector& pt)
	{
		const auto pos = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition();
		const auto f = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetForward();
		return _mm_cvtss_f32(Math::SIMDVector::Dot(pt - pos, f));
	}
};
