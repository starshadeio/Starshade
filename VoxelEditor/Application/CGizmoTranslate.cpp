//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoTranslate.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmoTranslate.h"
#include "CGizmoNode.h"
#include "CEditor.h"
#include <Application/CCommandManager.h>
#include <Graphics/CGraphicsAPI.h>
#include <Factory/CFactory.h>
#include <Application/CSceneManager.h>
#include <unordered_map>

namespace App
{
	static u32 GizmoIndex(u32 hash)
	{
		static const std::unordered_map<u32, u32> GIZMO_HASH_MAP = {
			{ CGizmoTranslate::GIZMO_HASH_AXIS_X, 0 },
			{ CGizmoTranslate::GIZMO_HASH_AXIS_Y, 1 },
			{ CGizmoTranslate::GIZMO_HASH_AXIS_Z, 2 },
			{ CGizmoTranslate::GIZMO_HASH_PLANE_X, 3 },
			{ CGizmoTranslate::GIZMO_HASH_PLANE_Y, 4 },
			{ CGizmoTranslate::GIZMO_HASH_PLANE_Z, 5 },
		};

		return GIZMO_HASH_MAP.find(hash)->second;
	}

	CGizmoTranslate::CGizmoTranslate() : 
		m_clickPoint(0)
	{
		memset(m_bHovered, 0, sizeof(m_bHovered));
		memset(m_bClicked, 0, sizeof(m_bClicked));
	}

	CGizmoTranslate::~CGizmoTranslate()
	{
	}
	
	void CGizmoTranslate::Interact()
	{
		const float planeSize = m_data.planeSize * m_data.pGizmoPivot->GetScreenScale();

		// X-Axis (Line)
		GizmoInteract gizmoInteract { };
		gizmoInteract.hash = GIZMO_HASH_AXIS_X;
		gizmoInteract.type = GizmoInteractType::Line;
		gizmoInteract.onEnter = std::bind(&CGizmoTranslate::OnEnter, this, std::placeholders::_1);
		gizmoInteract.onExit = std::bind(&CGizmoTranslate::OnExit, this, std::placeholders::_1);
		gizmoInteract.onPressed = std::bind(&CGizmoTranslate::OnPressed, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		gizmoInteract.whileHeld = std::bind(&CGizmoTranslate::WhileHeld, this, std::placeholders::_1, std::placeholders::_2);
		gizmoInteract.onReleased = std::bind(&CGizmoTranslate::OnReleased, this, std::placeholders::_1);
		gizmoInteract.line.origin = m_data.pGizmoPivot->GetPosition();
		gizmoInteract.line.dir = m_data.pGizmoPivot->GetRight();
		gizmoInteract.line.length = m_data.pGizmoPivot->GetScreenScale() + (m_data.coneSize * 1.5f * m_data.pGizmoPivot->GetScreenScale());
		gizmoInteract.line.width = 0.015f;

		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);
		
		// Y-Axis (Line)
		gizmoInteract.hash = GIZMO_HASH_AXIS_Y;
		gizmoInteract.line.dir = m_data.pGizmoPivot->GetUp();
		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);
		
