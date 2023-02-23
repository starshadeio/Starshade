//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioListener.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIOLISTENER_H
#define CAUDIOLISTENER_H

#include <Objects/CVComponent.h>

namespace Audio
{
	class CAudioListener : public CVComponent
	{
	public:
		CAudioListener(const CVObject* pObject) : CVComponent(pObject) { }
		~CAudioListener() { }
		CAudioListener(const CAudioListener&) = delete;
		CAudioListener(CAudioListener&&) = delete;
		CAudioListener& operator = (const CAudioListener&) = delete;
		CAudioListener& operator = (CAudioListener&&) = delete;

	private:
	};
};

#endif
