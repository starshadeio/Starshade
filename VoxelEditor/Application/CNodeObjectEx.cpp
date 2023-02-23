//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CNodeObjectEx.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CNodeObjectEx.h"
#include <Logic/CNodeTransform.h>
#include <Math/CMathFNV.h>
#include <Math/CMathVector3.h>
#include <fstream>
#include <cassert>

CNodeObjectEx::CNodeObjectEx(const std::wstring& name, u32 viewHash, u32 layer, u32 tagHash) :
	CNodeObject(Math::FNV1a_64(name.c_str(), name.size()), viewHash, layer, tagHash),
	m_name(name)
{
}

CNodeObjectEx::~CNodeObjectEx()
{
}

void CNodeObjectEx::New()
{

}