		// Z-Axis (Line)
		gizmoInteract.hash = GIZMO_HASH_AXIS_Z;
		gizmoInteract.line.dir = m_data.pGizmoPivot->GetForward();
		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);

		// X-Axis (Quad)
		gizmoInteract.type = GizmoInteractType::Quad;
		gizmoInteract.priority = 1;
		gizmoInteract.quad.origin = m_data.pGizmoPivot->GetPosition();
		gizmoInteract.quad.size = Math::VEC2_ONE * planeSize;

		gizmoInteract.hash = GIZMO_HASH_PLANE_X;
		gizmoInteract.quad.normal = m_data.pGizmoPivot->GetRight();
		gizmoInteract.quad.tangent = m_data.pGizmoPivot->GetForward();
		gizmoInteract.quad.bitangent = m_data.pGizmoPivot->GetUp();
		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);

		// Y-Axis (Quad)
		gizmoInteract.hash = GIZMO_HASH_PLANE_Y;
		gizmoInteract.quad.normal = m_data.pGizmoPivot->GetUp();
		gizmoInteract.quad.tangent = m_data.pGizmoPivot->GetRight();
		gizmoInteract.quad.bitangent = m_data.pGizmoPivot->GetForward();
		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);

		// Z-Axis (Quad)
		gizmoInteract.hash = GIZMO_HASH_PLANE_Z;
		gizmoInteract.quad.normal = m_data.pGizmoPivot->GetForward();
		gizmoInteract.quad.tangent = m_data.pGizmoPivot->GetRight();
		gizmoInteract.quad.bitangent = m_data.pGizmoPivot->GetUp();
		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);
	}

	void CGizmoTranslate::Draw()
	{
		const float coneSize = m_data.coneSize * m_data.pGizmoPivot->GetScreenScale();
		const float planeSize = m_data.planeSize * m_data.pGizmoPivot->GetScreenScale();

		// Lambda for drawing an axis for this gizmo.
		auto DrawAxis = [this, &coneSize, &planeSize](u32 axisIndex, u32 planeIndex, const Math::SIMDVector& normal, const Math::SIMDVector& tangent, const Math::SIMDVector& bitangent, const Math::Color& color) {
			const float scaleAxis = m_bHovered[axisIndex] ? m_data.axisHoveredWidth : m_data.axisWidth;
			const Math::Vector3 scaleCone = m_bHovered[axisIndex] ? Math::Vector3(m_data.coneHoveredScale, m_data.coneHoveredScale, 1.0f) : 1.0f;
			const Math::Color colorAxis = m_bClicked[axisIndex] ? Math::COLOR_YELLOW : color;
			const Math::Color colorPlaneOutline = m_bClicked[planeIndex] ? Math::COLOR_YELLOW : color;

			CEditor::Instance().Nodes().Gizmo().Draw().Line().Draw(m_data.pGizmoPivot->GetPosition(), tangent * planeSize, bitangent, planeSize, 1.0f, colorPlaneOutline);
			CEditor::Instance().Nodes().Gizmo().Draw().Line().Draw(m_data.pGizmoPivot->GetPosition(), bitangent * planeSize, tangent, planeSize, 1.0f, colorPlaneOutline);

			CEditor::Instance().Nodes().Gizmo().Draw().Line().Draw(m_data.pGizmoPivot->GetPosition(), 0.0f, normal, m_data.pGizmoPivot->GetScreenScale(), scaleAxis, colorAxis);
			CEditor::Instance().Nodes().Gizmo().Draw().Cone().Draw(m_data.pGizmoPivot->GetPosition(), normal * m_data.pGizmoPivot->GetScreenScale(), Math::SIMDQuaternion::RotateDirection(normal, bitangent), scaleCone * coneSize, colorAxis);
			
			if(m_bHovered[planeIndex])
			{
				const Math::Color colorQuad = m_bClicked[planeIndex] ? Math::Color(1.0f, 1.0f, 0.0f, 0.5f) : Math::Color(color.r, color.g, color.b, 0.5f);

				CEditor::Instance().Nodes().Gizmo().Draw().Quad().Draw(m_data.pGizmoPivot->GetPosition(), (bitangent + tangent) * planeSize * 0.5f, 
					Math::SIMDQuaternion::RotateDirection(normal, bitangent), Math::VEC3_ONE * planeSize, colorQuad);
			}
		};

		// X-Axis
		DrawAxis(GizmoIndex(GIZMO_HASH_AXIS_X), GizmoIndex(GIZMO_HASH_PLANE_X), m_data.pGizmoPivot->GetRight(), m_data.pGizmoPivot->GetForward(), m_data.pGizmoPivot->GetUp(), Math::COLOR_RED);

		// Y-Axis
		DrawAxis(GizmoIndex(GIZMO_HASH_AXIS_Y), GizmoIndex(GIZMO_HASH_PLANE_Y), m_data.pGizmoPivot->GetUp(), m_data.pGizmoPivot->GetRight(), m_data.pGizmoPivot->GetForward(), Math::COLOR_GREEN);

		// Z-Axis
		DrawAxis(GizmoIndex(GIZMO_HASH_AXIS_Z), GizmoIndex(GIZMO_HASH_PLANE_Z), m_data.pGizmoPivot->GetForward(), m_data.pGizmoPivot->GetRight(), m_data.pGizmoPivot->GetUp(), Math::COLOR_BLUE);
	}

	//-----------------------------------------------------------------------------------------------
	// Interaction methods.
	//-----------------------------------------------------------------------------------------------

	void CGizmoTranslate::OnEnter(u32 hash)
	{
		m_bHovered[GizmoIndex(hash)] = true;
	}

	void CGizmoTranslate::OnExit(u32 hash)
	{
		m_bHovered[GizmoIndex(hash)] = false;
	}
	
	void CGizmoTranslate::OnPressed(u32 hash, const Math::CSIMDRay& ray, float t)
	{
		m_bClicked[GizmoIndex(hash)] = true;
		
		m_data.pGizmoPivot->GatherStart();

		Math::SIMDVector outPt;
		CalculatePoint(hash, ray, outPt);

		m_clickPoint = outPt;
	}
	
	void CGizmoTranslate::WhileHeld(u32 hash, const Math::CSIMDRay& ray)
	{
		Math::SIMDVector outPt;
		if(CalculatePoint(hash, ray, outPt))
		{
			if(m_data.snap > 0.0f)
			{
				const vf32 snapVec = _mm_set_ps1(m_data.snap);
				outPt = _mm_mul_ps(_mm_round_ps(_mm_div_ps(outPt.m_xmm, snapVec), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC), snapVec);
			}

			m_data.pGizmoPivot->SetPosition(m_data.pGizmoPivot->GetLastPosition() + outPt);
		}
	}

	void CGizmoTranslate::OnReleased(u32 hash)
	{
		Math::SIMDVector position = m_data.pGizmoPivot->GetPosition();
		App::CCommandManager::Instance().Execute(m_data.onReleaseHash, &position);

		m_clickPoint = 0;
		m_bClicked[GizmoIndex(hash)] = false;
	}

	//-----------------------------------------------------------------------------------------------
	// Intersection methods.
	//-----------------------------------------------------------------------------------------------

	bool CGizmoTranslate::TranslateAxis(const Math::SIMDVector& axis, const Math::CSIMDRay& ray, Math::SIMDVector& outPt)
	{
		float t0;
		if(CGizmoInteract::EdgePoint(m_data.pGizmoPivot->GetLastPosition(), axis, ray, t0))
		{
			const float scale = m_data.pGizmoPivot->GetStartScale();//m_data.pGizmoPivot->CalculateDistanceScale(m_data.pGizmoPivot->GetPosition());
			const float bias = (scale - m_data.pGizmoPivot->GetStartScale()) * m_data.pGizmoPivot->GetScreenScale();

			auto pt = ray.GetOrigin() + ray.GetDirection() * t0;
			outPt = ((axis * (_mm_cvtss_f32(Math::SIMDVector::Dot(axis, pt)) - bias)) - m_clickPoint);
			return true;
		}

		return false;
	}

	bool CGizmoTranslate::TranslatePlane(const Math::SIMDVector& normal, const Math::CSIMDRay& ray, Math::SIMDVector& outPt)
	{
		const float dTest = _mm_cvtss_f32(Math::SIMDVector::Dot(ray.GetOrigin() - m_data.pGizmoPivot->GetPosition(), normal));

		float t0;
		if(CGizmoInteract::PlanePoint(m_data.pGizmoPivot->GetLastPosition(), normal * Math::Sign(dTest), ray, t0))
		{
			outPt = ray.GetOrigin() + ray.GetDirection() * t0 - m_clickPoint;
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	bool CGizmoTranslate::CalculatePoint(u32 hash, const Math::CSIMDRay& ray, Math::SIMDVector& outPt)
	{
		bool bHit = false;
		switch(hash)
		{
			case GIZMO_HASH_AXIS_X:
				bHit |= TranslateAxis(m_data.pGizmoPivot->GetRight(), ray, outPt);
				break;
			case GIZMO_HASH_AXIS_Y:
				bHit |= TranslateAxis(m_data.pGizmoPivot->GetUp(), ray, outPt);
				break;
			case GIZMO_HASH_AXIS_Z:
				bHit |= TranslateAxis(m_data.pGizmoPivot->GetForward(), ray, outPt);
				break;
			case GIZMO_HASH_PLANE_X:
				bHit |= TranslatePlane(m_data.pGizmoPivot->GetRight(), ray, outPt);
				break;
			case GIZMO_HASH_PLANE_Y:
				bHit |= TranslatePlane(m_data.pGizmoPivot->GetUp(), ray, outPt);
				break;
			case GIZMO_HASH_PLANE_Z:
				bHit |= TranslatePlane(m_data.pGizmoPivot->GetForward(), ray, outPt);
				break;
			default:
				break;
		}

		return bHit;
	}
};
