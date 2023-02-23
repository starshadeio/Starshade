//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioWave.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIOWAVE_H
#define CAUDIOWAVE_H

#include "CAudioData.h"
#include <string>
#include <functional>

namespace Audio
{
	class CAudioWave
	{
	private:
		static const u32 WAVE_CHUNK_ID = 0x46464952;
		static const u32 WAVE_FORMAT = 0x45564157;
		static const u32 WAVE_JUNK = 0x4B4E554A;
		static const u32 WAVE_SUB_CHUNK_ID = 0x20746d66;
		static const u32 WAVE_LIST = 0x5453494C;
		static const u32 WAVE_FACT = 0x74636166;
		static const u32 WAVE_INFO = 0x4F464E49;
		static const u32 WAVE_INAM = 0x4D414E49;
		static const u32 WAVE_ISFT = 0x54465349;
		static const u32 WAVE_DATA_CHUNK_ID = 0x61746164;
		static const u32 WAVE_SAMPLE = 0x6C706D73;
		static const u32 WAVE_INSTRUMENT = 0x74736E69;
		static const u32 WAVE_ACID = 0x64696361;

	private:
#pragma pack(push, 1)
		struct HEADER
		{
			u32 chunkId; // 0x52494646 big-endian
			u32 chunkSize;
			u32 format; // 0x57415645 big-endian
			u32 subChunkId; // 0x666d7420 big-endian
			u32 subChunkSize;
			u16 audioFormat;
			u16 numChannels;
			u32 sampleRate;
			u32 byteRate;
			u16 blockAlign;
			u16 bitsPerSample;
			u32 dataChunkId; // 0x64617461 big-endian
			u32 dataChunkSize;
		};

		struct SAMPLE_HEADER
		{
			u32 manufacturer;
			u32 product;
			u32 samplePeriod;
			u32 MIDI_unityNote;
			u32 MIDI_pitchFraction;
			u32 SMPTE_format;
			u32 SMPTE_offset;
			u32 numSampleLoops;
			u32 sampleData;
		};

		struct SAMPLE_LOOP
		{
			u32 id;
			u32 type;
			u32 start;
			u32 end;
			u32 fraction;
			u32 loopCount;
		};

		struct INSTRUMENT_HEADER
		{
			u8 unshiftedNote;
			u8 fineTuning;
			u8 gain;
			u8 lowNote;
			u8 highNote;
			u8 lowVelocity;
			u8 highVelocity;
			u8 padding0;
		};

		struct ACID_HEADER
		{
			u32 type;
			u16 rootNote;
			u16 unknown0;
			u32 unknown1;
			u32 beatCount;
			u16 meterDenominator; // Denominator and numerator
			u16 meterNumerator; // might need to be swapped.
			u32 tempo;
		};
#pragma pack(pop)

	public:
		static void Load(const std::wstring filename, AudioInfo& audioInfo);
		static void Stream(class CAudioSource* pSource, size_t bufferSize, size_t bufferCount, 
			std::function<bool(char*, size_t, bool)> streamReader, std::function<void()> streamFinal);
	};
};

#endif
