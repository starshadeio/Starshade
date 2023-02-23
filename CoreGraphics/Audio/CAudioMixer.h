//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioMixer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIOMIXER_H
#define CAUDIOMIXER_H

#include <Utilities/CTSDeque.h>
#include <Globals/CGlobals.h>
#include <unordered_map>
#include <queue>

namespace Audio
{
	class CAudioMixer
	{
	public:
		struct Data
		{
			u32 maxVoices = 16;
		};

	public:
		CAudioMixer();
		~CAudioMixer();
		CAudioMixer(const CAudioMixer&) = delete;
		CAudioMixer(CAudioMixer&&) = delete;
		CAudioMixer& operator = (const CAudioMixer&) = delete;
		CAudioMixer& operator = (CAudioMixer&&) = delete;

		void Intialize();
		void PostInitialize();
		void Update();
		void Release();

		void Play(class CAudioSource* pSource, float volumeMul = 1.0f);

		void CreateAudioGroup(u32 key);

		void RegisterSource(class CAudioSource* pSource);
		void DeregisterSource(class CAudioSource* pSource);
		
		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void ResetVoice(class CAudioVoice* pVoice) { m_resetDeque.PushBack(pVoice); }

	private:
		void QueueVoice(u64 hash, class CAudioVoice* pVoice);

	private:
		Data m_data;

		Util::CTSDeque<std::pair<class CAudioSource*, float>> m_playDeque;
		Util::CTSDeque<class CAudioVoice*> m_resetDeque;

		std::unordered_map<u32, class CAudioGroup*> m_groupMap;
		std::unordered_map<u64, std::queue<class CAudioVoice*>> m_voicePoolMap;

		class CAudio* m_pAudio;
	};
};

#endif
