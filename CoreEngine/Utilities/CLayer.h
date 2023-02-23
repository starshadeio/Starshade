//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CLayer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CLAYER_H
#define CLAYER_H

namespace Util
{
	class CLayer
	{
	public:
		static CLayer& Instance()
		{
			static CLayer instance;
			return instance;
		}

	private:
		CLayer() { }
		~CLayer() { }
		CLayer(const CLayer&) = delete;
		CLayer(CLayer&&) = delete;
		CLayer& operator = (const CLayer&) = delete;
		CLayer& operator = (CLayer&&) = delete;
	};
};

#endif
