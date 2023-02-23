//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CNodeEditor.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CNodeEditor.h"
#include "CNodeObjectEx.h"
#include "CAppData.h"
#include "CAppState.h"
#include "../Resources/CAssetManager.h"
#include <Application/CSceneManager.h>
#include <Application/CCommandManager.h>
#include <Actors/CSpawner.h>
#include <Logic/CTransform.h>
#include <Logic/CNodeTransform.h>
#include <Graphics/CMeshFilter.h>
#include <Graphics/CMeshRenderer.h>
#include <Graphics/CMaterial.h>
#include <Math/CMathFloat.h>

namespace App
{
	CNodeEditor::CNodeEditor() :
		m_user(L"User", VIEW_HASH_MAIN),
		m_rootNode(L"Root", VIEW_HASH_MAIN),
		m_grid(L"NodeGrid", VIEW_HASH_MAIN),
		m_pSelectedNode(&m_rootNode)
	{
	}

	CNodeEditor::~CNodeEditor()
	{
	}
		
	void CNodeEditor::Initialize()
	{
		{ // Gizmos.
			m_gizmo.Initialize();
		}

		{ // Mesh.
			m_meshSpawnPoint.Initialize();
		}

		{ // User.
			Actor::CUser::Data data { };
			data.inputHash = INPUT_HASH_LAYOUT_PLAYER;
			data.pawnData.speed = 3.5f;
			data.pawnData.jumpPower = 7.5f;
			data.pawnData.startEuler = Math::Vector3(Math::g_PiOver180 * -35.0f, 0.0f, 0.0f);
			data.pawnData.startPosition = Math::SIMDVector(0.0f, 18.0f, -32.0f);
			m_user.SetData(data);
			m_user.Initialize();
		}

		{ // Setup the environment.
			Universe::CEnvironment::Data data { };
			data.bActive = true;
			m_environment.SetData(data);
			m_environment.Initialize();
		}
		
		{ // Create node selector.
			CNodeSelect::Data data { };
			m_nodeSelect.SetData(data);
			m_nodeSelect.Initialize();
		}
		
		{ // Grid.
			Universe::CNodeGrid::Data data { };
			data.cellSize = 1.0f;
			data.right = Math::VEC3_RIGHT;
			data.up = Math::VEC3_UP;
			data.forward = Math::VEC3_FORWARD;
			data.width = 33;
			data.height = 33;
			data.length = 33;
			m_grid.SetData(data);
			m_grid.Initialize();
		}

		{ // Add node objects.
			// Spawn points.
			m_rootNode.AddObject(L"spawn_point");
			
			m_rootNode.AddComponent<Logic::CNodeTransform, Logic::CNodeTransform::Data>([](Logic::CNodeTransform::Data* pTransform){
				pTransform->SetPosition(Math::SIMDVector(0.0f, -15.0f, 0.0f));
				pTransform->SetEulerConstraints(Logic::AXIS_CONSTRAINT_FLAG_X | Logic::AXIS_CONSTRAINT_FLAG_Z);
				pTransform->SetScaleConstraints(Logic::AXIS_CONSTRAINT_FLAG_ALL);
			});

			m_rootNode.AddComponent<Actor::CSpawner, Actor::CSpawner::Data>([](Actor::CSpawner::Data* pData){
			});

			m_rootNode.AddComponent<Graphics::CMeshFilter, Graphics::CMeshFilter::Data>([this](Graphics::CMeshFilter::Data* pData){
				pData->SetInstanceCount(0);
				pData->SetInstanceMax(0);
				pData->SetInstanceStride(0);
				pData->SetMesh(m_meshSpawnPoint.GetMesh());
			});

			m_rootNode.AddComponent<Graphics::CMeshRenderer, Graphics::CMeshRenderer::Data>([this](Graphics::CMeshRenderer::Data* pData){
				pData->SetMaterialHash(Math::FNV1a_64(L"MATERIAL_PRIMITIVE"));
				pData->SetPreRenderer([this](Graphics::CMaterial* pMaterial){
					static const u32 matrixBufferHash = Math::FNV1a_32("MatrixBuffer");
					static const u32 worldHash = Math::FNV1a_32("World");
					static const u32 vpHash = Math::FNV1a_32("VP");

					Math::SIMDMatrix mtx = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrix();
					mtx *= App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix();

					auto& transform = *reinterpret_cast<Logic::CNodeTransform::Data*>(App::CCoreManager::Instance().NodeRegistry().GetComponentFromObject(Logic::CNodeTransform::HASH, m_rootNode.GetSelectedObject()->GetHash()));

					pMaterial->SetFloat(matrixBufferHash, worldHash, transform.GetWorldMatrix().f32, 16);
					pMaterial->SetFloat(matrixBufferHash, vpHash, mtx.f32, 16);
				});
			});
		}

		{ // Setup chunk editor.
			m_chunkEditor.SetNode(&m_rootNode);
			m_chunkEditor.Initialize();
		}

		m_gizmo.Pivot().SetNode(&m_rootNode);
		
		CSceneManager::Instance().UniverseManager().ChunkManager().SetTexture(Resources::CAssets::Instance().GetPalette().GetTexture());
	}
	
