//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CTag.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CTAG_H
#define CTAG_H

namespace Util
{
	class CTag
	{
	public:
		static CTag& Instance()
		{
			static CTag instance;
			return instance;
		}

	private:
		CTag() { }
		~CTag() { }
		CTag(const CTag&) = delete;
		CTag(CTag&&) = delete;
		CTag& operator = (const CTag&) = delete;
		CTag& operator = (CTag&&) = delete;
	};
};

#endif
