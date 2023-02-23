//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioGroup.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIOGROUP_H
#define CAUDIOGROUP_H

namespace Audio
{
	class CAudioGroup
	{
	public:
		CAudioGroup(class CAudio* pAudio) { }
		virtual ~CAudioGroup() { }
		CAudioGroup(const CAudioGroup&) = delete;
		CAudioGroup(CAudioGroup&&) = delete;
		CAudioGroup& operator = (const CAudioGroup&) = delete;
		CAudioGroup& operator = (CAudioGroup&&) = delete;
		
		virtual void Initialize() = 0;
		virtual void Release() = 0;
	};
};

#endif
