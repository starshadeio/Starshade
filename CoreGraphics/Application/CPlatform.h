//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CPlatform.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPLATFORM_H
#define CPLATFORM_H

#include "CPlatformData.h"
#include "CPanel.h"
#include "CWUIScript.h"
#include <vector>
#include <string>

namespace Graphics
{
	class CGraphicsAPI;
};

namespace App
{
	class CPlatform
	{
	public:
		struct Data
		{
			std::wstring wuiFilename;
		};

	protected:
		CPlatform();
		CPlatform(const CPlatform&) = delete;
		CPlatform(CPlatform&&) = delete;
		CPlatform& operator = (const CPlatform&) = delete;
		CPlatform& operator = (CPlatform&&) = delete;

	public:
		virtual ~CPlatform();

		virtual int Run(class CAppBase* pApp, void* pData) { m_pApp = pApp; return 0; }

		virtual void UnlockCursor() = 0;
		virtual void CenterCursor(const class CPanel* pPanel) = 0;
		virtual void LockCursor(const class CPanel* pPanel) = 0;
		virtual void ConfineCursor(const class CPanel* pPanel) = 0;
		virtual void SetCursorVisibility(bool bShow) = 0;

		virtual MessageBoxState PostMessageBox(const wchar_t* msg, const wchar_t* caption, MessageBoxType type) const = 0;

		virtual void Quit(int exitCode = 0) = 0;

		void SaveProductionScripts();

		// Accessors.
		inline const std::wstring& GetWUIFilename() const { return m_mData.wuiFilename; }

		inline const size_t GetMonitorCount() const { return m_monitorDataList.size(); }
		inline const MonitorData& GetMonitor(size_t index) const { return m_monitorDataList[index]; }

		inline const CPanel* GetMainPanel() const { return m_wuiScript.GetMainPanel(); }
		inline const CPanel* GetPanel(const std::wstring& key) const { return m_wuiScript.GetPanel(key); }

	protected:
		void Initialize(const Data& baseData);
		void Frame(bool bInFocus);

		bool OnQuit(int exitCode);

	private:
		void LoadAssets();
		void Compute();
		void Render();
		void PostRender();

	public:
		void OnMove(const CPanel* pPanel);
		void OnResize(const CPanel* pPanel);

	public:
		void Release();

	protected:
		std::vector<MonitorData> m_monitorDataList;
		CWUIScript m_wuiScript;
		Graphics::CGraphicsAPI* m_pGraphicsAPI;
		class CAppBase* m_pApp;

	private:
		Data m_mData;
	};
};

#endif
