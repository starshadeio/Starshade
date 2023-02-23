//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Application/CAppBase.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAPPBASE_H
#define CAPPBASE_H

namespace App
{
	class CAppBase
	{
	protected:
		CAppBase();
		virtual ~CAppBase();
		CAppBase(const CAppBase&) = delete;
		CAppBase(CAppBase&&) = delete;
		CAppBase& operator = (const CAppBase&) = delete;
		CAppBase& operator = (CAppBase&&) = delete;

	public:
		virtual void Create();
		virtual void Run();
		virtual void Destroy();

	private:
	};
};

#endif
