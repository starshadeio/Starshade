//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel App
//
// File: Application/CAppKeybinds.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAPPKEYBINDS_H
#define CAPPKEYBINDS_H

#include <Application/CKeybinds.h>

namespace App
{
	class CAppKeybinds : public CKeybinds
	{
	public:
		static const wchar_t INPUT_KEY_QUIT[];
		static const wchar_t INPUT_KEY_SCREENSHOT[];

	public:
		CAppKeybinds();
		~CAppKeybinds();
		CAppKeybinds(const CAppKeybinds&) = delete;
		CAppKeybinds(CAppKeybinds&&) = delete;
		CAppKeybinds& operator = (const CAppKeybinds&) = delete;
		CAppKeybinds& operator = (CAppKeybinds&&) = delete;

	protected:
		void RegisterKeybinds() final;

	private:
		void Quit(const App::InputCallbackData& callback);
		void Screenshot(const App::InputCallbackData& callback);

	private:
	};
};

#endif
