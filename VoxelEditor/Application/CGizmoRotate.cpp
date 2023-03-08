//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoRotate.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmoRotate.h"
#include "CGizmoNode.h"
#include "CEditor.h"
#include <Application/CCommandManager.h>
#include <Graphics/CGraphicsAPI.h>
#include <Factory/CFactory.h>
#include <Application/CSceneManager.h>
#include <unordered_map>
#include <Windows.h>

namespace App
{
	static u32 GizmoIndex(u32 hash)
	{
		static const std::unordered_map<u32, u32> GIZMO_HASH_MAP = {
			{ CGizmoRotate::GIZMO_HASH_AXIS_X, 0 },
			{ CGizmoRotate::GIZMO_HASH_AXIS_Y, 1 },
			{ CGizmoRotate::GIZMO_HASH_AXIS_Z, 2 },
			{ CGizmoRotate::GIZMO_HASH_EDGE, 3 },
			{ CGizmoRotate::GIZMO_HASH_CENTER, 4 },
		};

		return GIZMO_HASH_MAP.find(hash)->second;
	}

	CGizmoRotate::CGizmoRotate() : 
		m_clickPoint(0)
	{
		memset(m_bHovered, 0, sizeof(m_bHovered));
		memset(m_bClicked, 0, sizeof(m_bClicked));
	}

	CGizmoRotate::~CGizmoRotate()
	{
	}

	void CGizmoRotate::Interact()
	{
		auto pos = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition();
		auto diff = pos - m_data.pGizmoPivot->GetPosition();
		m_viewDir = diff.Normalized();

		const float sphereSize = m_data.sphereSize * m_data.pGizmoPivot->GetScreenScale() * 0.5f;
		const float sphereOuterSize = m_data.sphereOuterSize * m_data.pGizmoPivot->GetScreenScale() * 0.5f;
		
		// X-Axis (Circle)
		GizmoInteract gizmoInteract { };
		gizmoInteract.hash = GIZMO_HASH_AXIS_X;
		gizmoInteract.type = GizmoInteractType::Circle;
		gizmoInteract.onEnter = std::bind(&CGizmoRotate::OnEnter, this, std::placeholders::_1);
		gizmoInteract.onExit = std::bind(&CGizmoRotate::OnExit, this, std::placeholders::_1);
		gizmoInteract.onPressed = std::bind(&CGizmoRotate::OnPressed, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		gizmoInteract.whileHeld = std::bind(&CGizmoRotate::WhileHeld, this, std::placeholders::_1, std::placeholders::_2);
		gizmoInteract.onReleased = std::bind(&CGizmoRotate::OnReleased, this, std::placeholders::_1);
		gizmoInteract.circle.origin = m_data.pGizmoPivot->GetPosition();
		gizmoInteract.circle.normal = m_data.pGizmoPivot->GetLocalRight();
		gizmoInteract.circle.bitangent = m_viewDir;
		gizmoInteract.circle.width = 0.02f;
		gizmoInteract.circle.radius = sphereSize;
		gizmoInteract.circle.bHalf = true;

		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);
		
		gizmoInteract.hash = GIZMO_HASH_AXIS_Y;
		gizmoInteract.circle.normal = m_data.pGizmoPivot->GetLocalUp();

		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);
		
