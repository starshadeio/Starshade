//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CCommand.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CCommand.h"

namespace App
{
	CCommand::CCommand(u64 key, Command cmd, const Properties& props) :
		m_key(key),
		m_command(cmd),
		m_properties(props)
	{
	}

	CCommand::~CCommand()
	{
	}

	bool CCommand::Execute(const void* params, bool bInverse)
	{
		if(!m_command(params, bInverse)) return false;
		for(auto& elem : m_actionList)
		{
			elem(params, bInverse);
		}

		return true;
	}
	
	void CCommand::RegisterAction(Action action)
	{
		m_actionList.push_back(action);
	}
};
