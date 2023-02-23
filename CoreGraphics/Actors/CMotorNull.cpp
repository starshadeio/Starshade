//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CMotorNull.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMotorNull.h"
#include <Math/CMathFNV.h>
#include <unordered_map>
#include <functional>

namespace Actor
{
	const u32 CMotorNull::MOTOR_HASH = Math::FNV1a_32("MOTOR_NULL");

	CMotorNull::CMotorNull(const CVObject* pObject) :
		CMotor(pObject, MOTOR_HASH)
	{
	}

	CMotorNull::~CMotorNull()
	{
	}
	
	void CMotorNull::Load()
	{
		CMotor::Load();
	}
	
	void CMotorNull::Unload()
	{
		CMotor::Unload();
	}
};
