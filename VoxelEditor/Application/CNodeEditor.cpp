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
		m_bEditorAudioPlaying(true),
		m_user(L"User", VIEW_HASH_MAIN),
		m_rootNode(L"Root", VIEW_HASH_MAIN),
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
			data.pawnData.startPosition = Math::SIMDVector(0.0f, 6.0f, -10.0f);
			m_user.SetData(data);
			m_user.Initialize();
		}

		{ // Setup the environment.
			Universe::CEnvironment::Data data { };
			data.bActive = m_bEditorAudioPlaying;
			m_environment.SetData(data);
			m_environment.Initialize();
		}
		
		{ // Create node selector.
			CNodeSelect::Data data { };
			m_nodeSelect.SetData(data);
			m_nodeSelect.Initialize();
		}

		{ // Add node objects.
			// Spawn points.
			m_rootNode.AddObject(L"spawn_point");
			
			m_rootNode.AddComponent<Logic::CNodeTransform, Logic::CNodeTransform::Data>([](Logic::CNodeTransform::Data* pTransform){
				pTransform->SetPosition(Math::SIMDVector(0.0f, -3.0f, 0.0f));
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
				pData->AddMaterial(reinterpret_cast<Graphics::CMaterial*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_MATERIAL, Math::FNV1a_64(L"MATERIAL_PRIMITIVE"))));
				pData->SetPreRenderer([this](Graphics::CMaterial* pMaterial){
					static const u32 matrixBufferHash = Math::FNV1a_32("MatrixBuffer");
					static const u32 worldHash = Math::FNV1a_32("World");
					static const u32 viewHash = Math::FNV1a_32("View");
					static const u32 projHash = Math::FNV1a_32("Proj");

					auto& transform = *reinterpret_cast<Logic::CNodeTransform::Data*>(App::CCoreManager::Instance().NodeRegistry().GetComponentFromObject(Logic::CNodeTransform::HASH, m_rootNode.GetSelectedObject()->GetHash()));

					pMaterial->SetFloat(matrixBufferHash, worldHash, transform.GetWorldMatrix().f32, 16);
					pMaterial->SetFloat(matrixBufferHash, viewHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetViewMatrixInv().f32, 16);
					pMaterial->SetFloat(matrixBufferHash, projHash, App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetProjectionMatrix().f32, 16);
				});
			});
		}

		{ // Setup chunk editor.
			m_chunkEditor.SetNode(&m_rootNode);
			m_chunkEditor.Initialize();
		}

		m_gizmo.Node().SetNode(&m_rootNode);
		
		CSceneManager::Instance().UniverseManager().ChunkManager().SetTexture(Resources::CAssets::Instance().GetPalette().GetTexture());

		{ // Setup commands.
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_EDITOR_AUDIO_PLAY, std::bind(&CNodeEditor::EditorAudioPlay, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_EDITOR_AUDIO_STOP, std::bind(&CNodeEditor::EditorAudioStop, this));
		}
	}
	
	void CNodeEditor::PostInitialize()
	{
		m_user.PostInitialize();
		m_chunkEditor.PostInitialize();
	}

	void CNodeEditor::Update()
	{
		m_nodeSelect.Update();
		m_chunkEditor.Update();

		m_user.Update();
		m_environment.Update();
		m_gizmo.Update();
	}

	void CNodeEditor::LateUpdate()
	{
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
		transform.SetPosition(Math::SIMD_VEC_UP * -3.0f);
		transform.SetEuler(Math::SIMD_VEC_ZERO);
	}
	
	void CNodeEditor::Save()
	{
		m_user.Save();
		m_chunkEditor.Save();
		m_rootNode.Save();
	}

	void CNodeEditor::Load()
	{
		m_user.Load();
		m_chunkEditor.Load();
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
		m_environment.SetActive(true);

		m_nodeSelect.OnPlay();
		m_chunkEditor.OnPlay();
		m_user.OnPlay(bSpawn);
		m_gizmo.SetActive(false);
		m_rootNode.GetSelectedObject()->GetComponent<Graphics::CMeshRenderer>()->SetActive(false);
	}

	void CNodeEditor::OnEdit()
	{
		m_environment.SetActive(m_bEditorAudioPlaying);

		m_user.OnEdit();
		m_chunkEditor.OnEdit();
		m_nodeSelect.OnEdit();
		m_gizmo.SetActive(true);
		m_rootNode.GetSelectedObject()->GetComponent<Graphics::CMeshRenderer>()->SetActive(true);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Commands methods.
	//-----------------------------------------------------------------------------------------------

	bool CNodeEditor::EditorAudioPlay()
	{
		if(m_bEditorAudioPlaying) return false;
		m_bEditorAudioPlaying = true;
		m_environment.SetActive(true);
		return true;
	}

	bool CNodeEditor::EditorAudioStop()
	{
		if(!m_bEditorAudioPlaying) return false;
		m_bEditorAudioPlaying = false;
		m_environment.SetActive(false);
		return true;
	}
};
