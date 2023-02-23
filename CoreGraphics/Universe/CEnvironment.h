//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Universe/CEnvironment.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CENVIRONMENT_H
#define CENVIRONMENT_H

#include "../Audio/CAudioSource.h"

namespace Universe
{
	class CEnvironment
	{
	public:
		struct Data
		{
			bool bActive = true;
		};

	public:
		CEnvironment();
		~CEnvironment();
		CEnvironment(const CEnvironment&) = delete;
		CEnvironment(CEnvironment&&) = delete;
		CEnvironment& operator = (const CEnvironment&) = delete;
		CEnvironment& operator = (CEnvironment&&) = delete;

		void Initialize();
		void Update();
		void Release();

		void SetActive(bool bActive);

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		Data m_data;

		Audio::CAudioSource m_bgAudio;
	};
};

#endif
