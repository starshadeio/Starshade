//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CWinAudioGroup.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef PLATFORM_WINDOWS

#include "CWinAudioGroup.h"
#include "CWinAudio.h"
#include "../Application/CSceneManager.h"
#include <Utilities/CDebugError.h>

namespace Audio
{
	CWinAudioGroup::CWinAudioGroup(class CAudio* pAudio) :
		CAudioGroup(pAudio),
		m_pSubmix(nullptr),
		m_pAudio(dynamic_cast<CWinAudio*>(pAudio))
	{
	}

	CWinAudioGroup::~CWinAudioGroup()
	{
	}
	
	void CWinAudioGroup::Initialize()
	{
		ASSERT_HR_R(m_pAudio->GetAudio()->CreateSubmixVoice(&m_pSubmix, 2, 44100, 0, 0, 0, 0));

		m_sfxSend = {0, m_pSubmix};
		m_sfxSendList = {1, &m_sfxSend};
	}

	void CWinAudioGroup::Release()
	{
		m_pSubmix->DestroyVoice();
		m_pSubmix = nullptr;
	}
};

#endif
