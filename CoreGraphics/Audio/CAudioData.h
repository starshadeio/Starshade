//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIODATA_H
#define CAUDIODATA_H

#include <Globals/CGlobals.h>
#include <Utilities/CMemoryFree.h>

namespace Audio
{
	enum class AudioFormatType : u8
	{
		Unknown,
		Wave,
		Ogg,
	};

	struct AudioInfo
	{
		AudioFormatType type;
		bool bStream;

		u16 audioFormat;
		u16 bitsPerSample;
		u16 numChannels;
		u16 blockAlign;
		u32 sampleRate;
		u32 byteRate;
		u32 dataChunkSize;
		u32 dataOffset;

		u32 loopStart;
		u32 loopEnd;

		u64 hash;

		char* pBuffer;

		// Method for generating a 64-bit hash code from audio info.
		//	This is used for accessing the proper audio voice pool when playing audio sources.
		u64 GenerateHash()
		{
			hash = 0xcbf29ce484222325;
			auto AddValue = [this](u64 val){ hash = (hash ^ val) * 0x100000001B3; };

			AddValue(audioFormat);
			AddValue(sampleRate);
			AddValue(bitsPerSample);
			AddValue(numChannels);
			AddValue(blockAlign);
			AddValue(byteRate);

			return hash;
		}

		~AudioInfo()
		{
			SAFE_DELETE_ARRAY(pBuffer);
		}
	};

	enum AUDIO_ACTION
	{
		AUDIO_PLAY,
		AUDIO_RESUME,
		AUDIO_PAUSE,
		AUDIO_STOP,
	};
};

#endif
