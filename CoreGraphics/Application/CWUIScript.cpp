//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CWUIScript.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CWUIScript.h"
#include "../Application/CPanel.h"
#include "../Application/CPlatform.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include <Utilities/CFileSystem.h>
#include <Utilities/CDebugError.h>
#include <Application/CAppVersion.h>
#include <Utilities/CConvertUtil.h>
#include <Utilities/CMemoryFree.h>
#include <Utilities/CFileUtil.h>
#include <unordered_map>
#include <string>
#include <functional>
#include <fstream>

namespace App
{
	static PANEL_SCREEN_MODE StringToScreenMode(const std::wstring& word)
	{
		static const std::unordered_map<std::wstring, PANEL_SCREEN_MODE> MAP = {
			{ L"Windowed", PSM_WINDOWED },
			{ L"Borderless", PSM_BORDERLESS },
			{ L"Fullscreen", PSM_FULLSCREEN },
		};

		return MAP.find(word)->second;
	}

	typedef std::function<void(const std::wstring&, CPanel::Data&)> PROP_FUNC;

	static const std::unordered_map<std::wstring, PROP_FUNC> PROP_MAP = {
		{ L"main", [](const std::wstring& word, CPanel::Data& data) { data.bMain = Util::StringToBool(word); } },
		{ L"title", [](const std::wstring& word, CPanel::Data& data) { data.title = word; } },
		{ L"useVersionNumber", [](const std::wstring& word, CPanel::Data& data) { data.bUseVersionNumber = Util::StringToBool(word); } },
		{ L"icon", [](const std::wstring& word, CPanel::Data& data) { data.icon = word; } },
		{ L"screenMode", [](const std::wstring& word, CPanel::Data& data) { data.targetScreenMode = StringToScreenMode(word); } },
		{ L"dimensions", [](const std::wstring& word, CPanel::Data& data) { data.targetDims = Util::StringToVectorInt<Math::VectorInt2>(word); } },
		{ L"minDimensions", [](const std::wstring& word, CPanel::Data& data) { data.minimumDims = Util::StringToVectorInt<Math::VectorInt2>(word); } },
		{ L"allowResize", [](const std::wstring& word, CPanel::Data& data) { data.bAllowResize = Util::StringToBool(word); } },
		{ L"allowDoubleClick", [](const std::wstring& word, CPanel::Data& data) { data.bAllowDoubleClick = Util::StringToBool(word); } },
	};

	//-----------------------------------------------------------------------------------------------
	// CWUIScript
	//-----------------------------------------------------------------------------------------------

	CWUIScript::CWUIScript() :
		m_pMainPanel(nullptr) {
	}

	CWUIScript::~CWUIScript() { }

	void CWUIScript::Parse()
	{
		{ // Parse script.
			Util::CScriptObject::Data data { };
			data.filename = m_data.filename;
			m_scriptObject.SetData(data);
			m_scriptObject.Build();
		}

		// Create panels out of script data.
		const auto& propBlockList = m_scriptObject.GetPropBlockList();
		for(size_t i = 0; i < propBlockList.size(); ++i)
		{
			CPanel::Data data { };

			for(size_t j = 0; j < propBlockList[i].children.size(); ++j)
			{
				auto elem = PROP_MAP.find(propBlockList[i].children[j].propList[0]);
				if(elem != PROP_MAP.end())
				{
					elem->second(propBlockList[i].children[j].propList[1], data);
				}
			}

			if(data.bUseVersionNumber) data.title += CVersion::Instance().GetAsString();
			CPanel* pPanel = CFactory::Instance().CreatePanel();

			// Set panel adjustment callbacks based on whether this is the main window.
			if(data.bMain)
			{
				assert(m_pMainPanel == nullptr);
				data.funcOnMove = m_data.platformOnMove;
				data.funcOnResize = m_data.platformOnResize;
				m_pMainPanel = pPanel;
			}
			else
			{
				data.funcOnMove = std::bind(&CWUIScript::OnMove, this, std::placeholders::_1);
				data.funcOnResize = std::bind(&CWUIScript::OnResize, this, std::placeholders::_1);
			}

			pPanel->SetData(data);
			pPanel->Initialize();

			m_panelMap.insert({ propBlockList[i].propList[0], pPanel });
		}
	}

	void CWUIScript::Release()
	{
		m_pMainPanel = nullptr;
		for(auto elem : m_panelMap)
		{
			SAFE_RELEASE_DELETE(elem.second);
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// Window adjustments.
	//-----------------------------------------------------------------------------------------------

	void CWUIScript::SaveProductionResources()
	{
		for(auto elem : m_panelMap)
		{
			if(elem.second->GetData().icon.empty()) return;

			// TODO: Copy texture to production directory.
			std::wstring path;
			std::wstring filename;
			std::wstring extension;
			Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(elem.second->GetData().icon.c_str(), path, filename, extension);
		
			path = Resources::CManager::Instance().GetProductionPath() + path.substr(wcslen(Resources::CManager::Instance().GetFilePath()));
			ASSERT_R(Util::CFileSystem::Instance().NewPath((path).c_str()));

			std::ifstream fileIn(elem.second->GetData().icon, std::ios::binary | std::ios::ate);
			ASSERT_R(fileIn.is_open());

			const size_t sz = fileIn.tellg();
			fileIn.seekg(0, std::ios::beg);
		
			std::vector<char> data(sz, '\0');
			fileIn.read(data.data(), sz);

			std::ofstream fileOut(path + filename + extension, std::ios::binary);
			ASSERT_R(fileOut.is_open());

			fileOut.write(data.data(), sz);

			fileOut.clear();
			fileIn.clear();
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Window adjustments.
	//-----------------------------------------------------------------------------------------------

	void CWUIScript::OnMove(const CPanel* pPanel) { }
	void CWUIScript::OnResize(const CPanel* pPanel) { }
};
