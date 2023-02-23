//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioLoader.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CAudioLoader.h"
#include "CAudioSource.h"
#include "CAudioClip.h"
#include "CAudioWave.h"
#include "CAudioOgg.h"
#include "../Resources/CResourceManager.h"
#include <Utilities/CCompilerUtil.h>
#include <unordered_map>
#include <thread>
#include <fstream>

namespace Audio
{
	//-----------------------------------------------------------------------------------------------
	// Meta data utility methods.
	//-----------------------------------------------------------------------------------------------

	void SetIsStream(const std::string& contents, AudioInfo& audioInfo)
	{
		audioInfo.bStream = atoi(contents.c_str());
	}

	//-----------------------------------------------------------------------------------------------
	// Audio loader maps.
	//-----------------------------------------------------------------------------------------------

	static const std::unordered_map<std::wstring, AudioFormatType> AUDIO_FORMAT_MAP = {
		{ L"wav", AudioFormatType::Wave, },
		{ L"ogg", AudioFormatType::Ogg, },
	};

	static const std::unordered_map<std::string, std::function<void(const std::string&, AudioInfo&)>> AUDIO_META_MAP = {
		{ "STREAM", SetIsStream },
	};

	//-----------------------------------------------------------------------------------------------
	// Audio loader.
	//-----------------------------------------------------------------------------------------------

	void CAudioLoader::Load(const std::wstring& filename, AudioInfo& audioInfo)
	{
		{ // Extract file extension to run proper importer.
			size_t index = filename.length() - 1;
			while(filename[index] != '.')
			{
				--index;
			}

			std::wstring ext = filename.substr(index + 1);
			for(wchar_t& ch : ext) { ch = tolower(ch); }

			const auto& elem = AUDIO_FORMAT_MAP.find(ext);
			if(elem != AUDIO_FORMAT_MAP.end())
			{
				audioInfo.type = elem->second;
			}
			else
			{
				audioInfo.type = AudioFormatType::Unknown;
			}

			LoadMetaData(filename + Resources::META_DATA_EXTENSION, audioInfo);

			// Load data based on extension format.
			switch(audioInfo.type)
			{
				case AudioFormatType::Wave:
					CAudioWave::Load(filename, audioInfo);
					break;
				case AudioFormatType::Ogg:
					CAudioOgg::Load(filename, audioInfo);
					break;
				default:
					break;
			}

			audioInfo.GenerateHash();
		}
	}

	void CAudioLoader::LoadMetaData(const std::wstring& filename, AudioInfo& audioInfo)
	{
		std::ifstream file(filename.c_str());
		if(!file.is_open()) { return; }

		// Read audio meta data.
		std::string word;
		Util::CompilerTuple<2, std::string, char> line;

		while(!file.eof())
		{
			char ch = file.get();

			if(ch == '\n' || ch == -1)
			{
				if(!word.empty())
				{
					line.SetTarget(word);
					line.Continue();
					if(line.IsComplete())
					{
						// Process line.
						AUDIO_META_MAP.find(line.GetElement(0))->second(line.GetElement(1), audioInfo);
					}
				}

				line.Clear();
				word.clear();
			}
			else if(ch == '#')
			{
				// Read to next line.
				while(!file.eof() && file.peek() != '\n')
				{
					file.get();
				}
			}
			else
			{
				if(line.GetTargetIndex() == 0 && ch == ' ')
				{
					line.SetTarget(word);
					line.Continue();
					word.clear();
				}
				else
				{
					word += ch;
				}
			}
		}

		file.close();
	}

	//-----------------------------------------------------------------------------------------------
	// Audio streamer.
	//-----------------------------------------------------------------------------------------------

	void CAudioLoader::Stream(CAudioSource* pSource, std::function<bool(char*, size_t, bool)> streamReader, std::function<void()> streamFinal)
	{
		switch(pSource->GetClip()->GetInfo().type)
		{
			case AudioFormatType::Wave:
				CAudioWave::Stream(pSource, STREAMING_BUFFER_SIZE, STREAMING_BUFFER_COUNT, streamReader, streamFinal);
				break;
			case AudioFormatType::Ogg:
				CAudioOgg::Stream(pSource, STREAMING_BUFFER_SIZE, STREAMING_BUFFER_COUNT, streamReader, streamFinal);
				break;
			default:
				break;
		}
	}
};
