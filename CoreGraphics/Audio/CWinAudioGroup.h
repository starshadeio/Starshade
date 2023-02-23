//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CWinAudioGroup.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CWINAUDIOGROUP_H
#define CWINAUDIOGROUP_H

#include "CAudioGroup.h"
#include <xaudio2.h>

namespace Audio
{
	class CWinAudioGroup : public CAudioGroup
	{
	public:
		CWinAudioGroup(class CAudio* pAudio);
		virtual ~CWinAudioGroup();
		CWinAudioGroup(const CWinAudioGroup&) = delete;
		CWinAudioGroup(CWinAudioGroup&&) = delete;
		CWinAudioGroup& operator = (const CWinAudioGroup&) = delete;
		CWinAudioGroup& operator = (CWinAudioGroup&&) = delete;
		
		void Initialize() final;
		void Release() final;

		// Accessors.
		inline const XAUDIO2_VOICE_SENDS* GetSendList() const { return &m_sfxSendList; }

	private:
		XAUDIO2_SEND_DESCRIPTOR m_sfxSend;
		XAUDIO2_VOICE_SENDS m_sfxSendList;

		IXAudio2SubmixVoice* m_pSubmix;

		class CWinAudio* m_pAudio;
	};
};

#endif
