//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoDraw.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGizmoDraw.h"
#include "CAppData.h"
#include <Graphics/CMaterial.h>
#include <Graphics/CShader.h>
#include <Graphics/CRootSignature.h>
#include <Resources/CResourceManager.h>
#include <Application/CSceneManager.h>

namespace App
{
	CGizmoDraw::CGizmoDraw() : 
		CVObject(L"Gizmo Draw", App::VIEW_HASH_OVERLAY),
		m_gizmoLine(this),
		m_gizmoCircle(this),
		m_gizmoHalfCircle(this),
		m_gizmoQuad(this),
		m_gizmoCube(this),
		m_gizmoCone(this),
		m_pMaterialLine(nullptr),
		m_pMaterialTri(nullptr),
		m_pMaterialTriUnit(nullptr)
	{
	}

	CGizmoDraw::~CGizmoDraw()
	{
	}
	
	void CGizmoDraw::Initialize()
	{
		{ // Setup gizmo line.
			CGizmoLine::Data data { };
			data.instanceMax = 16;
			data.matHash = Math::FNV1a_64(L"MATERIAL_GIZMO_LINE");
			m_gizmoLine.SetData(data);
			m_gizmoLine.Initialize();
		}

		{ // Setup gizmo circle.
			CGizmoCircle::Data data { };
			data.instanceMax = 16;
			data.matHash = Math::FNV1a_64(L"MATERIAL_GIZMO_LINE");
			m_gizmoCircle.SetData(data);
			m_gizmoCircle.Initialize();
		}
		
		{ // Setup gizmo half circle.
			CGizmoCircle::Data data { };
			data.bHalfCircle = true;
			data.instanceMax = 16;
			data.matHash = Math::FNV1a_64(L"MATERIAL_GIZMO_LINE");
			m_gizmoHalfCircle.SetData(data);
			m_gizmoHalfCircle.Initialize();
		}
		
		{ // Setup gizmo quad.
			CGizmoQuad::Data data { };
			data.instanceMax = 16;
			data.matHash = Math::FNV1a_64(L"MATERIAL_GIZMO_TRIANGLE_UNLIT");
			m_gizmoQuad.SetData(data);
			m_gizmoQuad.Initialize();
		}
		
		{ // Setup gizmo cube.
			CGizmoCube::Data data { };
			data.instanceMax = 16;
			data.matHash = Math::FNV1a_64(L"MATERIAL_GIZMO_TRIANGLE");
			m_gizmoCube.SetData(data);
			m_gizmoCube.Initialize();
		}
		
		{ // Setup gizmo cone.
			CGizmoCone::Data data { };
			data.instanceMax = 16;
			data.matHash = Math::FNV1a_64(L"MATERIAL_GIZMO_TRIANGLE");
			m_gizmoCone.SetData(data);
			m_gizmoCone.Initialize();
		}
		
		m_pMaterialLine = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64(L"MATERIAL_GIZMO_LINE")));
		m_pMaterialTri = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64(L"MATERIAL_GIZMO_TRIANGLE")));
		m_pMaterialTriUnit = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64(L"MATERIAL_GIZMO_TRIANGLE_UNLIT")));

		CSceneManager::Instance().RenderingSystem().AddOverlayRenderer(VIEW_HASH_OVERLAY, std::bind(&CGizmoDraw::Render, this));
	}

	void CGizmoDraw::Release()
	{
		m_gizmoQuad.Release();
		m_gizmoCone.Release();
		m_gizmoCube.Release();
		m_gizmoHalfCircle.Release();
		m_gizmoCircle.Release();
		m_gizmoLine.Release();
	}

	void CGizmoDraw::Render()
	{
		// Line rendering.
		m_pMaterialLine->GetShader()->GetRootSignature()->Bind();

		m_gizmoLine.Render();
		m_gizmoCircle.Render();
		m_gizmoHalfCircle.Render();

		// Lit Geometry rendering.
		m_pMaterialTri->GetShader()->GetRootSignature()->Bind();

		m_gizmoCube.Render();
		m_gizmoCone.Render();
		
		// Unlit Geometry rendering.
		m_pMaterialTriUnit->GetShader()->GetRootSignature()->Bind();

		m_gizmoQuad.Render();
	}
};