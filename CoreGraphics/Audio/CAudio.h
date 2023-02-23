//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudio.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIO_H
#define CAUDIO_H

class CVObject;

namespace Audio
{
	class CAudio
	{
	public:
		CAudio();
		virtual ~CAudio();
		CAudio(const CAudio&) = delete;
		CAudio(CAudio&&) = delete;
		CAudio& operator = (const CAudio&) = delete;
		CAudio& operator = (CAudio&&) = delete;

		virtual void Initialize() = 0;
		virtual void Release() = 0;

		virtual class CAudioGroup* CreateGroup() = 0;
		virtual class CAudioVoice* CreateVoice() = 0;

	protected:
	};
};

#endif