		gizmoInteract.hash = GIZMO_HASH_AXIS_Z;
		gizmoInteract.circle.normal = m_data.pGizmoPivot->GetLocalForward();

		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);
		
		gizmoInteract.hash = GIZMO_HASH_EDGE;
		gizmoInteract.circle.normal = m_viewDir;
		gizmoInteract.circle.bitangent = Math::SIMD_VEC_UP;
		gizmoInteract.circle.radius = sphereOuterSize;
		gizmoInteract.circle.bHalf = false;

		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);
		
		gizmoInteract.hash = GIZMO_HASH_CENTER;
		gizmoInteract.type = GizmoInteractType::Sphere;
		gizmoInteract.sphere.radius = sphereSize;

		CEditor::Instance().Nodes().Gizmo().Interact().AddInteractable(gizmoInteract);
	}

	void CGizmoRotate::Draw()
	{
		auto pos = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition();
		auto diff = pos - m_data.pGizmoPivot->GetPosition();
		
		const float sphereSize = m_data.sphereSize * m_data.pGizmoPivot->GetScreenScale();
		const float sphereOuterSize = m_data.sphereOuterSize * m_data.pGizmoPivot->GetScreenScale();

		auto DrawAxis = [this, &sphereSize](u32 axis, const Math::SIMDVector& forward, const Math::SIMDVector& up, const Math::Color& color)
		{
			const float w = m_bHovered[axis] ? 3.0f : 2.0f;

			if(m_bClicked[axis])
			{
				CEditor::Instance().Nodes().Gizmo().Draw().Circle().Draw(m_data.pGizmoPivot->GetPosition(), 0.0f, Math::SIMDQuaternion::RotateDirection(forward, up), sphereSize, w, Math::COLOR_YELLOW);
			}
			else
			{
				CEditor::Instance().Nodes().Gizmo().Draw().HalfCircle().Draw(m_data.pGizmoPivot->GetPosition(), 0.0f, Math::SIMDQuaternion::RotateDirection(forward, up), sphereSize, w, color);
			}
		};

		DrawAxis(0, m_data.pGizmoPivot->GetLocalRight(), m_viewDir, Math::COLOR_RED);
		DrawAxis(1, m_data.pGizmoPivot->GetLocalUp(), m_viewDir, Math::COLOR_GREEN);
		DrawAxis(2, m_data.pGizmoPivot->GetLocalForward(), m_viewDir, Math::COLOR_BLUE);
		
		
		const float wE = m_bHovered[3] ? 3.0f : 2.0f;
		const Math::Color cE = m_bClicked[3] ? Math::COLOR_YELLOW : Math::Color(0.75f, 1.0f);
		
		CEditor::Instance().Nodes().Gizmo().Draw().Circle().Draw(m_data.pGizmoPivot->GetPosition(), 0.0f, Math::SIMDQuaternion::RotateDirection(m_viewDir), sphereOuterSize, wE, cE);
		
		const float wC = m_bHovered[4] ? 1.0f : 1.0f;
		const Math::Color cC = m_bClicked[4] ? Math::COLOR_YELLOW : Math::Color(0.5f, 1.0f);

		CEditor::Instance().Nodes().Gizmo().Draw().Circle().Draw(m_data.pGizmoPivot->GetPosition(), 0.0f, Math::SIMDQuaternion::RotateDirection(m_viewDir), sphereSize, wC, cC);
	}

	//-----------------------------------------------------------------------------------------------
	// Interaction methods.
	//-----------------------------------------------------------------------------------------------

	void CGizmoRotate::OnEnter(u32 hash)
	{
		m_bHovered[GizmoIndex(hash)] = true;
	}

	void CGizmoRotate::OnExit(u32 hash)
	{
		m_bHovered[GizmoIndex(hash)] = false;
	}
	
	void CGizmoRotate::OnPressed(u32 hash, const Math::CSIMDRay& ray, float t)
	{
		m_bClicked[GizmoIndex(hash)] = true;
		m_data.pGizmoPivot->GatherStart();
		
		m_clickPoint = ray.GetOrigin() + ray.GetDirection() * t;
		m_normal = -App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetForward();
		
		switch(hash)
		{
			case GIZMO_HASH_AXIS_X:
				m_bitangent = m_data.pGizmoPivot->GetLocalRight();
				m_tangent = Math::SIMDVector::Cross(m_bitangent, m_normal).Normalized();
				break;
			case GIZMO_HASH_AXIS_Y:
				m_bitangent = m_data.pGizmoPivot->GetLocalUp();
				m_tangent = Math::SIMDVector::Cross(m_bitangent, m_normal).Normalized();
				break;
			case GIZMO_HASH_AXIS_Z:
				m_bitangent = m_data.pGizmoPivot->GetLocalForward();
				m_tangent = Math::SIMDVector::Cross(m_bitangent, m_normal).Normalized();
				break;
			case GIZMO_HASH_EDGE:
				m_bitangent = -m_normal;
				m_tangent = Math::SIMDVector::Cross((m_clickPoint - m_data.pGizmoPivot->GetPosition()), m_normal).Normalized();
				break;
			case GIZMO_HASH_CENTER:
				m_tangent = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetRight();
				m_bitangent = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetUp();
				break;
			default:
				break;
		}
	}

	void CGizmoRotate::WhileHeld(u32 hash, const Math::CSIMDRay& ray)
	{
		const float scale = m_data.pGizmoPivot->GetStartScale();//m_data.pGizmoPivot->CalculateDistanceScale(m_data.pGizmoPivot->GetPosition());
		const float invScreenScale = 1.0f / (scale * m_data.pGizmoPivot->GetScreenScale());
			float t;

		if(hash == GIZMO_HASH_CENTER)
		{
			if(CGizmoInteract::PlanePoint(m_clickPoint, m_normal, ray, t))
			{
				const auto planePt = ray.GetOrigin() + ray.GetDirection() * t - m_clickPoint;
				float deltaX = _mm_cvtss_f32(Math::SIMDVector::Dot(planePt, m_tangent)) * invScreenScale;
				float deltaY = _mm_cvtss_f32(Math::SIMDVector::Dot(planePt, m_bitangent)) * invScreenScale;
				
				if(m_data.snap > 0.0f)
				{
					deltaX = roundf(deltaX / m_data.snap) * m_data.snap;
					deltaY = roundf(deltaY / m_data.snap) * m_data.snap;
				}

				m_rotation = Math::SIMDQuaternion::RotateAxis(m_tangent, Math::WrapRadians(deltaY)) * Math::SIMDQuaternion::RotateAxis(m_bitangent, Math::WrapRadians(-deltaX));
			}
		}
		else
		{
			if(CGizmoInteract::PlanePoint(m_clickPoint, m_normal, ray, t))
			{
				const auto planePt = ray.GetOrigin() + ray.GetDirection() * t - m_clickPoint;
				float delta = _mm_cvtss_f32(Math::SIMDVector::Dot(planePt, m_tangent)) * invScreenScale;

				if(m_data.snap > 0.0f)
				{
					delta = roundf(delta / m_data.snap) * m_data.snap;
				}

				m_rotation = Math::SIMDQuaternion::RotateAxis(m_bitangent, Math::WrapRadians(delta));
			}
		}

		m_data.pGizmoPivot->SetRotation(m_rotation * m_data.pGizmoPivot->GetLastRotation());
	}

	void CGizmoRotate::OnReleased(u32 hash)
	{
		Math::SIMDQuaternion rotation = m_data.pGizmoPivot->GetRotation();
		App::CCommandManager::Instance().Execute(CGizmoNode::CMD_KEY_ROTATE, &rotation);

		m_rotation = Math::SIMD_QUAT_IDENTITY;
		m_clickPoint = 0;

		m_bClicked[GizmoIndex(hash)] = false;
	}
};

