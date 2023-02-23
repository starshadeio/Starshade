//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoScale.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmoScale.h"
#include "CGizmoPivot.h"
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
			{ CGizmoScale::GIZMO_HASH_AXIS_X, 0 },
			{ CGizmoScale::GIZMO_HASH_AXIS_Y, 1 },
			{ CGizmoScale::GIZMO_HASH_AXIS_Z, 2 },
			{ CGizmoScale::GIZMO_HASH_CENTER, 3 },
		};

		return GIZMO_HASH_MAP.find(hash)->second;
	}

	CGizmoScale::CGizmoScale() : 
		m_clickPoint(0),
		m_startScale(1.0f),
		m_scale(1.0f)
	{
		memset(m_bHovered, 0, sizeof(m_bHovered));
		memset(m_bClicked, 0, sizeof(m_bClicked));
	}

	CGizmoScale::~CGizmoScale()
	{
	}
	
	void CGizmoScale::Interact()
	{
		// X-Axis (Line)
		GizmoInteract gizmoInteract { };
		gizmoInteract.hash = GIZMO_HASH_AXIS_X;
		gizmoInteract.type = GizmoInteractType::Line;
		gizmoInteract.onEnter = std::bind(&CGizmoScale::OnEnter, this, std::placeholders::_1);
		gizmoInteract.onExit = std::bind(&CGizmoScale::OnExit, this, std::placeholders::_1);
		gizmoInteract.onPressed = std::bind(&CGizmoScale::OnPressed, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		gizmoInteract.whileHeld = std::bind(&CGizmoScale::WhileHeld, this, std::placeholders::_1, std::placeholders::_2);
		gizmoInteract.onReleased = std::bind(&CGizmoScale::OnReleased, this, std::placeholders::_1);
		gizmoInteract.line.origin = m_data.pGizmoPivot->GetPosition();
		gizmoInteract.line.dir = m_data.pGizmoPivot->GetLocalRight();
		gizmoInteract.line.length = m_data.pGizmoPivot->GetScreenScale() + (m_data.cubeSize * m_data.pGizmoPivot->GetScreenScale());
		gizmoInteract.line.width = 0.015f;

		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);
		
		// Y-Axis (Line)
		gizmoInteract.hash = GIZMO_HASH_AXIS_Y;
		gizmoInteract.line.dir = m_data.pGizmoPivot->GetLocalUp();
		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);
		
		// Z-Axis (Line)
		gizmoInteract.hash = GIZMO_HASH_AXIS_Z;
		gizmoInteract.line.dir = m_data.pGizmoPivot->GetLocalForward();
		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);

		// Center.
		const float centerCubeSize = m_data.centerCubeSize * m_data.pGizmoPivot->GetScreenScale();

		gizmoInteract.hash = GIZMO_HASH_CENTER;
		gizmoInteract.type = GizmoInteractType::Cube;
		gizmoInteract.priority = 1;
		gizmoInteract.cube.origin = m_data.pGizmoPivot->GetPosition();
		gizmoInteract.cube.right = m_data.pGizmoPivot->GetLocalRight();
		gizmoInteract.cube.up = m_data.pGizmoPivot->GetLocalUp();
		gizmoInteract.cube.forward = m_data.pGizmoPivot->GetLocalForward();
		gizmoInteract.cube.size = centerCubeSize;
		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);
	}

	void CGizmoScale::Draw()
	{
		const float cubeSize = m_data.cubeSize * m_data.pGizmoPivot->GetScreenScale();

		// Lambda for drawing an axis for this gizmo.
		auto DrawAxis = [this, &cubeSize](u32 axisIndex, const Math::SIMDVector& normal, const Math::SIMDVector& tangent, const Math::SIMDVector& bitangent, const Math::Color& color) {
			const float scaleAxis = (m_bHovered[axisIndex]) ? m_data.axisHoveredWidth : m_data.axisWidth;
			const Math::Vector3 scaleCube = (m_bHovered[axisIndex]) ? m_data.cubeHoveredScale : 1.0f;
			const Math::Color colorAxis = (m_bClicked[3] || m_bClicked[axisIndex]) ? Math::COLOR_YELLOW : color;

			CEditor::Instance().Nodes().Gizmo().Draw().Line().Draw(m_data.pGizmoPivot->GetPosition(), 0.0f, normal, m_data.pGizmoPivot->GetScreenScale() * m_scale[axisIndex], scaleAxis, colorAxis);
			CEditor::Instance().Nodes().Gizmo().Draw().Cube().Draw(m_data.pGizmoPivot->GetPosition(), normal * m_data.pGizmoPivot->GetScreenScale() * m_scale[axisIndex],
				Math::SIMDQuaternion::RotateDirection(normal, bitangent), scaleCube * cubeSize, colorAxis);
		};

		{ // Center
			const float centerCubeSize = m_data.centerCubeSize * m_data.pGizmoPivot->GetScreenScale();
			const float scaleCube = (m_bHovered[3] ? m_data.cubeHoveredScale : 1.0f) * centerCubeSize;
			const Math::Color colorCube = m_bClicked[3] ? Math::COLOR_YELLOW : Math::COLOR_WHITE;

			CEditor::Instance().Nodes().Gizmo().Draw().Cube().Draw(m_data.pGizmoPivot->GetPosition(), -m_data.pGizmoPivot->GetLocalUp() * scaleCube * 0.5f,
				Math::SIMDQuaternion::RotateDirection(m_data.pGizmoPivot->GetLocalUp(), m_data.pGizmoPivot->GetLocalForward()), scaleCube, colorCube);
		}

		// X-Axis
		DrawAxis(GizmoIndex(GIZMO_HASH_AXIS_X), m_data.pGizmoPivot->GetLocalRight(), m_data.pGizmoPivot->GetLocalForward(), m_data.pGizmoPivot->GetLocalUp(), Math::COLOR_RED);

		// Y-Axis
		DrawAxis(GizmoIndex(GIZMO_HASH_AXIS_Y), m_data.pGizmoPivot->GetLocalUp(), m_data.pGizmoPivot->GetLocalRight(), m_data.pGizmoPivot->GetLocalForward(), Math::COLOR_GREEN);

		// Z-Axis
		DrawAxis(GizmoIndex(GIZMO_HASH_AXIS_Z), m_data.pGizmoPivot->GetLocalForward(), m_data.pGizmoPivot->GetLocalRight(), m_data.pGizmoPivot->GetLocalUp(), Math::COLOR_BLUE);
	}

	//-----------------------------------------------------------------------------------------------
	// Interaction methods.
	//-----------------------------------------------------------------------------------------------

	void CGizmoScale::OnEnter(u32 hash)
	{
		m_bHovered[GizmoIndex(hash)] = true;
	}

	void CGizmoScale::OnExit(u32 hash)
	{
		m_bHovered[GizmoIndex(hash)] = false;
	}
	
	void CGizmoScale::OnPressed(u32 hash, const Math::CSIMDRay& ray, float t)
	{
		m_bClicked[GizmoIndex(hash)] = true;

		m_data.pGizmoPivot->GatherStart();
		m_scaleNormal = ray.GetDirection();
		m_scaleTangent = Math::SIMDVector::Cross(App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetUp(), m_scaleNormal).Normalized();
		m_scaleBitangent = Math::SIMDVector::Cross(m_scaleNormal, m_scaleTangent).Normalized();
		
		Math::SIMDVector outPt;
		CalculatePoint(hash, ray, outPt);
		m_scale = m_startScale = 1.0f;

		m_clickPoint = outPt;
	}
	
	void CGizmoScale::WhileHeld(u32 hash, const Math::CSIMDRay& ray)
	{
		Math::SIMDVector outPt;
		if(CalculatePoint(hash, ray, outPt))
		{
			const float scale = m_data.pGizmoPivot->CalculateDistanceScale(m_data.pGizmoPivot->GetPosition());
			if(hash == GIZMO_HASH_CENTER)
			{
				float localScaleFlt =
					(_mm_cvtss_f32(Math::SIMDVector::Dot(outPt, m_scaleTangent))) +
					(_mm_cvtss_f32(Math::SIMDVector::Dot(outPt, m_scaleBitangent)));

				Math::SIMDVector scaleVec = Math::SIMDVector::PointwiseProduct(localScaleFlt, m_data.pGizmoPivot->GetLastScale());

				if(m_data.snap > 0.0f)
				{
					const vf32 snapVec = _mm_set_ps1(m_data.snap);
					scaleVec = _mm_mul_ps(_mm_round_ps(_mm_div_ps(scaleVec.m_xmm, snapVec), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC), snapVec);
				}

				m_data.pGizmoPivot->SetScale(_mm_max_ps((m_data.pGizmoPivot->GetLastScale() + scaleVec).m_xmm, _mm_setr_ps(1.0f, 1.0f, 1.0f, 0.0f)));

				m_scale = m_startScale + *(Math::Vector3*)Math::SIMDVector::PointwiseQuotient(scaleVec, m_data.pGizmoPivot->GetLastScale()).ToFloat() / (scale * m_data.pGizmoPivot->GetScreenScale());
			}
			else
			{
				Math::Vector3 localScale(
					(_mm_cvtss_f32(Math::SIMDVector::Dot(outPt, m_data.pGizmoPivot->GetLocalRight()))),
					(_mm_cvtss_f32(Math::SIMDVector::Dot(outPt, m_data.pGizmoPivot->GetLocalUp()))),
					(_mm_cvtss_f32(Math::SIMDVector::Dot(outPt, m_data.pGizmoPivot->GetLocalForward())))
				);

				Math::SIMDVector scaleVec = Math::SIMDVector::PointwiseProduct(localScale, m_data.pGizmoPivot->GetLastScale());
				
				if(m_data.snap > 0.0f)
				{
					const vf32 snapVec = _mm_set_ps1(m_data.snap);
					scaleVec = _mm_mul_ps(_mm_round_ps(_mm_div_ps(scaleVec.m_xmm, snapVec), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC), snapVec);
				}

				m_data.pGizmoPivot->SetScale(_mm_max_ps((m_data.pGizmoPivot->GetLastScale() + scaleVec).m_xmm, _mm_setr_ps(1.0f, 1.0f, 1.0f, 0.0f)));

				m_scale = m_startScale + *(Math::Vector3*)Math::SIMDVector::PointwiseQuotient(scaleVec, m_data.pGizmoPivot->GetLastScale()).ToFloat() / (scale * m_data.pGizmoPivot->GetScreenScale());
			}
		}
	}

	void CGizmoScale::OnReleased(u32 hash)
	{
		Math::SIMDVector scale = m_data.pGizmoPivot->GetScale();
		App::CCommandManager::Instance().Execute(CGizmoPivot::CMD_KEY_SCALE, &scale);

		m_scale = m_startScale = 1.0f;
		m_clickPoint = 0;
		m_bClicked[GizmoIndex(hash)] = false;
	}

	//-----------------------------------------------------------------------------------------------
	// Intersection methods.
	//-----------------------------------------------------------------------------------------------

	bool CGizmoScale::TranslateAxis(const Math::SIMDVector& axis, const Math::CSIMDRay& ray, Math::SIMDVector& outPt)
	{
		float t0;
		if(CGizmoInteract::EdgePoint(m_data.pGizmoPivot->GetLastPosition(), axis, ray, t0))
		{
			const float scale = m_data.pGizmoPivot->CalculateDistanceScale(m_data.pGizmoPivot->GetPosition());
			const float bias = (scale - m_data.pGizmoPivot->GetStartScale()) * m_data.pGizmoPivot->GetScreenScale();

			auto pt = ray.GetOrigin() + ray.GetDirection() * t0;
			outPt = ((axis * (_mm_cvtss_f32(Math::SIMDVector::Dot(axis, pt)) - bias)) - m_clickPoint);
			return true;
		}

		return false;
	}

	bool CGizmoScale::TranslatePlane(const Math::SIMDVector& normal, const Math::CSIMDRay& ray, Math::SIMDVector& outPt)
	{
		float t0;
		if(CGizmoInteract::PlanePoint(m_data.pGizmoPivot->GetLastPosition(), -normal, ray, t0))
		{
			outPt = ray.GetOrigin() + ray.GetDirection() * t0 - m_clickPoint;
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	bool CGizmoScale::CalculatePoint(u32 hash, const Math::CSIMDRay& ray, Math::SIMDVector& outPt)
	{
		bool bHit = false;
		switch(hash)
		{
			case GIZMO_HASH_AXIS_X:
				bHit |= TranslateAxis(m_data.pGizmoPivot->GetLocalRight(), ray, outPt);
				break;
			case GIZMO_HASH_AXIS_Y:
				bHit |= TranslateAxis(m_data.pGizmoPivot->GetLocalUp(), ray, outPt);
				break;
			case GIZMO_HASH_AXIS_Z:
				bHit |= TranslateAxis(m_data.pGizmoPivot->GetLocalForward(), ray, outPt);
				break;
			case GIZMO_HASH_CENTER:
				bHit |= TranslatePlane(m_scaleNormal, ray, outPt);
				break;
		}

		return bHit;
	}
};
