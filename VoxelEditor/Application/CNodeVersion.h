//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CNodeVersion.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODEVERSION_H
#define CNODEVERSION_H

#include <Globals/CGlobals.h>
#include <string>

namespace App
{
	class CNodeVersion
	{
	public:
		CNodeVersion();
		~CNodeVersion();
		CNodeVersion(const CNodeVersion&) = delete;
		CNodeVersion(CNodeVersion&&) = delete;
		CNodeVersion& operator = (const CNodeVersion&) = delete;
		CNodeVersion& operator = (CNodeVersion&&) = delete;

		void Save(const std::wstring& path);
		void Load(const std::wstring& path);

		// Accessors.
		inline u32 GetVersion() const { return m_version; }

	private:
		u32 m_version;
	};
};

#endif
