//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CArchive.h
//
//-------------------------------------------------------------------------------------------------

#include "CArchive.h"
#include "CFileSystem.h"

namespace Util
{
	CArchive::CArchive()
	{
	}

	CArchive::~CArchive()
	{
	}

	// Method for adding an additional data path to the archive trie.
	void CArchive::Insert(const std::wstring& archivePath, const std::wstring& dataPath)
	{
		std::vector<std::wstring> pathSplit;
		std::wstring folder;
		for(wchar_t ch : archivePath)
		{
			if(ch == L'/' || ch == L'\\')
			{
				if(folder.size())
				{
					pathSplit.push_back(std::move(folder));
				}
			}
			else
			{
				folder += ch;
			}
		}

		if(folder.size())
		{
			pathSplit.push_back(std::move(folder));
		}

		m_trie.AddSequence(pathSplit, dataPath);
	}

	// Methods for saving an archive to a file.
	void CArchive::Save(std::ofstream& file) const
	{
		if(m_trie.GetHead() == nullptr) return;
		std::wstring path;
		Save(file, path, m_trie.GetHead());
	}

	void CArchive::Save(std::ofstream& file, std::wstring& path, Util::CDSTrieGeneric<std::wstring, std::wstring>::Node* pNode) const
	{
		path += pNode->prefix;

		if(pNode->bTerminal)
		{
			std::ifstream dataFile(path, std::ios::binary | std::ios::ate);

			if(dataFile.is_open())
			{
				u32 pathSize = static_cast<u32>(pNode->value.size() * sizeof(wchar_t));
				file.write(reinterpret_cast<char*>(&pathSize), sizeof(pathSize));
				file.write(reinterpret_cast<char*>(pNode->value.data()), pathSize);

				u32 fileSize = static_cast<u32>(dataFile.tellg());
				file.write(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));

				dataFile.seekg(0, std::ios::beg);
				
				static const u32 bufferSz = 4096;
				char buffer[bufferSz];
				while(dataFile.read(buffer, bufferSz))
				{
					file.write(buffer, bufferSz);
				}

				file.write(buffer, dataFile.gcount());
			}

			dataFile.close();
		}
		else
		{
			if(path.size()) path.push_back(L'\\');

			for(auto child : pNode->children)
			{
				Save(file, path, child);
			}

			if(path.size()) path.pop_back();
		}

		while(!path.empty() && path.back() != L'/' && path.back() != L'\\')
		{
			path.pop_back();
		}
	}

	// Methods for loading and extracting an archive from a file.
	void CArchive::Load(const std::wstring& path, std::ifstream& file)
	{
		static const u32 bufferSz = 4096;
		char buffer[bufferSz];
		u32 sz;

		while(file.read(reinterpret_cast<char*>(&sz), sizeof(sz)))
		{
			file.read(buffer, sz);

			std::wstring relFile(reinterpret_cast<wchar_t*>(buffer), reinterpret_cast<wchar_t*>(buffer + sz));
			std::wstring relDir;
			std::wstring filename;
			std::wstring extension;

			CFileSystem::Instance().SplitDirectoryFilenameExtension(relFile.c_str(), relDir, filename, extension);
			CFileSystem::Instance().NewPath((path + relDir).c_str());

			std::ofstream fileOut(path + relDir + filename + extension, std::ios::binary);
		
			file.read(reinterpret_cast<char*>(&sz), sizeof(sz));
			u32 targetRead = std::min(sz, bufferSz);
			while(targetRead && file.read(buffer, targetRead))
			{
				fileOut.write(buffer, targetRead);
				sz -= targetRead;
				targetRead = std::min(sz, bufferSz);
			}

			if(targetRead)
			{
				fileOut.write(buffer, file.gcount());
			}

			fileOut.close();
		}

		return;
	}
};
