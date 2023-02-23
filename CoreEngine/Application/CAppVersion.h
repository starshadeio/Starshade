//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Application/CAppVersion.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAPPVERSION_H
#define CAPPVERSION_H

#include <string>
#include <sstream>
#include <iomanip>
#include "../Globals/CGlobals.h"

namespace App
{
	class CVersion
	{
	public:
		union VersionBlock
		{
			struct
			{
				u16 majorVerion;
				u8 minorVersion;
				u8 patch : 6;
				u8 branch : 2;
			};

			u32 version;
		};

	public:
		static CVersion& Instance()
		{
			static CVersion instance;
			return instance;
		}

		// Accessors.
		inline void SetVersion(u16 majorVersion, u8 minorVersion, u8 patch, u8 branch, std::wstring branchTitle)
		{
			m_versionBlock = { majorVersion, minorVersion, patch, branch };
			m_branchTitle = branchTitle;
		}

		inline void SetPreviousVersion(u32 version)
		{
			m_prevVersionBlock.version = version;
		}

		// Modifiers.
		inline u32 GetVersion() const { return m_versionBlock.version; }
		inline u32 GetPreviousVersion() const { return m_prevVersionBlock.version; }
		inline bool VersionMismatch() const { return m_versionBlock.version != m_prevVersionBlock.version; }

		inline std::wstring GetAsString() const
		{
			std::wstringstream wss;
			wss << L" v" << m_versionBlock.majorVerion << L'.';
			wss << std::setfill(L'0') << std::setw(2) << (m_versionBlock.minorVersion + 1);
			wss << static_cast<wchar_t>(L'a' + m_versionBlock.patch);
			wss << L" [" << m_branchTitle << L"]";
			return wss.str();
		}

	private:
		VersionBlock m_versionBlock;
		VersionBlock m_prevVersionBlock;
		std::wstring m_branchTitle;
	};
};

#endif
