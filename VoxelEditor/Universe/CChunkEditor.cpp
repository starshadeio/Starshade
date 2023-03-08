//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Universe/CChunkEditor.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CChunkEditor.h"
#include "../Application/CEditor.h"
#include "../Application/CAppData.h"
#include "../Resources/CAssetManager.h"
#include <Factory/CFactory.h>
#include <Application/CPlatform.h>
#include <Application/CPanel.h>
#include <Application/CSceneManager.h>
#include <Application/CCommandManager.h>
#include <Application/CLocalization.h>
#include <Math/CMathFNV.h>
#include <fstream>

namespace Universe
{
	CChunkEditor::CChunkEditor() :
		m_pNode(nullptr)
	{
	}

	CChunkEditor::~CChunkEditor()
	{
	}

	void CChunkEditor::Initialize()
	{
		{ // Setup workspace.
			m_workspace.Initialize();
		}

		{ // Setup commands.
			App::CCommand::Properties props = { };
			props.bSaveEnabled = true;
			props.bUndoEnabled = false;

			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_CHUNK_RESET, std::bind(&CChunkEditor::ChunkReset, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_CHUNK_CLEAR, std::bind(&CChunkEditor::ChunkClear, this), props);

			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_CUT, std::bind(&CChunkEditor::Cut, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_COPY, std::bind(&CChunkEditor::Copy, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_PASTE, std::bind(&CChunkEditor::Paste, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_DELETE, std::bind(&CChunkEditor::Delete, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_FILL, std::bind(&CChunkEditor::Fill, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_PAINT, std::bind(&CChunkEditor::Paint, this));

			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_LOD_DOWN, std::bind(&CChunkEditor::LODDown, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_LOD_UP, std::bind(&CChunkEditor::LODUp, this));

			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_CHUNK_COPY, std::bind(&CChunkEditor::ChunkCopy, this, std::placeholders::_1));

			props.bUndoEnabled = true;
			
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_CHUNK_CUT, std::bind(&CChunkEditor::ChunkCut, this, std::placeholders::_1, std::placeholders::_2), props);
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_CHUNK_PASTE, std::bind(&CChunkEditor::ChunkPaste, this, std::placeholders::_1, std::placeholders::_2), props);
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_CHUNK_DELETE, std::bind(&CChunkEditor::ChunkDelete, this, std::placeholders::_1, std::placeholders::_2), props);
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_CHUNK_FILL, std::bind(&CChunkEditor::ChunkFill, this, std::placeholders::_1, std::placeholders::_2), props);
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_CHUNK_PAINT, std::bind(&CChunkEditor::ChunkPaint, this, std::placeholders::_1, std::placeholders::_2), props);

