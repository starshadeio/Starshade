//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CWinAudio.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CWINAUDIO_H
#define CWINAUDIO_H

#include "CAudio.h"
#include <xaudio2.h>
#include <x3daudio.h>

#pragma comment(lib, "xaudio2.lib")

namespace Audio
{
	class CWinAudio : public CAudio
	{
	public:
		CWinAudio();
		~CWinAudio();
		CWinAudio(const CWinAudio&) = delete;
		CWinAudio(CWinAudio&&) = delete;
		CWinAudio& operator = (const CWinAudio&) = delete;
		CWinAudio& operator = (CWinAudio&&) = delete;

		void Initialize() final;
		void Release() final;

		class CAudioGroup* CreateGroup() final;
		class CAudioVoice* CreateVoice() final;

		inline IXAudio2* GetAudio() const { return m_pXAudio2; }
		inline IXAudio2MasteringVoice* GetMasterVoice() const { return m_pMasterVoice; }
		inline const X3DAUDIO_HANDLE& Getx3dInstance() const { return m_x3dInstance; }

	private:
		WAVEFORMATEXTENSIBLE m_wfx;

		X3DAUDIO_HANDLE m_x3dInstance;
		IXAudio2* m_pXAudio2;
		IXAudio2MasteringVoice* m_pMasterVoice;
	};
};

#endif
