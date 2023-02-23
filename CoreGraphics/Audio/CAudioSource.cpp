//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioSource.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAudioSource.h"
#include "CAudioVoice.h"
#include "../Resources/CResourceManager.h"
#include "../Application/CSceneManager.h"

namespace Audio
{
	CAudioSource::CAudioSource(const CVObject* pObject) :
		CVComponent(pObject),
		m_pClip(nullptr),
		m_pVoice(nullptr) {
	}

	CAudioSource::~CAudioSource() { }

	void CAudioSource::Initialize()
	{
		App::CSceneManager::Instance().AudioMixer().RegisterSource(this);
		m_pClip = reinterpret_cast<CAudioClip*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_AUDIO, m_data.audioClip));
	}

	void CAudioSource::Play(float volumeMul)
	{
		if(m_pVoice)
		{
			m_pVoice->SourceCallback(AUDIO_PLAY);
		}

		App::CSceneManager::Instance().AudioMixer().Play(this, volumeMul);
	}
	
	void CAudioSource::Resume()
	{
		if(m_pVoice)
		{
			m_pVoice->SourceCallback(AUDIO_RESUME);
		}
	}

	void CAudioSource::Pause()
	{
		if(m_pVoice)
		{
			m_pVoice->SourceCallback(AUDIO_PAUSE);
		}
	}

	void CAudioSource::Stop()
	{
		if(m_pVoice)
		{
			m_pVoice->SourceCallback(AUDIO_STOP);
		}
	}

	void CAudioSource::Release()
	{
		App::CSceneManager::Instance().AudioMixer().DeregisterSource(this);
	}
};
