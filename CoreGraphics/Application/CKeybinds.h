//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CKeybinds.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CKEYBINDS_H
#define CKEYBINDS_H

#include "CInputData.h"
#include <Utilities/CConfigFile.h>
#include <vector>

namespace App
{
	class CKeybinds
	{
	public:
		static const wchar_t INPUT_KEY_MOVE_FORWARD[];
		static const wchar_t INPUT_KEY_MOVE_BACKWARD[];
		static const wchar_t INPUT_KEY_MOVE_LEFT[];
		static const wchar_t INPUT_KEY_MOVE_RIGHT[];

		static const wchar_t INPUT_KEY_LOOK_H[];
		static const wchar_t INPUT_KEY_LOOK_V[];
	
		static const wchar_t INPUT_KEY_JUMP[];
		
		static const wchar_t INPUT_KEY_SELECT[];

	public:
		typedef std::function<bool(u32, const InputCallbackData&)> Processor;

	public:
		CKeybinds();
		virtual ~CKeybinds();
		CKeybinds(const CKeybinds&) = delete;
		CKeybinds(CKeybinds&&) = delete;
		CKeybinds& operator = (const CKeybinds&) = delete;
		CKeybinds& operator = (CKeybinds&&) = delete;

		void Initialize();

		// Modifiers.
		inline void AddProcessor(const Processor& processor) { m_processorList.push_back(processor); }

	protected:
		virtual void RegisterKeybinds();
		void RegisterKeybinding(const u32* pLayoutHashList, u32 layoutHashCount, const std::wstring& key, InputCallback func,
			const InputBinding& primary, const InputBinding& secondary);

		void Callback(const InputCallbackData& callback, InputCallback func);
		
	private:
		void ProcessConfig(const std::wstring& prop, const std::vector<std::vector<std::wstring>>& elemList);

	protected:
		u32 m_inputHashDefault;
		std::wstring m_configFilename;

	private:
		Util::CConfigFile m_configFile;
		std::vector<Processor> m_processorList;
	};
};

#endif
