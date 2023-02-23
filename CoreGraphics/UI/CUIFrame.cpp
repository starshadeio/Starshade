//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIFrame.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIFrame.h"
#include "CUICanvas.h"
#include "../Utilities/CDebug.h"
#include "../Factory/CFactory.h"
#include "../Graphics/CGraphicsAPI.h"
#include <cassert>

namespace UI
{
	CUIFrame::CUIFrame(const CVObject* pObject, class CUICanvas* pCanvas) :
		CVComponent(pObject),
		m_pCanvas(pCanvas),
		m_bActive(true),
		m_bActiveSelf(true),
		m_bHidden(false),
		m_bDirty(false),
		m_layer(0),
		m_onUpdate(nullptr)
	{
	}

	CUIFrame::~CUIFrame()
	{
	}

	void CUIFrame::Initialize()
	{
		m_bDirty = false;

		if(m_pCanvas->GetFrame() != this)
		{
			if(m_data.pParent == nullptr)
			{
				m_data.pParent = m_pCanvas->GetFrame();
			}

			m_layer = m_data.pParent->m_layer + 1;
			m_data.pParent->m_children.push_back(this);
		}
		else
		{
			MarkAsDirty();
		}
	}

	void CUIFrame::Update()
	{
		if(!m_bDirty) return;
		m_bDirty = false;

		// Active check.
		if(m_data.pParent)
		{
			if(m_data.pParent->IsActive())
			{
				const bool bActive = m_data.pParent->IsActive() && m_bActiveSelf;
				if(m_bActive != bActive)
				{
					m_bActive = bActive;

					if(m_onActiveChange)
					{
						m_onActiveChange(m_bActive);
					}
				}
			}
			else if(m_bActive)
			{
				m_bActive = m_data.pParent->IsActive();
				if(m_onActiveChange)
				{
					m_onActiveChange(m_bActive);
				}
			}
		}
		else if(m_bActiveSelf != m_bActive)
		{
			m_bActive = m_bActiveSelf;
			if(m_onActiveChange)
			{
				m_onActiveChange(m_bActive);
			}
		}

		if(!m_bActiveSelf || !m_bActive)
		{
			return;
		}

		// Get parental bounds.
		Math::Bounds2D bounds;
		Math::Vector2 viewportSize = Math::Vector2(CFactory::Instance().GetGraphicsAPI()->GetWidth(), CFactory::Instance().GetGraphicsAPI()->GetHeight());
		if(m_data.pParent)
		{
			bounds = m_data.pParent->GetBounds();
		}
		else
		{
			bounds = Math::Bounds2D(Math::Vector2(0.0f), viewportSize);
		}

		// Calculate data from pivot and anchors.
		Math::Vector2 pivot = Math::Vector2::PointwiseProduct(m_data.size, m_data.pivot);
		Math::Vector2 anchorMin = bounds.minExtents + Math::Vector2::PointwiseProduct(bounds.size, m_data.anchorMin);
		Math::Vector2 anchorMax = bounds.minExtents + Math::Vector2::PointwiseProduct(bounds.size, m_data.anchorMax);

		Math::Vector2 minExtents;
		Math::Vector2 maxExtents;

		for(u32 i = 0; i < 2; ++i)
		{
			if(anchorMin[i] != anchorMax[i])
			{
				assert(anchorMin[i] < anchorMax[i]);

				minExtents[i] = anchorMin[i] + m_data.rect[i].x;
				maxExtents[i] = anchorMax[i] - m_data.rect[i].y;
			}
			else
			{
				minExtents[i] = (anchorMin[i] + m_data.position[i]) - pivot[i];
				maxExtents[i] = minExtents[i] + m_data.size[i];
			}
		}

		// Calculate bounds.
		Math::Vector2 extents = maxExtents - minExtents;

		if(m_data.bUseMinWidth || m_data.bUseMaxWidth)
		{
			const float diffMin = std::max(0.0f, m_data.minWidth - extents.x) * m_data.bUseMinWidth;
			const float diffMax = std::max(0.0f, extents.x - m_data.maxWidth) * m_data.bUseMaxWidth;
			minExtents.x -= (diffMin - diffMax) * (1.0f - m_data.pivot.x);
			maxExtents.x += (diffMin - diffMax) * (m_data.pivot.x);

			extents.x = maxExtents.x - minExtents.x;
		}

		if(m_data.bUseMinHeight || m_data.bUseMaxHeight)
		{
			const float diffMin = std::max(0.0f, m_data.minHeight - extents.y) * m_data.bUseMinHeight;
			const float diffMax = std::max(0.0f, extents.y - m_data.maxHeight) * m_data.bUseMaxHeight;
			minExtents.y -= (diffMin - diffMax) * (m_data.pivot.y);
			maxExtents.y += (diffMin - diffMax) * (1.0f - m_data.pivot.y);

			extents.y = maxExtents.y - minExtents.y;
		}

		m_bounds = Math::Bounds2D(minExtents, extents);

		pivot = Math::Vector2::PointwiseProduct(extents, m_data.pivot);
		
		// Calculate rotation, scale and world matrices.
		if(m_data.pParent)
		{
			Math::Vector2 diff = m_data.pParent->m_bounds.minExtents - minExtents;
			m_scaleMatrix = Math::Matrix3x2::Translate(-diff) * m_data.pParent->m_scaleMatrix * Math::Matrix3x2::Translate(diff);
			m_rotationMatrix = Math::Matrix3x2::Translate(-diff) * m_data.pParent->m_rotationMatrix * Math::Matrix3x2::Translate(diff);
		}
		else
		{
			m_scaleMatrix = Math::MTX3X2_IDENTITY;
			m_rotationMatrix = Math::MTX3X2_IDENTITY;
		}
		
		m_scaleMatrix = Math::Matrix3x2::Translate(-pivot) * Math::Matrix3x2::Scale(m_data.scale) * Math::Matrix3x2::Translate(pivot) * m_scaleMatrix;
		m_rotationMatrix = Math::Matrix3x2::Translate(-pivot) * Math::Matrix3x2::Rotate(m_data.rotation) * Math::Matrix3x2::Translate(pivot) * m_rotationMatrix;

		m_internalMatrix = m_scaleMatrix * m_rotationMatrix * Math::Matrix3x2::Translate(minExtents - viewportSize * 0.5f);
		m_internalMatrixInv = m_scaleMatrix * m_rotationMatrix;
		m_internalMatrixInv.Inverse();

		m_worldMatrix = Math::Matrix3x2::Scale(extents) * m_internalMatrix;

		m_right = m_worldMatrix.TransformNormal(Math::VEC2_RIGHT).Normalized();
		m_up = m_worldMatrix.TransformNormal(Math::VEC2_UP).Normalized();
		
		if(!m_data.bAllowOverflow && m_data.pParent)
		{ // Test if this frame is fully inside of the parent container.

			bool bLastActive = IsActive();
			m_bHidden = false;

			if(fabsf(m_data.rotation) < 1e-5f)
			{
				Math::Bounds2D centeredBounds(m_data.pParent->m_worldMatrix.TransformPoint(0.0f), m_data.pParent->m_bounds.size);

				const auto v0 = m_worldMatrix.TransformPoint({ 0.0f, 0.0f });
				const auto v1 = m_worldMatrix.TransformPoint({ 1.0f, 0.0f });
				const auto v2 = m_worldMatrix.TransformPoint({ 1.0f, 1.0f });
				const auto v3 = m_worldMatrix.TransformPoint({ 0.0f, 1.0f });

				m_bHidden |= !centeredBounds.IsIntersecting(v0);
				m_bHidden |= !centeredBounds.IsIntersecting(v1);
				m_bHidden |= !centeredBounds.IsIntersecting(v2);
				m_bHidden |= !centeredBounds.IsIntersecting(v3);
			}
			else
			{
				m_bHidden |= !m_data.pParent->m_bounds.HasInside(m_bounds);
			}

			if(bLastActive != IsActive())
			{
				if(m_onActiveChange)
				{
					m_onActiveChange(m_bActive);
				}
			}

			return;
		}

		if(m_onUpdate != nullptr)
		{
			m_onUpdate();
		}
	}

