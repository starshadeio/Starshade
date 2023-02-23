//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUITransform.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUITransform.h"

namespace UI
{
	CUITransform::CUITransform(class CUIElement* pElement) :
		m_pParent(nullptr),
		m_pElement(pElement)
	{
		Reset();
	}

	CUITransform::~CUITransform()
	{
	}
	
	void CUITransform::Reset()
	{
		if(m_pParent)
		{
			m_pParent->RemoveChild(this);
		}

		m_bDirty = false;
		m_layer = 0;

		m_position = 0.0f;
		m_localPosition = 0.0f;
		m_scale = 1.0f;
		m_localScale = 1.0f;
		m_rotation = 0.0f;
		m_localRotation = 0.0f;

		m_right = Math::VEC2_RIGHT;
		m_up = Math::VEC2_UP;

		m_pivot = 0.5f;
		m_size = 1.0f;

		memset(m_verices, 0, sizeof(m_verices[0]) * 4);
		m_bounds = Math::Bounds2D(0.0f, 1.0f);
		m_localBounds = Math::Bounds2D(0.0f, 1.0f);

		m_internalMatrix = Math::MTX3X2_IDENTITY;
		m_localMatrix = Math::MTX3X2_IDENTITY;
		m_worldMatrix = Math::MTX3X2_IDENTITY;
		m_finalMatrix = Math::MTX3X2_IDENTITY;

		m_children.clear();
		m_pParent = nullptr;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Update methods.
	//-----------------------------------------------------------------------------------------------

	void CUITransform::Update()
	{
		if(!m_bDirty) return;

		Calculate();

		m_bDirty = false;
	}

	void CUITransform::Calculate()
	{
		m_internalMatrix  = Math::Matrix3x2::Translate(-m_pivot);
		m_internalMatrix *= Math::Matrix3x2::Scale(m_size);
		
		if(m_pParent)
		{ // Has parent.
			m_localMatrix = Math::Matrix3x2::Scale(m_localScale);
			m_localMatrix *= Math::Matrix3x2::Rotate(m_localRotation);
			m_localMatrix *= Math::Matrix3x2::Translate(m_localPosition);

			// Update world transforms.
			m_position = m_pParent->GetPosition() + m_localPosition;
			m_rotation = m_pParent->GetRotation() + m_localRotation;
			m_scale = Math::Vector2::PointwiseProduct(m_pParent->GetScale(), m_localScale);

			m_worldMatrix = m_localMatrix * m_pParent->m_worldMatrix;
		}
		else
		{ // At root.
			m_worldMatrix = Math::Matrix3x2::Scale(m_scale);
			m_worldMatrix *= Math::Matrix3x2::Rotate(m_rotation);
			m_worldMatrix *= Math::Matrix3x2::Translate(m_position);

			m_localMatrix = m_worldMatrix;
		}

		m_finalMatrix = m_internalMatrix * m_worldMatrix;

		// Calculate bounds.
		m_right = m_finalMatrix.TransformNormal(Math::VEC2_RIGHT);
		m_up = m_finalMatrix.TransformNormal(Math::VEC2_UP);
		
		m_verices[0] = m_finalMatrix.TransformPoint(Math::VEC2_ZERO);
		m_verices[1] = m_verices[0] + m_right;
		m_verices[2] = m_verices[0] + m_right + m_up;
		m_verices[3] = m_verices[0] + m_up;

		{
			Math::Vector2 extents = Math::Vector2::PointwiseProduct(m_scale, m_size);
			m_localBounds = Math::Bounds2D(m_finalMatrix.TransformPoint(0.5f) - extents * 0.5f, extents);
		}

		if(fabsf(m_rotation) < 1e-5f)
		{
			m_bounds = m_localBounds;
		}
		else
		{
			Math::Vector2 extentsMin(
				std::min(m_verices[0].x, std::min(m_verices[1].x, std::min(m_verices[2].x, m_verices[3].x))),
				std::min(m_verices[0].y, std::min(m_verices[1].y, std::min(m_verices[2].y, m_verices[3].y)))
			);
		
			Math::Vector2 extentsMax(
				std::max(m_verices[0].x, std::max(m_verices[1].x, std::max(m_verices[2].x, m_verices[3].x))),
				std::max(m_verices[0].y, std::max(m_verices[1].y, std::max(m_verices[2].y, m_verices[3].y)))
			);

			Math::Vector2 extents = extentsMax - extentsMin;

			m_bounds = Math::Bounds2D(m_finalMatrix.TransformPoint(0.5f) - extents * 0.5f, extents);
		}

		m_right.Normalize();
		m_up.Normalize();
	}

	bool CUITransform::MarkAsDirty()
	{
		if(m_bDirty) return false;
		
		for(auto& child : m_children)
		{
			child->MarkAsDirty();
		}

		m_bDirty = true;
		return true;
	}

	//-----------------------------------------------------------------------------------------------
	// Family methods.
	//-----------------------------------------------------------------------------------------------

	bool CUITransform::SetParent(CUITransform* pParent)
	{
		if(m_pParent == pParent)
		{
			return false;
		}

		if(m_pParent)
		{
			m_pParent->RemoveChild(this);
		}

		m_pParent = pParent;
		if(m_pParent)
		{
			m_pParent->m_children.push_back(this);
			m_layer = m_pParent->m_layer + 1;

			// Update local values.
			m_localPosition = m_position - m_pParent->GetPosition();
			m_localRotation = m_rotation - m_pParent->GetRotation();
			m_localScale = Math::Vector2::PointwiseQuotient(m_scale, m_pParent->GetScale());
			
			// Calculate new local matrix.
			m_localMatrix = Math::Matrix3x2::Scale(m_localScale);
			m_localMatrix *= Math::Matrix3x2::Rotate(m_localRotation);
			m_localMatrix *= Math::Matrix3x2::Translate(m_localPosition);
		}
		else
		{
			m_layer = 0;
			
			m_localPosition = m_position;
			m_localRotation = m_rotation;
			m_localScale = m_scale;
			m_localMatrix = m_worldMatrix;
		}

		return true;
	}
	
	void CUITransform::RemoveChild(CUITransform* pChild)
	{
		size_t i, j;
		for(i = 0, j = 0; i < m_children.size(); ++i)
		{
			if(m_children[i] != pChild)
			{
				m_children[j++] = m_children[i];
			}
		}

		m_children.resize(j);
	}
};
