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
#include <Application/CCommandManager.h>
#include <Application/CSceneManager.h>
#include <Factory/CFactory.h>
#include <Graphics/CGraphicsAPI.h>

namespace App
{
	CGizmoPivot::CGizmoPivot() :
		m_screenScale(0.0f),
		m_startScale(0.0f),
		m_lastPosition(0.0f),
		m_lastRotation(0.0f),
		m_lastScale(0.0f),
		m_pNode(nullptr)
	{
	}

	CGizmoPivot::~CGizmoPivot()
	{
	}

	void CGizmoPivot::Initialize()
	{
		{ // Setup translation gizmo.
			CGizmoTranslate::Data data { };
			data.pGizmoPivot = this;
			m_gizmoTranslate.SetData(data);
		}
		
		{ // Setup rotate gizmo.
			CGizmoRotate::Data data { };
			data.pGizmoPivot = this;
			m_gizmoRotate.SetData(data);
		}
		
		{ // Setup scale gizmo.
			CGizmoScale::Data data { };
			data.pGizmoPivot = this;
			m_gizmoScale.SetData(data);
		}

		{ // Setup commands.
			App::CCommand::Properties props = { };
			props.bSaveEnabled = true;
			props.bUndoEnabled = true;

			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_SELECT, std::bind(&CGizmoPivot::GizmoSelect, this));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_TRANSLATE, std::bind(&CGizmoPivot::GizmoTranslate, this));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_ROTATE, std::bind(&CGizmoPivot::GizmoRotate, this));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_SCALE, std::bind(&CGizmoPivot::GizmoScale, this));

			CCommandManager::Instance().RegisterCommand(CMD_KEY_TRANSLATE, std::bind(&CGizmoPivot::Translate, this, std::placeholders::_1, std::placeholders::_2), props);
			CCommandManager::Instance().RegisterCommand(CMD_KEY_ROTATE, std::bind(&CGizmoPivot::Rotate, this, std::placeholders::_1, std::placeholders::_2), props);
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SCALE, std::bind(&CGizmoPivot::Scale, this, std::placeholders::_1, std::placeholders::_2), props);
			
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_SELECT_OFF, [](const void* params, bool bInverse){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_TRANSLATE_OFF, [](const void* params, bool bInverse){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_ROTATE_OFF, [](const void* params, bool bInverse){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_GIZMO_SCALE_OFF, [](const void* params, bool bInverse){ return true; });
		}
	}

	void CGizmoPivot::Update()
	{
		CalculateScreenScale();

		m_right = Math::SIMD_VEC_RIGHT;
		m_up = Math::SIMD_VEC_UP;
		m_forward = Math::SIMD_VEC_FORWARD;
		
		switch(m_gizmoType)
		{
			case GizmoType::Translate:
				m_gizmoTranslate.Interact();
				break;
			case GizmoType::Rotate:
				m_gizmoRotate.Interact();
				break;
			case GizmoType::Scale:
				m_gizmoScale.Interact();
				break;
			default:
				break;
		}
	}

	void CGizmoPivot::LateUpdate()
	{
		m_rightLocal = m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetRight();
		m_upLocal = m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetUp();
		m_forwardLocal = m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->GetForward();
		
		switch(m_gizmoType)
		{
			case GizmoType::Translate:
				m_gizmoTranslate.Draw();
				break;
			case GizmoType::Rotate:
				m_gizmoRotate.Draw();
				break;
			case GizmoType::Scale:
				m_gizmoScale.Draw();
				break;
			default:
				break;
		}
	}

	void CGizmoPivot::Release()
	{
	}

	//-----------------------------------------------------------------------------------------------
	// Command methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CGizmoPivot::GizmoSelect()
	{
		SwitchGizmo(GizmoType::Select);
		return true;
	}
	
	bool CGizmoPivot::GizmoTranslate()
	{
		SwitchGizmo(GizmoType::Translate);
		return true;
	}

	bool CGizmoPivot::GizmoRotate()
	{
		SwitchGizmo(GizmoType::Rotate);
		return true;
	}

	bool CGizmoPivot::GizmoScale()
	{
		SwitchGizmo(GizmoType::Scale);
		return true;
	}

	bool CGizmoPivot::Translate(const void* param, bool bInverse)
	{
		Math::SIMDVector position = *(Math::SIMDVector*)param;
		if(position == m_lastPosition) return false;

		if(!bInverse)
		{
			App::CCommandManager::Instance().RecordUndo(&m_lastPosition, sizeof(m_lastPosition), alignof(Math::SIMDVector));
		}

		m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->SetPosition(position);
		
		m_lastPosition = position;
		m_startScale = CalculateDistanceScale(m_lastPosition);
		
		return true;
	}

	bool CGizmoPivot::Rotate(const void* param, bool bInverse)
	{
		Math::SIMDQuaternion rotation = *(Math::SIMDQuaternion*)param;
		if(rotation == m_lastRotation) return false;

		if(!bInverse)
		{
			App::CCommandManager::Instance().RecordUndo(&m_lastRotation, sizeof(m_lastRotation), alignof(Math::SIMDQuaternion));
		}
		
		m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->SetRotation(rotation);
		m_lastRotation = rotation;

		return true;
	}

	bool CGizmoPivot::Scale(const void* param, bool bInverse)
	{
		Math::SIMDVector scale = *(Math::SIMDVector*)param;
		if(scale == m_lastScale) return false;

		if(!bInverse)
		{
			App::CCommandManager::Instance().RecordUndo(&m_lastScale, sizeof(m_lastScale), alignof(Math::SIMDVector));
		}
		
		m_pNode->GetSelectedObject()->GetComponent<Logic::CNodeTransform>()->SetScale(scale);
		m_lastScale = scale;

		return true;
	}

	void CGizmoPivot::SwitchGizmo(GizmoType next)
	{
		switch(m_gizmoType)
		{
			case GizmoType::Select:
				if(next != GizmoType::Select)
					App::CCommandManager::Instance().Execute(CMD_KEY_GIZMO_SELECT_OFF);
				break;
			case GizmoType::Translate:
				if(next != GizmoType::Translate)
					App::CCommandManager::Instance().Execute(CMD_KEY_GIZMO_TRANSLATE_OFF);
				break;
			case GizmoType::Rotate:
				if(next != GizmoType::Rotate)
					App::CCommandManager::Instance().Execute(CMD_KEY_GIZMO_ROTATE_OFF);
				break;
			case GizmoType::Scale:
				if(next != GizmoType::Scale)
					App::CCommandManager::Instance().Execute(CMD_KEY_GIZMO_SCALE_OFF);
				break;
			default:
				break;
		}

		m_gizmoType = next;
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

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
