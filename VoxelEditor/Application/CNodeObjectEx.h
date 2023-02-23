//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CNodeObjectEx.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODEOBJECTEX_H
#define CNODEOBJECTEX_H

#include <Globals/CGlobals.h>
#include <Objects/CNodeObject.h>
#include <string>
#include <fstream>

class CNodeObjectEx : public CNodeObject
{
public:
	CNodeObjectEx(const std::wstring& name, u32 viewHash = 0, u32 layer = 0, u32 tagHash = 0);
	virtual ~CNodeObjectEx();
	CNodeObjectEx(const CNodeObjectEx&) = delete;
	CNodeObjectEx(CNodeObjectEx&&) = delete;
	CNodeObjectEx& operator = (const CNodeObjectEx&) = delete;
	CNodeObjectEx& operator = (CNodeObjectEx&&) = delete;

	void New();

	// Accessors.
	inline const std::wstring& GetName() const { return m_name; }

private:
	std::wstring m_name;
};

#endif
