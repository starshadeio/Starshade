//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CTimer.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CTIMER_H
#define CTIMER_H

#include "../Globals/CGlobals.h"
#include <chrono>

namespace Util
{
	const int TIMER_SAMPLE_COUNT = 10;

	class CTimer
	{
	private:
		CTimer();
		CTimer(const CTimer&) = delete;
		CTimer(CTimer&&) = delete;
		CTimer& operator = (const CTimer&) = delete;
		CTimer& operator = (CTimer&&) = delete;

	public:
		static CTimer& Instance()
		{
			static thread_local CTimer instance;
			return instance;
		}

		void Tick();

		//
		// Inline methods.
		//

		inline float GetDelta() const { return m_delta; }
		inline float GetTime() const { return m_time; }
		inline float GetSmoothDelta() const { return m_smoothDelta; }
		inline float GetSmoothTime() const { return m_smoothTime; }

		inline float GetTimeScale() const { return m_timeScale; }
		inline void SetTimeScale(float scale) { m_timeScale = scale; }

		inline float GetMaxTimeStep() const { return m_maxTimeStep; }
		inline void SetMaxTimeStep(float maxStep) { m_maxTimeStep = maxStep; }

		inline float GetTargetFrameRate() const { return m_targetFrameRate; }
		inline void SetTargetFrameRate(float targetFPS)
		{
			m_targetFrameRate = targetFPS;

			if(m_targetFrameRate > 0.0f)
			{
				m_targetFrameDelay = 1.0f / m_targetFrameRate;
			}
			else
			{
				m_targetFrameDelay = 0.0f;
			}
		}

	private:
		float m_timeScale;
		float m_maxTimeStep;
		float m_delta;
		float m_smoothDelta;

		std::chrono::steady_clock::time_point m_lastTime;

		float m_frameTime[TIMER_SAMPLE_COUNT];
		u32 m_sampleCount;

		u32 m_frameRate;
		u32 m_frameCount;
		float m_timeElapsed;
		float m_time;
		float m_smoothTime;

		float m_targetFrameRate;
		float m_targetFrameDelay;
	};
};

#endif
