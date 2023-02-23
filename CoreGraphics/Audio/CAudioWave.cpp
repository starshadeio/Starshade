//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioWave.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAudioWave.h"
#include "CAudioSource.h"
#include "CAudioClip.h"
#include <fstream>
#include <cassert>
#include <thread>

namespace Audio
{
	void CAudioWave::Load(const std::wstring filename, AudioInfo& audioInfo)
	{
		std::ifstream file(filename, std::ios::binary);
		assert(file.is_open());

		HEADER header;

		file.read(reinterpret_cast<char*>(&header.chunkId), sizeof(header.chunkId));
		assert(header.chunkId == WAVE_CHUNK_ID);
		file.read(reinterpret_cast<char*>(&header.chunkSize), sizeof(header.chunkSize));

		file.read(reinterpret_cast<char*>(&header.format), sizeof(header.format));
		file.read(reinterpret_cast<char*>(&header.subChunkId), sizeof(header.subChunkId));

		if(header.subChunkId == WAVE_JUNK)
		{
			u32 sz;
			file.read(reinterpret_cast<char*>(&sz), sizeof(sz));
			file.seekg(sz, std::ios::cur);

			file.read(reinterpret_cast<char*>(&header.subChunkId), sizeof(header.subChunkId));
		}

		assert(header.subChunkId == WAVE_SUB_CHUNK_ID);

		file.read(reinterpret_cast<char*>(&header.subChunkSize), sizeof(header.subChunkSize));
		file.read(reinterpret_cast<char*>(&header.audioFormat), sizeof(header.audioFormat));
		assert(header.audioFormat == 1 || header.audioFormat == 3); // PCM is 1, other values mean compression.

		file.read(reinterpret_cast<char*>(&header.numChannels), sizeof(header.numChannels));
		file.read(reinterpret_cast<char*>(&header.sampleRate), sizeof(header.sampleRate));
		file.read(reinterpret_cast<char*>(&header.byteRate), sizeof(header.byteRate));
		file.read(reinterpret_cast<char*>(&header.blockAlign), sizeof(header.blockAlign));
		file.read(reinterpret_cast<char*>(&header.bitsPerSample), sizeof(header.bitsPerSample));

		file.read(reinterpret_cast<char*>(&header.dataChunkId), sizeof(header.dataChunkId));
		if(header.dataChunkId != WAVE_DATA_CHUNK_ID)
		{
			// There is extra header data, though this is still a PCM format.
			u32 listSz;
			file.read(reinterpret_cast<char*>(&listSz), sizeof(listSz));

			file.seekg(listSz, std::ios::cur);
			file.read(reinterpret_cast<char*>(&header.dataChunkId), sizeof(header.dataChunkId));
			assert(header.dataChunkId == WAVE_DATA_CHUNK_ID);
		}

		file.read(reinterpret_cast<char*>(&header.dataChunkSize), sizeof(header.dataChunkSize));

		audioInfo.audioFormat = header.audioFormat;
		audioInfo.sampleRate = header.sampleRate;
		audioInfo.byteRate = header.byteRate;
		audioInfo.bitsPerSample = header.bitsPerSample;
		audioInfo.numChannels = header.numChannels;
		audioInfo.blockAlign = header.blockAlign;
		audioInfo.dataChunkSize = header.dataChunkSize;
		audioInfo.dataOffset = static_cast<u32>(file.tellg());

		if(!audioInfo.bStream)
		{
			audioInfo.pBuffer = new char[header.dataChunkSize];
			file.read(audioInfo.pBuffer, header.dataChunkSize);
		}

		file.close();
	}

	void CAudioWave::Stream(CAudioSource* pSource, size_t bufferSize, size_t bufferCount, std::function<bool(char*, size_t, bool)> streamReader, std::function<void()> streamFinal)
	{
		const bool bLoop = pSource->GetData().bLoop;
		const u32 dataOffset = pSource->GetClip()->GetInfo().dataOffset;
		const u32 dataSize = pSource->GetClip()->GetInfo().dataChunkSize;
		const u32 bytesPerSample = pSource->GetClip()->GetInfo().blockAlign;
		const u32 loopStart = pSource->GetClip()->GetInfo().loopStart * bytesPerSample;
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

			std::ifstream file(pSource->GetClip()->GetFilename(), std::ios::binary);
			assert(file.is_open());
			file.seekg(dataOffset, std::ios::beg);
		
			size_t bufferOffset = 0;
			size_t readPos = 0;
			while(readPos < endPos)
			{
				const size_t readTarget = std::min(bufferSize, endPos - readPos);
				file.read(pBufferList + bufferOffset, readTarget);
				size_t readCount = file.gcount();
				readPos += readCount;

				const bool bEnd = readPos >= endPos;
				if(!streamReader(pBufferList + bufferOffset, readCount, !bLoop && bEnd))
				{
					break;
				}

				if(bEnd && bLoop)
				{
					file.seekg(dataOffset, std::ios::beg + loopStart);
					readPos = loopStart;
				}

				if((bufferOffset += bufferSize) >= bufferSize * bufferCount)
				{
					bufferOffset = 0;
				}
			}

			streamFinal();
		
			file.close();
			delete [] pBufferList;
		});

		thread.detach();
	}
};
