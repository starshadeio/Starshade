//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CWinAudioVoice.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CWINAUDIOVOICE_H
#define CWINAUDIOVOICE_H

#include "CAudioVoice.h"
#include "CAudioSource.h"
#include <xaudio2.h>

namespace Audio
{
	struct AudioVoiceContext : IXAudio2VoiceCallback
	{
		HANDLE hBufferEndEvent;

		AudioVoiceContext(class CWinAudioVoice* pVoice);
		~AudioVoiceContext();
		
    // Called just before this voice's processing pass begins.
    __declspec(nothrow) void OnVoiceProcessingPassStart(UINT32 BytesRequired) final;

    // Called just after this voice's processing pass ends.
    __declspec(nothrow) void OnVoiceProcessingPassEnd() final;

    // Called when this voice has just finished playing a buffer stream
    // (as marked with the XAUDIO2_END_OF_STREAM flag on the last buffer).
    __declspec(nothrow) void OnStreamEnd() final;

    // Called when this voice is about to start processing a new buffer.
    __declspec(nothrow) void OnBufferStart(void* pBufferContext) final;

    // Called when this voice has just finished processing a buffer.
    // The buffer can now be reused or destroyed.
    __declspec(nothrow) void OnBufferEnd(void* pBufferContext) final;

    // Called when this voice has just reached the end position of a loop.
    __declspec(nothrow) void OnLoopEnd(void* pBufferContext) final;

    // Called in the event of a critical error during voice processing,
    // such as a failing xAPO or an error from the hardware XMA decoder.
    // The voice may have to be destroyed and re-created to recover from
    // the error.  The callback arguments report which buffer was being
    // processed when the error occurred, and its HRESULT code.
		__declspec(nothrow) void OnVoiceError(void* pBufferContext, HRESULT Error) final;

	private:
		class CWinAudioVoice* m_pVoice;
	};

	class CWinAudioVoice : public CAudioVoice
	{
	public:
		friend struct AudioVoiceContext;

	public:
		CWinAudioVoice(class CAudio* pAudio);
		~CWinAudioVoice();
		CWinAudioVoice(const CWinAudioVoice&) = delete;
		CWinAudioVoice(CWinAudioVoice&&) = delete;
		CWinAudioVoice& operator = (const CWinAudioVoice&) = delete;
		CWinAudioVoice& operator = (CWinAudioVoice&&) = delete;
		
		void Initialize(const AudioInfo& info) final;
		void Reset() final;
		void Submit(class CAudioSource* pSource, float volumeMul = 1.0f) final;
		void SourceCallback(AUDIO_ACTION action) final;
		void Release() final;

		// Accessors.
		u64 GetHash() const final { return m_info.hash; }

	private:
		void OnBufferEnd();

	private:
		Aflag m_steamFlag;
		Aflag m_steamEndFlag;

		AudioInfo m_info;

		AudioVoiceContext m_context;
		
		IXAudio2SourceVoice* m_pVoice;
		class CAudioSource* m_pAudioSource;
		class CWinAudio* m_pWinAudio;
	};
};

#endif
