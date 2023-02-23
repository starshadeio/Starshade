//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CDebugDraw.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CDebugDraw.h"
#include "CMaterial.h"
#include "../Graphics/CShader.h"
#include "../Graphics/CRootSignature.h"
#include "../Graphics/CGraphicsAPI.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include "../Application/CSceneManager.h"
#include <Math/CMathFNV.h>

namespace Graphics
{
	CDebugDraw::CDebugDraw() : 
		CVObject(L"Debug Draw", 0),
		m_debugRect(this),
		m_pMaterial3D(nullptr),
		m_pMaterial2D(nullptr)
	{
	}

	CDebugDraw::~CDebugDraw()
	{
	}
	
	void CDebugDraw::Initialize()
	{
		{ // Setup debug rect.
			CDebugRect::Data data { };
			data.instanceMax = 1024;
			data.matHash = Math::FNV1a_64(L"MATERIAL_DEBUG2D");
			m_debugRect.SetData(data);
			m_debugRect.Initialize();
		}
		
		//m_pMaterial3D = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64(L"MATERIAL_DEBUG3D")));
		m_pMaterial2D = reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64(L"MATERIAL_DEBUG2D")));
	}
	
	void CDebugDraw::Render()
	{
		// 3D.
		//m_pMaterial3D->GetShader()->GetRootSignature()->Bind();

		// 2D.
		m_pMaterial2D->GetShader()->GetRootSignature()->Bind();

		m_debugRect.Render();
	}
	
	void CDebugDraw::Release()
	{
		m_debugRect.Release();
	}
};