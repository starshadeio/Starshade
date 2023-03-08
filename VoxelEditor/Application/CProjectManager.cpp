//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CProjectManager.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CProjectManager.h"
#include "CEditor.h"
#include <Factory/CFactory.h>
#include <Application/CPlatform.h>
#include <Application/CPanel.h>
#include <Application/CCommandManager.h>
#include <Application/CLocalization.h>
#include <Math/CMathFNV.h>
#include <Utilities/CFileSystem.h>
#include <Utilities/CArchive.h>

namespace App
{
	static const u64 LOC_KEY_DIALOG_TITLE_SAVE_PROJECT = Math::FNV1a_64("MENUS:DIALOG_TITLE_SAVE_PROJECT");
	static const u64 LOC_KEY_DIALOG_TITLE_OPEN_PROJECT = Math::FNV1a_64("MENUS:DIALOG_TITLE_OPEN_PROJECT");
	static const u64 LOC_KEY_DIALOG_TITLE_BUILD_PROJECT = Math::FNV1a_64("MENUS:DIALOG_TITLE_BUILD_PROJECT");

	static const u64 LOC_KEY_MSG_TITLE_CONFIRM = Math::FNV1a_64("MENUS:MSG_TITLE_CONFIRM");
	static const u64 LOC_KEY_MSG_TITLE_WARNING = Math::FNV1a_64("MENUS:MSG_TITLE_WARNING");
	static const u64 LOC_KEY_MSG_TITLE_FAILURE = Math::FNV1a_64("MENUS:MSG_TITLE_FAILURE");

	static const u64 LOC_KEY_MSG_FILE_NOT_FOUND = Math::FNV1a_64("MENUS:MSG_FILE_NOT_FOUND");
	static const u64 LOC_KEY_MSG_INVALID_PROJECT_DIRECTORY = Math::FNV1a_64("MENUS:MSG_INVALID_PROJECT_DIRECTORY");

	static const u64 LOC_KEY_MSG_SAVE_BEFORE_NEW = Math::FNV1a_64("MENUS:MSG_SAVE_BEFORE_NEW");
	static const u64 LOC_KEY_MSG_SAVE_BEFORE_OPEN = Math::FNV1a_64("MENUS:MSG_SAVE_BEFORE_OPEN");
	static const u64 LOC_KEY_MSG_SAVE_BEFORE_EXIT = Math::FNV1a_64("MENUS:MSG_SAVE_BEFORE_EXIT");
	static const u64 LOC_KEY_PROJECT_UNTITLED = Math::FNV1a_64("PROJECT:UNTITLED");

	CProjectManager::CProjectManager()
	{
	}

	CProjectManager::~CProjectManager()
	{
	}
	
