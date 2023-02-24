//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CProjectManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPROJECTMANAGER_H
#define CPROJECTMANAGER_H

#include <Math/CMathFNV.h>
#include <Globals/CGlobals.h>
#include <string>

namespace App
{
	static const wchar_t* PROJECT_DIR_SETTINGS = L"\\Settings";
	static const wchar_t* PROJECT_DIR_USER = L"\\User";
	static const wchar_t* PROJECT_DIR_ASSETS = L"\\Assets";
	static const wchar_t* PROJECT_DIR_NODES = L"\\Nodes";

	static const wchar_t* BUILD_DIR_DATA = L"\\Data";
	static const wchar_t* BUILD_DIR_RESOURCE = L"\\Resources";

	class CProjectManager
	{
	public:
		static const u64 CMD_KEY_NEW_PROJECT = Math::FNV1a_64("new_project");
		static const u64 CMD_KEY_OPEN_PROJECT = Math::FNV1a_64("open_project");
		static const u64 CMD_KEY_SAVE_PROJECT = Math::FNV1a_64("save_project");
		static const u64 CMD_KEY_SAVE_PROJECT_AS = Math::FNV1a_64("save_project_as");
		static const u64 CMD_KEY_BUILD_PROJECT = Math::FNV1a_64("build_project");

	public:
		CProjectManager();
		~CProjectManager();
		CProjectManager(const CProjectManager&) = delete;
		CProjectManager(CProjectManager&&) = delete;
		CProjectManager& operator = (const CProjectManager&) = delete;
		CProjectManager& operator = (CProjectManager&&) = delete;
		
		void Initialize();
		bool ShouldQuit();

		// Accessors.
		inline const std::wstring& GetProjectPath() const { return m_projectPath; }
		inline const std::wstring& GetProjectSettingsPath() const { return m_projectSettingsPath; }
		inline const std::wstring& GetProjectUserPath() const { return m_projectUserPath; }
		inline const std::wstring& GetProjectAssetsPath() const { return m_projectAssetsPath; }
		inline const std::wstring& GetProjectNodesPath() const { return m_projectNodesPath; }
		inline const std::wstring& GetProjectName() const { return m_projectName; }

		inline const std::wstring& GetBuildPath() const { return m_buildPath; }
		inline const std::wstring& GetBuildDataPath() const { return m_buildDataPath; }
		inline const std::wstring& GetBuildResourcePath() const { return m_buildResourcePath; }

	private:
		bool ConfirmSaveChanges(const wchar_t* msg);

		void UpdateTitle(bool bSaved);
		void ExtractTitleFromPath();

		bool NewProject();
		bool OpenProject();
		bool SaveProject();
		bool SaveProjectAs();
		bool BuildProject();

		void ClearPaths();
		void FillPaths();
		void CreatePaths();
		bool VerifyPaths();

	private:
		std::wstring m_projectPath;
		std::wstring m_projectSettingsPath;
		std::wstring m_projectUserPath;
		std::wstring m_projectAssetsPath;
		std::wstring m_projectNodesPath;
		std::wstring m_projectName;

		std::wstring m_buildPath;
		std::wstring m_buildDataPath;
		std::wstring m_buildResourcePath;
	};
};

#endif
