//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CEnvironment.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CEnvironment.h"
#include <Math/CMathFNV.h>

namespace Universe
{
	CEnvironment::CEnvironment() : 
		m_bgAudio(nullptr)
	{
	}

	CEnvironment::~CEnvironment()
	{
	}

	void CEnvironment::Initialize()
	{
		{ // Setup the audio.
			Audio::CAudioSource::Data data { };
			data.bLoop = true;
			data.volume = 0.5f;
			data.audioClip = Math::FNV1a_64(L"AUDIO_WINDY_DAY");
			m_bgAudio.SetData(data);
			m_bgAudio.Initialize();

			if(m_data.bActive)
			{
				m_bgAudio.Play();
			}
		}
	}

	void CEnvironment::Update()
	{
	}

	void CEnvironment::Release()
	{
		m_bgAudio.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CEnvironment::SetActive(bool bActive)
	{
		if(m_data.bActive == bActive) return;
		m_data.bActive = bActive;

		if(m_data.bActive)
		{
			if(m_bgAudio.HasVoice())
			{
				m_bgAudio.Resume();
			}
			else
			{
				m_bgAudio.Play();
			}
		}
		else
		{
			m_bgAudio.Pause();
		}
	}
};