	void CProjectManager::Initialize()
	{
		m_projectName = CLocalization::Instance().Get(LOC_KEY_PROJECT_UNTITLED).Get();

		{ // Setup commands.
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_NEW_PROJECT, std::bind(&CProjectManager::NewProject, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_OPEN_PROJECT, std::bind(&CProjectManager::OpenProject, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_SAVE_PROJECT, std::bind(&CProjectManager::SaveProject, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_SAVE_PROJECT_AS, std::bind(&CProjectManager::SaveProjectAs, this));
			App::CCommandManager::Instance().RegisterCommand(CMD_KEY_BUILD_PROJECT, std::bind(&CProjectManager::BuildProject, this));

			App::CCommandManager::Instance().AddOnSaveChangeCallback([this](bool bUnsaved, u64 unsavedIndex){ UpdateTitle(!bUnsaved); });
		}

		UpdateTitle(true);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Utilities methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CProjectManager::ShouldQuit()
	{
		return ConfirmSaveChanges(CLocalization::Instance().Get(LOC_KEY_MSG_SAVE_BEFORE_EXIT).Get().c_str());
	}

	// Method for asking if the user once to save any changes they made to their scene before replacing it or exiting.
	bool CProjectManager::ConfirmSaveChanges(const wchar_t* msg)
	{
		if(CCommandManager::Instance().HasUnsavedChanges())
		{
			auto result = CFactory::Instance().GetPlatform()->PostMessageBox(msg, CLocalization::Instance().Get(LOC_KEY_MSG_TITLE_CONFIRM).Get().c_str(), App::MessageBoxType::YesNoCancel);
			switch(result)
			{
				case App::MessageBoxState::Cancel:
					return false;
				case App::MessageBoxState::No:
					return true;
				case App::MessageBoxState::Yes:
					if(App::CCommandManager::Instance().Execute(CMD_KEY_SAVE_PROJECT))
					{
						return true;
					}
					else
					{
						return false;
					}
				default:
					return false;
			}
		}
		else
		{
			return true;
		}
	}
	
	void CProjectManager::UpdateTitle(bool bSaved)
	{
		auto mainPanel = CFactory::Instance().GetPlatform()->GetMainPanel();
		mainPanel->SetTitle((mainPanel->GetData().title + L" : " + m_projectName + (bSaved ? L"" : L"*")).c_str());
	}

	void CProjectManager::ExtractTitleFromPath()
	{
		size_t offset = m_projectPath.size();

		while(offset > 0 && (m_projectPath[offset - 1] != L'/' && m_projectPath[offset - 1] != L'\\'))
		{
			--offset;
		}

		m_projectName = m_projectPath.substr(offset);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Command methods.
	//-----------------------------------------------------------------------------------------------

	bool CProjectManager::NewProject()
	{
		if(ConfirmSaveChanges(CLocalization::Instance().Get(LOC_KEY_MSG_SAVE_BEFORE_NEW).Get().c_str()))
		{
			ClearPaths();
			m_projectName = CLocalization::Instance().Get(LOC_KEY_PROJECT_UNTITLED).Get();
			App::CCommandManager::Instance().ResetRecorder();
			if(!App::CCommandManager::Instance().MarkSavedChanges()) UpdateTitle(true);
			return true;
		}
		
		return false;
	}

	bool CProjectManager::OpenProject()
	{
		if(ConfirmSaveChanges(CLocalization::Instance().Get(LOC_KEY_MSG_SAVE_BEFORE_OPEN).Get().c_str()))
		{
			if(Util::CFileSystem::Instance().OpenFile(CLocalization::Instance().Get(LOC_KEY_DIALOG_TITLE_OPEN_PROJECT).Get().c_str(), m_projectPath, true))
			{
				ExtractTitleFromPath();
				FillPaths();
				if(VerifyPaths())
				{
					App::CCommandManager::Instance().ResetRecorder();
					if(!App::CCommandManager::Instance().MarkSavedChanges()) UpdateTitle(true);
					return true;
				}
				else
				{
					CFactory::Instance().GetPlatform()->PostMessageBox(CLocalization::Instance().Get(LOC_KEY_MSG_INVALID_PROJECT_DIRECTORY).Get().c_str(), 
						CLocalization::Instance().Get(LOC_KEY_MSG_TITLE_FAILURE).Get().c_str(), App::MessageBoxType::Ok);
				}
			}
		}

		return false;
	}

	bool CProjectManager::SaveProject()
	{
		if(m_projectPath.empty())
		{
			return SaveProjectAs();
		}
		else
		{
			// Project already created.
			ExtractTitleFromPath();
			if(!App::CCommandManager::Instance().MarkSavedChanges()) UpdateTitle(true);
			return true;
		}
	}

	bool CProjectManager::SaveProjectAs()
	{
		if(Util::CFileSystem::Instance().OpenFile(CLocalization::Instance().Get(LOC_KEY_DIALOG_TITLE_SAVE_PROJECT).Get().c_str(), m_projectPath, true))
		{
			ExtractTitleFromPath();
			FillPaths();
			CreatePaths();
			if(!App::CCommandManager::Instance().MarkSavedChanges()) UpdateTitle(true);
			return true;
		}

		return false;
	}

	bool CProjectManager::BuildProject()
	{
		if(Util::CFileSystem::Instance().OpenFile(CLocalization::Instance().Get(LOC_KEY_DIALOG_TITLE_BUILD_PROJECT).Get().c_str(), m_buildPath, true))
		{
			Util::CFileSystem::Instance().NewDirectory(m_buildPath.c_str());
			Util::CFileSystem::Instance().NewDirectory((m_buildDataPath = m_buildPath + BUILD_DIR_DATA).c_str());
			Util::CFileSystem::Instance().NewDirectory((m_buildConfigPath = m_buildPath + BUILD_DIR_CONFIG).c_str());
			Util::CFileSystem::Instance().NewDirectory((m_buildResourcePath = m_buildPath + BUILD_DIR_RESOURCE).c_str());

			{	// Copy binaries.
				Util::CArchive m_archive;

				std::ifstream fileIn(L".\\Data\\app.pack", std::ios::binary);
				if(fileIn.is_open())
				{
					m_archive.Load(m_buildPath + L"\\", fileIn);
					fileIn.close();
				}
				else
				{
					CFactory::Instance().GetPlatform()->PostMessageBox(CLocalization::Instance().Get(LOC_KEY_MSG_FILE_NOT_FOUND).Format({ L".\\Data\\app.pack" }).c_str(), 
						CLocalization::Instance().Get(LOC_KEY_MSG_TITLE_WARNING).Get().c_str(), App::MessageBoxType::Ok);
				}
			}

			Util::CFileSystem::Instance().OpenFolderWithTarget(m_buildResourcePath.c_str(), nullptr);

			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	void CProjectManager::ClearPaths()
	{
		m_projectPath.clear();
		m_projectSettingsPath.clear();
		m_projectUserPath.clear();
		m_projectAssetsPath.clear();
		m_projectNodesPath.clear();

		m_buildPath.clear();
		m_buildDataPath.clear();
		m_buildConfigPath.clear();
		m_buildResourcePath.clear();
	}

	void CProjectManager::FillPaths()
	{
		m_projectSettingsPath = m_projectPath + PROJECT_DIR_SETTINGS;
		m_projectUserPath = m_projectPath + PROJECT_DIR_USER;
		m_projectAssetsPath = m_projectPath + PROJECT_DIR_ASSETS;
		m_projectNodesPath = m_projectPath + PROJECT_DIR_NODES;
	}
	
	void CProjectManager::CreatePaths()
	{
		Util::CFileSystem::Instance().NewDirectory(m_projectSettingsPath.c_str());
		Util::CFileSystem::Instance().NewDirectory(m_projectUserPath.c_str());
		Util::CFileSystem::Instance().NewDirectory(m_projectAssetsPath.c_str());
		Util::CFileSystem::Instance().NewDirectory(m_projectNodesPath.c_str());
	}

	bool CProjectManager::VerifyPaths()
	{
		if(!Util::CFileSystem::Instance().VerifyDirectory(m_projectPath.c_str())) return false;
		if(!Util::CFileSystem::Instance().VerifyDirectory(m_projectSettingsPath.c_str())) return false;
		if(!Util::CFileSystem::Instance().VerifyDirectory(m_projectUserPath.c_str())) return false;
		if(!Util::CFileSystem::Instance().VerifyDirectory(m_projectAssetsPath.c_str())) return false;
		if(!Util::CFileSystem::Instance().VerifyDirectory(m_projectNodesPath.c_str())) return false;

		return true;
	}
};
