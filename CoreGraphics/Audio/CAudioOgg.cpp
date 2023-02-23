//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioOgg.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAudioOgg.h"
#include "CAudioSource.h"
#include "CAudioClip.h"

#include <fstream>
#include <stdio.h>
#include <cassert>
#include <thread>

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace Audio
{
	void CAudioOgg::Load(const std::wstring filename, AudioInfo& audioInfo)
	{
		int err;
		OggVorbis_File vf;

		FILE* pFile;
		_wfopen_s(&pFile, filename.c_str(), L"rb");
		assert(pFile);

		err = ov_open_callbacks(pFile, &vf, nullptr, 0, OV_CALLBACKS_NOCLOSE);
		assert(err == 0);

		{ // Gather file info.
			vorbis_info* vi = ov_info(&vf, -1);
			audioInfo.sampleRate = vi->rate;
			audioInfo.numChannels = vi->channels;
			audioInfo.bitsPerSample = 16;
			audioInfo.audioFormat = 1;
			audioInfo.blockAlign = audioInfo.numChannels * 2;
			audioInfo.byteRate = audioInfo.sampleRate * audioInfo.blockAlign;
			audioInfo.dataOffset = 0;
			audioInfo.dataChunkSize = static_cast<u32>(ov_pcm_total(&vf, -1)) * audioInfo.blockAlign;
		}

		if(!audioInfo.bStream)
		{ // Read audio data.
			audioInfo.pBuffer = new char[audioInfo.dataChunkSize];

			u32 dataOffset = 0;
			char buffer[4096];
			int read;
			int currentSection;
			do
			{
				read = ov_read(&vf, buffer, 4096, 0, 2, 1, &currentSection);
				if(read > 0)
				{
					// Simply copy the PCM data into the audio buffer.
					//	There might be need for more care taken for property changes in the stream down the line.
					memcpy(audioInfo.pBuffer + dataOffset, buffer, read);
					dataOffset += read;
				}
			} while(read > 0);
		}

		err = ov_clear(&vf);
		assert(err == 0);

		fclose(pFile);
	}

	void CAudioOgg::Stream(CAudioSource* pSource, size_t bufferSize, size_t bufferCount, std::function<bool(char*, size_t, bool)> streamReader, std::function<void()> streamFinal)
	{
		const bool bLoop = pSource->GetData().bLoop;
		const u32 dataOffset = pSource->GetClip()->GetInfo().dataOffset;
		const u32 dataSize = pSource->GetClip()->GetInfo().dataChunkSize;
		const u32 bytesPerSample = pSource->GetClip()->GetInfo().blockAlign;
		const u32 loopStartSmp = pSource->GetClip()->GetInfo().loopStart;
		const u32 loopStart = pSource->GetClip()->GetInfo().loopStart * bytesPerSample;
		const u32 loopEndSmp = pSource->GetClip()->GetInfo().loopEnd;
		const u32 loopEnd = pSource->GetClip()->GetInfo().loopEnd * bytesPerSample;
		
		u32 endPos;
		if(bLoop && loopEnd > loopStart)
		{
			endPos = loopEnd;
		}
		else
		{
			endPos = dataSize;
		}
		
		std::thread thread([=](){
			char* pBufferList = new char[bufferCount * bufferSize];

			int err;
			OggVorbis_File vf;

			FILE* pFile;
			_wfopen_s(&pFile, pSource->GetClip()->GetFilename().c_str(), L"rb");
			assert(pFile);

			err = ov_open_callbacks(pFile, &vf, nullptr, 0, OV_CALLBACKS_NOCLOSE);
			assert(err == 0);
			
			int currentSection;
			size_t bufferOffset = 0;
			size_t readPos = 0;
			while(readPos < endPos)
			{
				const size_t readTarget = std::min(bufferSize, endPos - readPos);
				size_t readCount = 0;

				while(readCount < readTarget)
				{
					long rdCount = ov_read(&vf, pBufferList + bufferOffset + readCount, static_cast<int>(readTarget - readCount), 0, 2, 1, &currentSection);
					if(rdCount <= 0) break;

					readCount += static_cast<size_t>(rdCount);
				}

				readPos += readCount;

				const bool bEnd = readPos >= endPos;
				if(!streamReader(pBufferList + bufferOffset, readCount, !bLoop && bEnd))
				{
					break;
				}

				if(bEnd && bLoop)
				{
					ov_pcm_seek(&vf, static_cast<ogg_int64_t>(loopStartSmp));
					readPos = loopStart;
				}

				if((bufferOffset += bufferSize) >= bufferSize * bufferCount)
				{
					bufferOffset = 0;
				}
			}

			streamFinal();

			err = ov_clear(&vf);
			assert(err == 0);

			fclose(pFile);

			delete [] pBufferList;
		});

		thread.detach();
	}
};
