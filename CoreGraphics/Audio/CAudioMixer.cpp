//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioMixer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAudioMixer.h"
#include "CAudioGroup.h"
#include "CAudioSource.h"
#include "CAudioClip.h"
#include "CAudioVoice.h"
#include <Utilities/CMemoryFree.h>
#include <Utilities/CDebugError.h>

#ifdef PLATFORM_WINDOWS
#include "CWinAudio.h"
#endif
#ifdef PLATFORM_LINUX
#endif

namespace Audio
{
	CAudioMixer::CAudioMixer() :
		m_pAudio(nullptr) {
	}

	CAudioMixer::~CAudioMixer() { }

	void CAudioMixer::Intialize()
	{
#ifdef PLATFORM_WINDOWS
		m_pAudio = new CWinAudio();
#endif
#ifdef PLATFORM_LINUX
		m_pAudio = new CLinuxAudio();
#endif

		m_pAudio->Initialize();
	}

	void CAudioMixer::PostInitialize()
	{
	}

	void CAudioMixer::Update()
	{
		{ // Pop through reset queue.
			CAudioVoice* pVoice;
			while(m_resetDeque.TryPopFront(pVoice))
			{
				pVoice->Reset();
				QueueVoice(pVoice->GetHash(), pVoice);
			}
		}

		{ // Pop through play queue.
			std::pair<CAudioSource*, float> pair;
			while(m_playDeque.TryPopFront(pair))
			{
				CAudioVoice* pVoice;

				auto& q = m_voicePoolMap[pair.first->GetClip()->GetInfo().hash];
				if(!q.empty())
				{
					pVoice = q.front();
					q.pop();
				}
				else
				{
					pVoice = m_pAudio->CreateVoice();
					pVoice->Initialize(pair.first->GetClip()->GetInfo());
				}

				pVoice->Submit(pair.first, pair.second);
			}
		}
	}

	void CAudioMixer::Release()
	{
		for(auto& elem : m_groupMap)
		{
			SAFE_RELEASE_DELETE(elem.second);
		}

		SAFE_RELEASE_DELETE(m_pAudio);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Mixer group methods.
	//-----------------------------------------------------------------------------------------------

	void CAudioMixer::Play(CAudioSource* pSource, float volumeMul)
	{
		std::pair<CAudioSource*, float> pair = { pSource, volumeMul };
		m_playDeque.PushBack(pair);
	}
	
	void CAudioMixer::QueueVoice(u64 hash, class CAudioVoice* pVoice)
	{
		m_voicePoolMap[hash].push(pVoice);
	}
	
	//-----------------------------------------------------------------------------------------------
	// Mixer group methods.
	//-----------------------------------------------------------------------------------------------

	void CAudioMixer::CreateAudioGroup(u32 key)
	{
		ASSERT(m_groupMap.insert({ key, m_pAudio->CreateGroup() }).second);
	}
	
	//-----------------------------------------------------------------------------------------------
	// (De)registration methods.
	//-----------------------------------------------------------------------------------------------

	void CAudioMixer::RegisterSource(CAudioSource* pSource)
	{
	}

	void CAudioMixer::DeregisterSource(CAudioSource* pSource)
	{
	}
};
