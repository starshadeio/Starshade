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

	bool CCommand::Execute(const void* params, bool bInverse, size_t sz, u16 align)
	{
		if(!m_command(params, bInverse, sz, align)) return false;
		for(auto& elem : m_actionList)
		{
			elem(params, bInverse, sz, align);
		}

		return true;
	}
	
	void CCommand::RegisterAction(Action action)
	{
		m_actionList.push_back(action);
	}
};
