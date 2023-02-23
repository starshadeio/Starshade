//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CArchive.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CARCHIVE_H
#define CARCHIVE_H

#include "../Globals/CGlobals.h"
#include "CDSTrie.h"
#include <string>
#include <fstream>

namespace Util
{
	class CArchive
	{
	public:
		CArchive();
		~CArchive();
		CArchive(const CArchive&) = delete;
		CArchive(CArchive&&) = delete;
		CArchive& operator = (const CArchive&) = delete;
		CArchive& operator = (CArchive&&) = delete;

		void Insert(const std::wstring& archivePath, const std::wstring& dataPath);

		void Save(std::ofstream& file) const;
		void Load(const std::wstring& path, std::ifstream& file);

	private:
		void Save(std::ofstream& file, std::wstring& path, Util::CDSTrieGeneric<std::wstring, std::wstring>::Node* pNode) const;

	private:
		CDSTrieGeneric<std::wstring, std::wstring> m_trie;
	};
};

#endif