			App::CCommandManager::Instance().RegisterAction(CChunkWorkspace::CMD_KEY_WORKSPACE_OFF, std::bind(&CChunkEditor::OnCmdWorkspaceOff, this));
			App::CCommandManager::Instance().RegisterAction(CChunkWorkspace::CMD_KEY_WORKSPACE_ON, std::bind(&CChunkEditor::OnCmdWorkspaceOn, this));
		}
	}

	void CChunkEditor::PostInitialize()
	{
		m_workspace.PostInitialize();
	}

	void CChunkEditor::Update()
	{
		m_workspace.Update();
	}

	void CChunkEditor::LateUpdate()
	{
		m_workspace.LateUpdate();
	}

	void CChunkEditor::Release()
	{
		m_workspace.Release();
	}
	
	//-----------------------------------------------------------------------------------------------
	// File methods.
	//-----------------------------------------------------------------------------------------------
	
	void CChunkEditor::Save() const
	{
		std::ofstream file(App::CEditor::Instance().Project().GetProjectSettingsPath() + L"\\workspace.dat", std::ios::binary);

		//assert(file.is_open());
		if(!file.is_open()) { return; }

		m_workspace.Save(file);

		file.close();
	}

	void CChunkEditor::Load()
	{
		std::ifstream file(App::CEditor::Instance().Project().GetProjectSettingsPath() + L"\\workspace.dat", std::ios::binary);

		//assert(file.is_open());
		if(!file.is_open()) { return; }

		m_workspace.Load(file);

		file.close();
	}
	
	//-----------------------------------------------------------------------------------------------
	// State methods.
	//-----------------------------------------------------------------------------------------------
	
	void CChunkEditor::OnPlay()
	{
		m_workspace.OnPlay();
	}

	void CChunkEditor::OnEdit()
	{
		m_workspace.OnEdit();
	}

	//-----------------------------------------------------------------------------------------------
	// Commands methods.
	//-----------------------------------------------------------------------------------------------
	
	static const u64 LOC_KEY_MSG_TITLE_CONFIRM = Math::FNV1a_64("MENUS:MSG_TITLE_CONFIRM");
	static const u64 LOC_KEY_MSG_CLEAR_CHUNK = Math::FNV1a_64("MENUS:MSG_CLEAR_CHUNK");

	bool CChunkEditor::ChunkReset()
	{
		if(m_pNode == nullptr) return false;
		m_pNode->GetChunkNode()->Reset();

		return true;
	}

	bool CChunkEditor::ChunkClear()
	{
		if(m_pNode == nullptr) return false;

		auto result = CFactory::Instance().GetPlatform()->PostMessageBox(App::CLocalization::Instance().Get(LOC_KEY_MSG_CLEAR_CHUNK).Get().c_str(), 
			App::CLocalization::Instance().Get(LOC_KEY_MSG_TITLE_CONFIRM).Get().c_str(), App::MessageBoxType::YesNoCancel);

		if(result == App::MessageBoxState::Yes)
		{
			m_pNode->GetChunkNode()->Clear();
			App::CCommandManager::Instance().ResetRecorder();

			return true;
		}

		return false;
	}

	bool CChunkEditor::Cut()
	{
		if(!m_workspace.IsActive()) return false;
		
		OpExtents opExtents;
		m_pNode->GetChunkNode()->GetExtentsWorkspace(opExtents.mn, opExtents.mx);
		App::CCommandManager::Instance().Execute(CMD_KEY_CHUNK_CUT, &opExtents);

		return true;
	}

	bool CChunkEditor::Copy()
	{
		if(!m_workspace.IsActive()) return false;
		
		OpExtents opExtents;
		m_pNode->GetChunkNode()->GetExtentsWorkspace(opExtents.mn, opExtents.mx);
		App::CCommandManager::Instance().Execute(CMD_KEY_CHUNK_COPY, &opExtents);

		return true;
	}

	bool CChunkEditor::Paste()
	{
		if(!m_workspace.IsActive()) return false;
		
		OpExtents opExtents;
		m_pNode->GetChunkNode()->GetExtentsWorkspace(opExtents.mn, opExtents.mx);
		App::CCommandManager::Instance().Execute(CMD_KEY_CHUNK_PASTE, &opExtents);

		return true;
	}

	bool CChunkEditor::Delete()
	{
		if(!m_workspace.IsActive()) return false;

		OpExtents opExtents;
		m_pNode->GetChunkNode()->GetExtentsWorkspace(opExtents.mn, opExtents.mx);
		App::CCommandManager::Instance().Execute(CMD_KEY_CHUNK_DELETE, &opExtents);

		return true;
	}

	bool CChunkEditor::Fill()
	{
		if(!m_workspace.IsActive()) return false;
		
		OpExtentsFill opExtents;
		opExtents.id = static_cast<u32>(Resources::CAssets::Instance().GetPalette().GetPaletteIndex());
		m_pNode->GetChunkNode()->GetExtentsWorkspace(opExtents.mn, opExtents.mx);
		App::CCommandManager::Instance().Execute(CMD_KEY_CHUNK_FILL, &opExtents);

		return true;
	}

	bool CChunkEditor::Paint()
	{
		if(!m_workspace.IsActive()) return false;
		
		OpExtentsFill opExtents;
		opExtents.id = static_cast<u32>(Resources::CAssets::Instance().GetPalette().GetPaletteIndex());
		m_pNode->GetChunkNode()->GetExtentsWorkspace(opExtents.mn, opExtents.mx);
		App::CCommandManager::Instance().Execute(CMD_KEY_CHUNK_PAINT, &opExtents);

		return true;
	}

	bool CChunkEditor::LODDown()
	{
		App::CSceneManager::Instance().UniverseManager().ChunkManager().LODDown(m_pNode->GetChunkNode());
		return true;
	}

	bool CChunkEditor::LODUp()
	{
		App::CSceneManager::Instance().UniverseManager().ChunkManager().LODUp(m_pNode->GetChunkNode());
		return true;
	}

	bool CChunkEditor::ChunkCut(const void* param, bool bInverse)
	{
		const OpExtents& opExtents = *reinterpret_cast<const OpExtents*>(param);
		
		if(!bInverse)
		{
			std::vector<BlockFloat> priorList;
			priorList.push_back(opExtents.mn.x);
			priorList.push_back(opExtents.mn.y);
			priorList.push_back(opExtents.mn.z);
			priorList.push_back(opExtents.mx.x);
			priorList.push_back(opExtents.mx.y);
			priorList.push_back(opExtents.mx.z);

			m_clipboard.clear();
			m_clipboard.push_back(opExtents.mx.x - opExtents.mn.x);
			m_clipboard.push_back(opExtents.mx.y - opExtents.mn.y);
			m_clipboard.push_back(opExtents.mx.z - opExtents.mn.z);

			m_pNode->GetChunkNode()->ReadWriteBlocksInExtents(opExtents.mn, opExtents.mx, [](u32 index, std::pair<Math::VectorInt3, u32> indices, Block& block){
				block = Block(0, false);
				return true;
			}, [this, &priorList](u32 index, Block block, std::pair<Math::VectorInt3, u32> indices){
				m_clipboard.push_back(block);
				priorList.push_back(block);
			});

			App::CCommandManager::Instance().RecordUndo(priorList.data(), sizeof(priorList[0]) * priorList.size(), alignof(BlockFloat));
		}
		else
		{
			const BlockFloat* priorList = reinterpret_cast<const BlockFloat*>(reinterpret_cast<const u8*>(param) + sizeof(opExtents));
			m_pNode->GetChunkNode()->WriteBlocksInExtents(opExtents.mn, opExtents.mx, [&priorList](u32 index, std::pair<Math::VectorInt3, u32> indices, Block& block){
				block = priorList[index].b;
				return true;
			});
		}

		return true;
	}

	bool CChunkEditor::ChunkCopy(const void* param)
	{
		const OpExtents& opExtents = *reinterpret_cast<const OpExtents*>(param);
		
		m_clipboard.clear();
		m_clipboard.push_back(opExtents.mx.x - opExtents.mn.x);
		m_clipboard.push_back(opExtents.mx.y - opExtents.mn.y);
		m_clipboard.push_back(opExtents.mx.z - opExtents.mn.z);

		m_pNode->GetChunkNode()->ReadBlocksInExtents(opExtents.mn, opExtents.mx, [this](u32 index, Block block, std::pair<Math::VectorInt3, u32> indices){
			m_clipboard.push_back(block);
		});

		return true;
	}

	bool CChunkEditor::ChunkPaste(const void* param, bool bInverse)
	{
		if(m_clipboard.empty()) return false;

		OpExtents opExtents = *reinterpret_cast<const OpExtents*>(param);

		if(!bInverse)
		{
			Math::Vector3 center = opExtents.mn + (opExtents.mx - opExtents.mn) * 0.5f;
			Math::Vector3 size = { m_clipboard[0].f, m_clipboard[1].f, m_clipboard[2].f };
			opExtents.mn = center - size * 0.5f;
			opExtents.mx = center + size * 0.5f;
		
			std::vector<BlockFloat> priorList;
			priorList.push_back(opExtents.mn.x);
			priorList.push_back(opExtents.mn.y);
			priorList.push_back(opExtents.mn.z);
			priorList.push_back(opExtents.mx.x);
			priorList.push_back(opExtents.mx.y);
			priorList.push_back(opExtents.mx.z);

			m_pNode->GetChunkNode()->ReadWriteBlocksInExtents(opExtents.mn, opExtents.mx, [this](u32 index, std::pair<Math::VectorInt3, u32> indices, Block& block){
				if(!m_clipboard[index + 3].b.bFilled) return false;
				block = m_clipboard[index + 3].b;
				return true;
			}, [&priorList, this](u32 index, Block block, std::pair<Math::VectorInt3, u32> indices){
				priorList.push_back(block);
			});

			App::CCommandManager::Instance().RecordUndo(priorList.data(), sizeof(priorList[0]) * priorList.size(), alignof(BlockFloat));
		}
		else
		{
			const BlockFloat* priorList = reinterpret_cast<const BlockFloat*>(reinterpret_cast<const u8*>(param) + sizeof(opExtents));
			m_pNode->GetChunkNode()->WriteBlocksInExtents(opExtents.mn, opExtents.mx, [&priorList](u32 index, std::pair<Math::VectorInt3, u32> indices, Block& block){
				block = priorList[index].b;
				return true;
			});
		}

		return true;
	}

	bool CChunkEditor::ChunkDelete(const void* param, bool bInverse)
	{
		const OpExtents& opExtents = *reinterpret_cast<const OpExtents*>(param);
		
		if(!bInverse)
		{
			std::vector<BlockFloat> priorList;
			priorList.push_back(opExtents.mn.x);
			priorList.push_back(opExtents.mn.y);
			priorList.push_back(opExtents.mn.z);
			priorList.push_back(opExtents.mx.x);
			priorList.push_back(opExtents.mx.y);
			priorList.push_back(opExtents.mx.z);

			m_pNode->GetChunkNode()->ReadWriteBlocksInExtents(opExtents.mn, opExtents.mx, [](u32 index, std::pair<Math::VectorInt3, u32> indices, Block& block){
				block = Block(0, false);
				return true;
			}, [&priorList](u32 index, Block block, std::pair<Math::VectorInt3, u32> indices){
				priorList.push_back(block);
			});

			App::CCommandManager::Instance().RecordUndo(priorList.data(), sizeof(priorList[0]) * priorList.size(), alignof(BlockFloat));
		}
		else
		{
			const BlockFloat* priorList = reinterpret_cast<const BlockFloat*>(reinterpret_cast<const u8*>(param) + sizeof(opExtents));
			m_pNode->GetChunkNode()->WriteBlocksInExtents(opExtents.mn, opExtents.mx, [&priorList](u32 index, std::pair<Math::VectorInt3, u32> indices, Block& block){
				block = priorList[index].b;
				return true;
			});
		}

		return true;
	}

	bool CChunkEditor::ChunkFill(const void* param, bool bInverse)
	{
		const OpExtentsFill& opExtents = *reinterpret_cast<const OpExtentsFill*>(param);
		
		typedef std::pair<std::pair<Math::VectorInt3, u32>, Block> PriorElem;

		if(!bInverse)
		{
			std::vector<BlockFloat> priorList;
			priorList.push_back(opExtents.mn.x);
			priorList.push_back(opExtents.mn.y);
			priorList.push_back(opExtents.mn.z);
			priorList.push_back(opExtents.mx.x);
			priorList.push_back(opExtents.mx.y);
			priorList.push_back(opExtents.mx.z);
			priorList.push_back(Block(opExtents.id));

			m_pNode->GetChunkNode()->ReadWriteBlocksInExtents(opExtents.mn, opExtents.mx, [&opExtents](u32 index, std::pair<Math::VectorInt3, u32> indices, Block& block){
				block = Block(opExtents.id, true);
				return true;
			}, [&priorList](u32 index, Block block, std::pair<Math::VectorInt3, u32> indices){
				priorList.push_back(block);
			});

			App::CCommandManager::Instance().RecordUndo(priorList.data(), sizeof(priorList[0]) * priorList.size(), alignof(PriorElem));
		}
		else
		{
			const BlockFloat* priorList = reinterpret_cast<const BlockFloat*>(reinterpret_cast<const u8*>(param) + sizeof(opExtents));
			m_pNode->GetChunkNode()->WriteBlocksInExtents(opExtents.mn, opExtents.mx, [&priorList](u32 index, std::pair<Math::VectorInt3, u32> indices, Block& block){
				block = priorList[index].b;
				return true;
			});
		}

		return true;
	}

	bool CChunkEditor::ChunkPaint(const void* param, bool bInverse)
	{
		const OpExtentsFill& opExtents = *reinterpret_cast<const OpExtentsFill*>(param);
		
		typedef std::pair<std::pair<Math::VectorInt3, u32>, Block> PriorElem;

		if(!bInverse)
		{
			std::vector<BlockFloat> priorList;
			priorList.push_back(opExtents.mn.x);
			priorList.push_back(opExtents.mn.y);
			priorList.push_back(opExtents.mn.z);
			priorList.push_back(opExtents.mx.x);
			priorList.push_back(opExtents.mx.y);
			priorList.push_back(opExtents.mx.z);
			priorList.push_back(Block(opExtents.id));

			m_pNode->GetChunkNode()->ReadPaintBlocksInExtents(opExtents.mn, opExtents.mx, [&opExtents](u32 index, std::pair<Math::VectorInt3, u32> indices, Block& block){
				block = Block(opExtents.id, true);
				return true;
			}, [&priorList](u32 index, Block block, std::pair<Math::VectorInt3, u32> indices){
				priorList.push_back(block);
			});

			App::CCommandManager::Instance().RecordUndo(priorList.data(), sizeof(priorList[0]) * priorList.size(), alignof(PriorElem));
		}
		else
		{
			const BlockFloat* priorList = reinterpret_cast<const BlockFloat*>(reinterpret_cast<const u8*>(param) + sizeof(opExtents));
			m_pNode->GetChunkNode()->WriteBlocksInExtents(opExtents.mn, opExtents.mx, [&priorList](u32 index, std::pair<Math::VectorInt3, u32> indices, Block& block){
				block = priorList[index].b;
				return true;
			});
		}

		return true;
	}

	// Actions.
	void CChunkEditor::OnCmdWorkspaceOff()
	{
		if(!m_pNode) return;
		m_pNode->GetChunkNode()->SetWorkspaceActive(false);
	}

	void CChunkEditor::OnCmdWorkspaceOn()
	{
		if(!m_pNode) return;
		m_pNode->GetChunkNode()->SetWorkspaceActive(true);
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CChunkEditor::SetNode(App::CNodeEx* pNode)
	{
		m_pNode = pNode;
		if(m_pNode) m_pNode->GetChunkNode()->SetWorkspaceActive(m_workspace.IsActive());
	}
};