	void CNodeEditor::PostInitialize()
	{
		m_user.PostInitialize();
	}

	void CNodeEditor::Update()
	{
		m_nodeSelect.Update();

		m_user.Update();
		m_environment.Update();
		m_gizmo.Update();
	}

	void CNodeEditor::LateUpdate()
	{
		m_grid.LateUpdate();
		m_chunkEditor.LateUpdate();
		m_gizmo.LateUpdate();
	}

	void CNodeEditor::PhysicsUpdate()
	{
		m_nodeSelect.PhysicsUpdate();
	}

	void CNodeEditor::Release()
	{
		m_nodeSelect.Release();
		m_chunkEditor.Release();
		m_grid.Release();

		m_environment.Release();
		m_user.Release();

		m_meshSpawnPoint.Release();
		m_gizmo.Release();
	}
	
	//-----------------------------------------------------------------------------------------------
	// Adjustment methods.
	//-----------------------------------------------------------------------------------------------
	
	void CNodeEditor::OnResize(const Math::Rect& rect)
	{
		m_user.OnResize(rect);
	}

	//-----------------------------------------------------------------------------------------------
	// Project methods.
	//-----------------------------------------------------------------------------------------------

	void CNodeEditor::New()
	{
		m_rootNode.New();
		auto& transform = *m_rootNode.GetSelectedObject()->GetComponent<Logic::CNodeTransform>();
		transform.SetPosition(Math::SIMD_VEC_UP * -15.0f);
		transform.SetEuler(Math::SIMD_VEC_ZERO);
	}
	
	void CNodeEditor::Save() const
	{
		m_user.Save();
		m_rootNode.Save();
	}

	void CNodeEditor::Load()
	{
		m_user.Load();
		m_rootNode.Load();
	}

	void CNodeEditor::Build() const
	{
		m_rootNode.Build();
	}
	
	//-----------------------------------------------------------------------------------------------
	// State methods.
	//-----------------------------------------------------------------------------------------------

	void CNodeEditor::OnPlay(bool bSpawn)
	{
		m_nodeSelect.OnPlay();
		m_user.OnPlay(bSpawn);
		m_grid.SetActive(false);
		m_gizmo.SetActive(false);
		m_rootNode.GetSelectedObject()->GetComponent<Graphics::CMeshRenderer>()->SetActive(false);
	}

	void CNodeEditor::OnEdit()
	{
		m_user.OnEdit();
		m_nodeSelect.OnEdit();
		m_grid.SetActive(true);
		m_gizmo.SetActive(true);
		m_rootNode.GetSelectedObject()->GetComponent<Graphics::CMeshRenderer>()->SetActive(true);
	}
};
