//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CWinAudioVoice.cpp
//
//-------------------------------------------------------------------------------------------------

#ifdef PLATFORM_WINDOWS

#include "CWinAudioVoice.h"
#include "CWinAudio.h"
#include "CAudioClip.h"
#include "CAudioLoader.h"
#include "../Application/CSceneManager.h"
#include "../Resources/CResourceManager.h"
#include <Math/CMathFNV.h>
#include <Utilities/CDebugError.h>

namespace Audio
{
	//-----------------------------------------------------------------------------------------------
	// AudioVoiceContext
	//-----------------------------------------------------------------------------------------------
	
	AudioVoiceContext::AudioVoiceContext(class CWinAudioVoice* pVoice) : 
		m_pVoice(pVoice),
		hBufferEndEvent(CreateEvent(NULL, FALSE, FALSE, NULL)) { }

	AudioVoiceContext::~AudioVoiceContext() { CloseHandle(hBufferEndEvent); }
		
  __declspec(nothrow) void AudioVoiceContext::OnVoiceProcessingPassStart(UINT32 BytesRequired) { }

  __declspec(nothrow) void AudioVoiceContext::OnVoiceProcessingPassEnd() { }

  __declspec(nothrow) void AudioVoiceContext::OnStreamEnd() { }

  __declspec(nothrow) void AudioVoiceContext::OnBufferStart(void* pBufferContext) { }

  __declspec(nothrow) void AudioVoiceContext::OnBufferEnd(void* pBufferContext) 
	{
		SetEvent(hBufferEndEvent);
		m_pVoice->OnBufferEnd();
	}

  __declspec(nothrow) void AudioVoiceContext::OnLoopEnd(void* pBufferContext) { }
	__declspec(nothrow) void AudioVoiceContext::OnVoiceError(void* pBufferContext, HRESULT Error) { }

	//-----------------------------------------------------------------------------------------------
	// CWinAudioVoice
	//-----------------------------------------------------------------------------------------------

	CWinAudioVoice::CWinAudioVoice(CAudio* pAudio) :
		CAudioVoice(pAudio),
		m_context(this),
		m_pVoice(nullptr),
		m_pAudioSource(nullptr),
		m_pWinAudio(dynamic_cast<CWinAudio*>(pAudio))
	{
	}

	CWinAudioVoice::~CWinAudioVoice()
	{
	}
		
	void CWinAudioVoice::Initialize(const AudioInfo& info)
	{
		m_info = info;

		WAVEFORMATEX wfx;
		memset(&wfx, 0, sizeof(wfx));
		wfx.wFormatTag = info.audioFormat;
		wfx.nSamplesPerSec = info.sampleRate;
		wfx.wBitsPerSample = (info.bitsPerSample + 0xF) & ~0xF;
		wfx.nChannels = info.numChannels;
		wfx.nBlockAlign = info.blockAlign;
		wfx.nAvgBytesPerSec = info.byteRate;
		
		ASSERT_HR_R(m_pWinAudio->GetAudio()->CreateSourceVoice(&m_pVoice, &wfx, 0, 2.0f, &m_context));
	}

	void CWinAudioVoice::Reset()
	{
		if(m_pAudioSource)
		{
			m_pAudioSource->ResetVoice(this);
			m_pAudioSource = nullptr;
		}
	}

	void CWinAudioVoice::Submit(CAudioSource* pSource, float volumeMul)
	{
		m_pVoice->SetVolume(pSource->GetData().volume * volumeMul);

		m_info = pSource->GetClip()->GetInfo();
		if(!m_info.bStream)
		{
			m_steamEndFlag.test_and_set(std::memory_order_acquire);

			XAUDIO2_BUFFER buffer;
			memset(&buffer, 0, sizeof(buffer));
			buffer.LoopCount = pSource->GetData().bLoop ? XAUDIO2_LOOP_INFINITE : 0;
			buffer.LoopBegin = (m_info.loopStart) * pSource->GetData().bLoop;
			buffer.LoopLength = (m_info.loopEnd - m_info.loopStart) * pSource->GetData().bLoop;
			buffer.AudioBytes = m_info.dataChunkSize;
			buffer.pAudioData = (const BYTE*)m_info.pBuffer;
			buffer.Flags = XAUDIO2_END_OF_STREAM;

			ASSERT_HR_R(m_pVoice->SubmitSourceBuffer(&buffer));

			ASSERT_HR_R(m_pVoice->Start());
		}
		else
		{
			m_steamFlag.test_and_set(std::memory_order_acquire);
			m_steamEndFlag.clear();

			ASSERT_HR_R(m_pVoice->Start());
			CAudioLoader::Stream(pSource, [&](char* pBuffer, size_t sz, bool bEnd){
				
				// Wait for source to be ready.
				XAUDIO2_VOICE_STATE state;
				while(m_pVoice->GetState(&state), state.BuffersQueued >= CAudioLoader::STREAMING_BUFFER_COUNT - 1) {
					WaitForSingleObject(m_context.hBufferEndEvent, INFINITE);
				}

				// Submit new buffer.
				XAUDIO2_BUFFER buf = { 0 };
				buf.AudioBytes = static_cast<UINT32>(sz);
				buf.pAudioData = (const BYTE*)pBuffer;
				if(bEnd) {
					buf.Flags = XAUDIO2_END_OF_STREAM;
				}

				m_pVoice->SubmitSourceBuffer(&buf);

				return m_steamFlag.test(std::memory_order_relaxed);

			}, [this]() {
				// Wait for source to be final.
				XAUDIO2_VOICE_STATE state;
				while(m_pVoice->GetState(&state), state.BuffersQueued > 0) {
					WaitForSingleObject(m_context.hBufferEndEvent, INFINITE);
				}

				App::CSceneManager::Instance().AudioMixer().ResetVoice(this);
				m_steamEndFlag.test_and_set(std::memory_order_acquire);
			});
		}

		m_pAudioSource = pSource;
		m_pAudioSource->SetVoice(this);
	}

	void CWinAudioVoice::SourceCallback(AUDIO_ACTION action)
	{
		switch(action)
		{
			case Audio::AUDIO_PLAY:
			case Audio::AUDIO_RESUME:
				ASSERT_HR_R(m_pVoice->Start());
				break;
			case Audio::AUDIO_PAUSE:
				ASSERT_HR_R(m_pVoice->Stop(XAUDIO2_PLAY_TAILS));
				break;
			case Audio::AUDIO_STOP:
				ASSERT_HR_R(m_pVoice->Stop(XAUDIO2_PLAY_TAILS));
				ASSERT_HR_R(m_pVoice->FlushSourceBuffers());
				m_steamFlag.clear();
				break;
			default:
				break;
		}
	}

	void CWinAudioVoice::Release()
	{
		SourceCallback(AUDIO_ACTION::AUDIO_STOP);
		while(!m_steamFlag.test(std::memory_order_relaxed)) 
			{ std::this_thread::yield(); }

		m_pVoice->DestroyVoice();
		m_pVoice = nullptr;
	}

	//-----------------------------------------------------------------------------------------------
	// Internal callback methods.
	//-----------------------------------------------------------------------------------------------

	void CWinAudioVoice::OnBufferEnd()
	{
		if(m_info.bStream) { return; }

		XAUDIO2_VOICE_STATE state;
		if(m_pVoice->GetState(&state), state.BuffersQueued == 0)
		{
			App::CSceneManager::Instance().AudioMixer().ResetVoice(this);
		}
	}
};

#endif