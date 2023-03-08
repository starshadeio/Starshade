//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoWorkspace.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmoChunkGrid.h"
#include "CGizmoPivot.h"
#include "CGizmoWorkspace.h"
#include "CEditor.h"
#include "../Universe/CChunkGrid.h"
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
			{ CGizmoChunkGrid::GIZMO_HASH_SIDE_LEFT, Universe::SIDE_LEFT },
			{ CGizmoChunkGrid::GIZMO_HASH_SIDE_RIGHT, Universe::SIDE_RIGHT },
			{ CGizmoChunkGrid::GIZMO_HASH_SIDE_BOTTOM, Universe::SIDE_BOTTOM },
			{ CGizmoChunkGrid::GIZMO_HASH_SIDE_TOP, Universe::SIDE_TOP },
			{ CGizmoChunkGrid::GIZMO_HASH_SIDE_BACK, Universe::SIDE_BACK },
			{ CGizmoChunkGrid::GIZMO_HASH_SIDE_FRONT, Universe::SIDE_FRONT },
		};

		return GIZMO_HASH_MAP.find(hash)->second;
	}

	CGizmoChunkGrid::CGizmoChunkGrid()
	{
	}

	CGizmoChunkGrid::~CGizmoChunkGrid()
	{
	}
	
	void CGizmoChunkGrid::Initialize()
	{
		{ // Setup translation gizmo.
			CGizmoTranslate::Data data { };
			data.snap = m_data.snap;
			data.onReleaseHash = CGizmoWorkspace::CMD_KEY_TRANSLATE;
			data.pGizmoPivot = m_data.pGizmoPivot;
			m_gizmoTranslate.SetData(data);
		}
	}
	
	void CGizmoChunkGrid::Interact()
	{
		GizmoInteract gizmoInteract { };
		gizmoInteract.type = GizmoInteractType::Cube;
		gizmoInteract.onEnter = std::bind(&CGizmoChunkGrid::OnEnter, this, std::placeholders::_1);
		gizmoInteract.onExit = std::bind(&CGizmoChunkGrid::OnExit, this, std::placeholders::_1);
		gizmoInteract.onPressed = std::bind(&CGizmoChunkGrid::OnPressed, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		gizmoInteract.whileHeld = std::bind(&CGizmoChunkGrid::WhileHeld, this, std::placeholders::_1, std::placeholders::_2);
		gizmoInteract.onReleased = std::bind(&CGizmoChunkGrid::OnReleased, this, std::placeholders::_1);
		
		// Lambda for drawing an axis for this gizmo.
		auto InteractSide = [&](u32 hash, u32 axisIndex, const Math::SIMDVector& normal, const Math::SIMDVector& tangent, const Math::SIMDVector& bitangent) {
			gizmoInteract.hash = hash;
			
			gizmoInteract.cube.size = m_data.cubeSize * m_data.pGizmoPivot->GetScreenScale();
			gizmoInteract.cube.origin = normal * fabsf(_mm_cvtss_f32(Math::SIMDVector::Dot(normal, m_pChunkGrid->GetScale()))) * 0.5f + m_pChunkGrid->GetOffset();

			gizmoInteract.cube.origin += normal * fabsf(_mm_cvtss_f32(Math::SIMDVector::Dot(normal, gizmoInteract.cube.size * m_data.pGizmoPivot->CalculateDistanceScale(gizmoInteract.cube.origin)))) * 0.5f;

			gizmoInteract.cube.right = tangent;
			gizmoInteract.cube.up = bitangent;
			gizmoInteract.cube.forward = normal;
			
			CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);
		};

		InteractSide(GIZMO_HASH_SIDE_LEFT, Universe::SIDE_LEFT, Math::SIMD_VEC_LEFT, Math::SIMD_VEC_BACKWARD, Math::SIMD_VEC_UP);
		InteractSide(GIZMO_HASH_SIDE_RIGHT, Universe::SIDE_RIGHT, Math::SIMD_VEC_RIGHT, Math::SIMD_VEC_FORWARD, Math::SIMD_VEC_UP);
		InteractSide(GIZMO_HASH_SIDE_BOTTOM, Universe::SIDE_BOTTOM, Math::SIMD_VEC_DOWN, Math::SIMD_VEC_RIGHT, Math::SIMD_VEC_BACKWARD);
		InteractSide(GIZMO_HASH_SIDE_TOP, Universe::SIDE_TOP, Math::SIMD_VEC_UP, Math::SIMD_VEC_RIGHT, Math::SIMD_VEC_FORWARD);
		InteractSide(GIZMO_HASH_SIDE_BACK, Universe::SIDE_BACK, Math::SIMD_VEC_BACKWARD, Math::SIMD_VEC_RIGHT, Math::SIMD_VEC_UP);
		InteractSide(GIZMO_HASH_SIDE_FRONT, Universe::SIDE_FRONT, Math::SIMD_VEC_FORWARD, Math::SIMD_VEC_LEFT, Math::SIMD_VEC_UP);

		m_gizmoTranslate.Interact();
	}
	
	void CGizmoChunkGrid::Draw()
	{
		const float cubeSize = m_data.cubeSize * m_data.pGizmoPivot->GetScreenScale();

		// Lambda for drawing an axis for this gizmo.
		auto DrawSide = [&](u32 axisIndex, const Math::SIMDVector& normal, const Math::SIMDVector& bitangent, const Math::Color& color) {
			const Math::Vector3 scaleCube = (m_bHovered[axisIndex]) ? m_data.cubeHoveredScale : 1.0f;
			const Math::Color colorAxis = m_bClicked[axisIndex] ? Math::COLOR_YELLOW : color;

			CEditor::Instance().Nodes().Gizmo().Draw().Cube().Draw(normal * fabsf(_mm_cvtss_f32(Math::SIMDVector::Dot(normal, m_pChunkGrid->GetScale()))) * 0.5f + m_pChunkGrid->GetOffset(), 0.0f,
				Math::SIMDQuaternion::RotateDirection(normal, bitangent), scaleCube * cubeSize, colorAxis);
		};

		Math::Color color = Math::Color(0.3333f, 1.0f);

		DrawSide(Universe::SIDE_LEFT, Math::SIMD_VEC_LEFT, Math::SIMD_VEC_UP, color);
		DrawSide(Universe::SIDE_RIGHT, Math::SIMD_VEC_RIGHT, Math::SIMD_VEC_UP, color);
		DrawSide(Universe::SIDE_BOTTOM, Math::SIMD_VEC_DOWN, Math::SIMD_VEC_BACKWARD, color);
		DrawSide(Universe::SIDE_TOP, Math::SIMD_VEC_UP, Math::SIMD_VEC_FORWARD, color);
		DrawSide(Universe::SIDE_BACK, Math::SIMD_VEC_BACKWARD, Math::SIMD_VEC_UP, color);
		DrawSide(Universe::SIDE_FRONT, Math::SIMD_VEC_FORWARD, Math::SIMD_VEC_UP, color);

		m_gizmoTranslate.Draw();
	}

	//-----------------------------------------------------------------------------------------------
	// Interaction methods.
	//-----------------------------------------------------------------------------------------------

	void CGizmoChunkGrid::OnEnter(u32 hash)
	{
		m_bHovered[GizmoIndex(hash)] = true;
	}

	void CGizmoChunkGrid::OnExit(u32 hash)
	{
		m_bHovered[GizmoIndex(hash)] = false;
	}
	
	void CGizmoChunkGrid::OnPressed(u32 hash, const Math::CSIMDRay& ray, float t)
	{
		m_bClicked[GizmoIndex(hash)] = true;

		GatherExtents();
		
		Math::SIMDVector outPt;
		CalculatePoint(hash, ray, outPt);
		m_clickPoint = outPt;
	}
	
	void CGizmoChunkGrid::WhileHeld(u32 hash, const Math::CSIMDRay& ray)
	{
		Math::SIMDVector outPt;
		if(CalculatePoint(hash, ray, outPt))
		{
			if(m_data.snap > 0.0f)
			{
				const vf32 snapVec = _mm_set_ps1(m_data.snap);
				outPt = _mm_mul_ps(_mm_round_ps(_mm_div_ps(outPt.m_xmm, snapVec), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC), snapVec);
			}

			switch(GizmoIndex(hash))
			{
				case Universe::SIDE_LEFT:
				case Universe::SIDE_BOTTOM:
				case Universe::SIDE_BACK:
					m_minExtents = m_lastMinExtents + *(Math::Vector3*)outPt.ToFloat();
					for(u32 i = 0; i < 3; ++i)
					{
						m_minExtents[i] += std::max(0.0f, (m_maxExtents[i] - m_minExtents[i]) - 32.0f);
						m_minExtents[i] -= std::max(0.0f, 0.25f - (m_maxExtents[i] - m_minExtents[i]));
					}
					break;
				case Universe::SIDE_RIGHT:
				case Universe::SIDE_TOP:
				case Universe::SIDE_FRONT:
					m_maxExtents = m_lastMaxExtents + *(Math::Vector3*)outPt.ToFloat();
					for(u32 i = 0; i < 3; ++i)
					{
						m_maxExtents[i] -= std::max(0.0f, (m_maxExtents[i] - m_minExtents[i]) - 32.0f);
						m_maxExtents[i] += std::max(0.0f, 0.25f - (m_maxExtents[i] - m_minExtents[i]));
					}
					break;
				default:
					break;
			}

			m_pChunkGrid->SetExtents(m_minExtents, m_maxExtents);
		}
	}

	void CGizmoChunkGrid::OnReleased(u32 hash)
	{
		std::pair<Math::Vector3, Math::Vector3> extents = { m_minExtents, m_maxExtents };

		App::CCommandManager::Instance().Execute(CGizmoWorkspace::CMD_KEY_EXTENTS, &extents);

		m_clickPoint = 0;
		m_bClicked[GizmoIndex(hash)] = false;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utilities methods.
	//-----------------------------------------------------------------------------------------------
	
		
	bool CGizmoChunkGrid::TranslateAxis(const Math::SIMDVector& axis, const Math::CSIMDRay& ray, Math::SIMDVector& outPt)
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

	bool CGizmoChunkGrid::CalculatePoint(u32 hash, const Math::CSIMDRay& ray, Math::SIMDVector& outPt)
	{
		bool bHit = false;
		switch(hash)
		{
			case GIZMO_HASH_SIDE_LEFT:
				bHit |= TranslateAxis(m_data.pGizmoPivot->GetLocalRight(), ray, outPt);
				break;
			case GIZMO_HASH_SIDE_RIGHT:
				bHit |= TranslateAxis(m_data.pGizmoPivot->GetLocalRight(), ray, outPt);
				break;
			case GIZMO_HASH_SIDE_BOTTOM:
				bHit |= TranslateAxis(m_data.pGizmoPivot->GetLocalUp(), ray, outPt);
				break;
			case GIZMO_HASH_SIDE_TOP:
				bHit |= TranslateAxis(m_data.pGizmoPivot->GetLocalUp(), ray, outPt);
				break;
			case GIZMO_HASH_SIDE_BACK:
				bHit |= TranslateAxis(m_data.pGizmoPivot->GetLocalForward(), ray, outPt);
				break;
			case GIZMO_HASH_SIDE_FRONT:
				bHit |= TranslateAxis(m_data.pGizmoPivot->GetLocalForward(), ray, outPt);
				break;
		}

		return bHit;
	}

	void CGizmoChunkGrid::GatherExtents()
	{
		m_data.pGizmoPivot->GatherStart();

		auto halfScale = m_pChunkGrid->GetScale() * 0.5f;
		auto position = *(Math::Vector3*)m_pChunkGrid->GetOffset().ToFloat();

		m_lastMinExtents = m_minExtents = position - halfScale;
		m_lastMaxExtents = m_maxExtents = position + halfScale;
	}
};