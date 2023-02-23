//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioVoice.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIOVOICE_H
#define CAUDIOVOICE_H

#include "CAudioData.h"

namespace Audio
{
	class CAudioVoice
	{
	public:
		CAudioVoice(class CAudio* pAudio) { }
		virtual ~CAudioVoice() { }
		CAudioVoice(const CAudioVoice&) = delete;
		CAudioVoice(CAudioVoice&&) = delete;
		CAudioVoice& operator = (const CAudioVoice&) = delete;
		CAudioVoice& operator = (CAudioVoice&&) = delete;

		virtual void Initialize(const AudioInfo& info) = 0;
		virtual void Reset() = 0;
		virtual void Submit(class CAudioSource* pSource, float volumeMul = 1.0f) = 0;
		virtual void SourceCallback(AUDIO_ACTION action) = 0;
		virtual void Release() = 0;

		// Accessors.
		virtual u64 GetHash() const = 0;

	private:
	};
};

#endif
