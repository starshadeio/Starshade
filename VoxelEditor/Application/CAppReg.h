//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CAppReg.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAPPREG_H
#define CAPPREG_H

namespace App
{
	class CAppReg
	{
	public:
		CAppReg();
		~CAppReg();
		CAppReg(const CAppReg&) = delete;
		CAppReg(CAppReg&&) = delete;
		CAppReg& operator = (const CAppReg&) = delete;
		CAppReg& operator = (CAppReg&&) = delete;

	private:
	};
};

#endif
