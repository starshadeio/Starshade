//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel App
//
// File: Application/CNodeLoader.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODELEADER_H
#define CNODELEADER_H

#include <Application/CNode.h>

namespace App
{
	class CNodeLoader
	{
	public:
		CNodeLoader();
		~CNodeLoader();
		CNodeLoader(const CNodeLoader&) = delete;
		CNodeLoader(CNodeLoader&&) = delete;
		CNodeLoader& operator = (const CNodeLoader&) = delete;
		CNodeLoader& operator = (CNodeLoader&&) = delete;

		void Initialize();
		void PostInitialize();

	private:
		CNode m_rootNode;
	};
};

#endif
