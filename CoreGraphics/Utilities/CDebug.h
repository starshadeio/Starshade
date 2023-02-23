//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Utilities/CDebug.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDEBUG_H
#define CDEBUG_H

#include "../Graphics/CDebugDraw.h"

namespace Util
{
	class CDebug
	{
	public:
		static CDebug& Instance()
		{
			static CDebug instance;
			return instance;
		}

	private:
		CDebug();
		~CDebug();
		CDebug(const CDebug&) = delete;
		CDebug(CDebug&&) = delete;
		CDebug& operator = (const CDebug&) = delete;
		CDebug& operator = (CDebug&&) = delete;

	public:
		void Initialize();
		void Release();

		inline Graphics::CDebugDraw& Drawer() { return m_debugDraw; }

	private:
		Graphics::CDebugDraw m_debugDraw;
	};
};

#endif
