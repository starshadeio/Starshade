//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CNodeEx.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODEEX_H
#define CNODEEX_H

#include "CNodeVersion.h"
#include <Application/CNode.h>
#include <string>
#include <unordered_map>
#include <cassert>
#include <functional>
#include <fstream>

namespace App
{
	class CNodeEx : public CNode
	{
	public:
		CNodeEx(const wchar_t* pName, u32 viewHash = 0);
		~CNodeEx();
		CNodeEx(const CNodeEx&) = delete;
		CNodeEx(CNodeEx&&) = delete;
		CNodeEx& operator = (const CNodeEx&) = delete;
		CNodeEx& operator = (CNodeEx&&) = delete;

	public:
		void New();
		void Save();
		void Load();
		void Build() const;

	protected:
		CNodeObject* CreateNodeObject(const wchar_t* name) final;

	private:
		App::CNodeVersion m_version;
	};
};

#endif