	void CUIFrame::DebugUpdate()
	{
		Util::CDebug::Instance().Drawer().Rect().Draw(m_worldMatrix, Math::COLOR_GREEN);
	}
	
	//---------------------------------------------------------------------------------------------
	// (De)activation methods.
	//---------------------------------------------------------------------------------------------

	bool CUIFrame::SetActive(bool bActive)
	{
		if(m_bActiveSelf == bActive)
		{
			return false;
		}

		m_bActiveSelf = bActive;
		MarkAsDirty();

		return true;
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUIFrame::IsIntersecting(const Math::Vector2& point, Math::Vector2* pRelativePt) const
	{
		Math::Vector2 viewportSize = Math::Vector2(CFactory::Instance().GetGraphicsAPI()->GetWidth(), CFactory::Instance().GetGraphicsAPI()->GetHeight());
		Math::Vector2 pt = point + viewportSize * 0.5f - m_bounds.minExtents;
		Math::Vector2 newPoint = m_internalMatrixInv.TransformPoint(pt);
		if(pRelativePt) *pRelativePt = newPoint;
		return Math::Bounds2D(0.0f, m_bounds.size).IsIntersecting(newPoint);
	}

	bool CUIFrame::MarkAsDirty()
	{
		if(m_bDirty) { return false; }
		m_bDirty = true;

		for(auto& child : m_children)
		{
			child->MarkAsDirty();
		}

		m_pCanvas->MarkFrameAsDirty(this);

		return true;
	}
};
