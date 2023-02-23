//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioLoader.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIOLOADER_H
#define CAUDIOLOADER_H

#include "CAudioData.h"
#include <string>
#include <functional>

namespace Audio
{
	class CAudioLoader
	{
	public:
		static const size_t STREAMING_BUFFER_COUNT = 4;
		static const size_t STREAMING_BUFFER_SIZE = 65536;

	public:
		static void Load(const std::wstring& filename, AudioInfo& audioInfo);
		static void Stream(class CAudioSource* pSource, std::function<bool(char*, size_t, bool)> streamReader, std::function<void()> streamFinal);

	private:
		static void LoadMetaData(const std::wstring& filename, AudioInfo& audioInfo);
	};
};

#endif
