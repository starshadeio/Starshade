//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CResourceFile.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CRESOURCEFILE_H
#define CRESOURCEFILE_H

#include <Globals/CGlobals.h>
#include <Utilities/CResScript.h>
#include <string>

namespace Resources
{
	class CResourceFile
	{
	public:
		struct Data
		{
			Util::CResScript::Data resData;
		};

	public:
		CResourceFile();
		~CResourceFile();
		CResourceFile(const CResourceFile&) = delete;
		CResourceFile(CResourceFile&&) = delete;
		CResourceFile& operator = (const CResourceFile&) = delete;
		CResourceFile& operator = (CResourceFile&&) = delete;

		void Initialize();
		
		// Accessors.
		inline const Util::CResScript& GetResScript() const { return m_resScript; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		Data m_data;
		Util::CResScript m_resScript;
	};
};

#endif
