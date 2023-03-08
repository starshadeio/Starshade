//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Logic/CCamera.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CCamera.h"

namespace Logic
{
	CCamera::CCamera(const CVObject* pObject, CTransform* pTransform) : 
		CVComponent(pObject),
		m_pTransform(pTransform)
	{
		m_pTransform->AddOnUpdateComponent(this);
	}

	void CCamera::SetupProjection(const Data& data)
	{
		m_data = data;
		SetupProjection();
	}

	void CCamera::SetupProjection(const Math::Rect& screenRect)
	{
		m_data.screenRect = screenRect;
		m_data.aspectRatio = static_cast<float>(m_data.screenRect.w) / static_cast<float>(m_data.screenRect.h);
		SetupProjection();
	}

	void CCamera::SetupProjection()
	{
		switch(m_data.projMode)
		{
			case CAMERA_PROJ_MODE_ORTHOGRAPHIC:
				m_projMatrix = Math::SIMDMatrix::OrthographicLH(m_data.size * m_data.aspectRatio, m_data.size, m_data.nearView, m_data.farView);
				m_projMatrixInv = Math::SIMDMatrix::OrthographicLHInv(m_data.size * m_data.aspectRatio, m_data.size, m_data.nearView, m_data.farView);
				break;
			case CAMERA_PROJ_MODE_PERSPECTIVE:
				m_projMatrix = Math::SIMDMatrix::PerspectiveFovLH(m_data.fov, m_data.aspectRatio, m_data.nearView, m_data.farView);
				m_projMatrixInv = Math::SIMDMatrix::PerspectiveFovLHInv(m_data.fov, m_data.aspectRatio, m_data.nearView, m_data.farView);
				break;
			default:
				break;
		}
	}

	void CCamera::Update()
	{
		// With some derived transform types m_pTransform->GetPosition() isn't the true world position, so extract it from the world matrix instead.
		m_viewMatrix = m_pTransform->GetWorldMatrix();
		m_viewMatrix.Transpose();
		const Math::SIMDVector position = m_viewMatrix.rows[3];

		m_viewMatrix = Math::SIMDMatrix::LookAtLH(
			position,
			position + m_pTransform->GetForward(),
			m_pTransform->GetUp().m_xmm
		);
		
		m_viewMatrixInv = m_pTransform->GetWorldMatrix();

		CalculateFrustumPlanes();
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CCamera::CalculateFrustumPlanes()
	{
		Math::SIMDMatrix combMatrix = m_viewMatrix * m_projMatrix;

		m_frustumPlanes[0] = Math::SIMDPlane(_mm_add_ps(combMatrix.rows[3], combMatrix.rows[0])).Normalized(); // Left
		m_frustumPlanes[1] = Math::SIMDPlane(_mm_sub_ps(combMatrix.rows[3], combMatrix.rows[0])).Normalized(); // Right
		m_frustumPlanes[2] = Math::SIMDPlane(_mm_add_ps(combMatrix.rows[3], combMatrix.rows[1])).Normalized(); // Bottom
		m_frustumPlanes[3] = Math::SIMDPlane(_mm_sub_ps(combMatrix.rows[3], combMatrix.rows[1])).Normalized(); // Top
		m_frustumPlanes[4] = Math::SIMDPlane(_mm_add_ps(combMatrix.rows[3], combMatrix.rows[2])).Normalized(); // Near
		m_frustumPlanes[5] = Math::SIMDPlane(_mm_sub_ps(combMatrix.rows[3], combMatrix.rows[2])).Normalized(); // Far
	}

	bool CCamera::CheckPoint(const Math::SIMDVector& pt) const
	{
		for(size_t i = 0; i < 6; ++i)
		{
			if(_mm_cvtss_f32(m_frustumPlanes[i].DotCoord(pt)) < 0.0f) return false;
		}

		return true;
	}
};
