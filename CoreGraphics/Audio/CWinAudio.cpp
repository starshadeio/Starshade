//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CWinAudio.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CWinAudio.h"
#include "CWinAudioVoice.h"
#include "CWinAudioGroup.h"
#include <Utilities/CDebugError.h>
#include <Utilities/CMemoryFree.h>
#include <cassert>

namespace Audio
{
	CWinAudio::CWinAudio() :
		m_pXAudio2(nullptr),
		m_pMasterVoice(nullptr) {
	}

	CWinAudio::~CWinAudio() { }

	void CWinAudio::Initialize()
	{
		ASSERT_HR_R(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
		ASSERT_HR_R(XAudio2Create(&m_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR));
		ASSERT_HR_R(m_pXAudio2->CreateMasteringVoice(&m_pMasterVoice));

		DWORD channelMask;
		ASSERT_HR_R(m_pMasterVoice->GetChannelMask(&channelMask));
		ASSERT_HR_R(X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, m_x3dInstance));

		{ // Setup the target wave format.
			memset(&m_wfx, 0, sizeof(m_wfx));
		}
	}

	void CWinAudio::Release()
	{
		m_pMasterVoice->DestroyVoice();
		SAFE_RELEASE(m_pXAudio2);
	}

	CAudioGroup* CWinAudio::CreateGroup()
	{
		return new CWinAudioGroup(this);
	}

	CAudioVoice* CWinAudio::CreateVoice()
	{
		return new CWinAudioVoice(this);
	}
};
