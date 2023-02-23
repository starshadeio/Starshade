//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CTimer.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CTimer.h"
#include <thread>

namespace Util
{
	CTimer::CTimer()
	{
		m_maxTimeStep = 1.0f / 3.0f;
		m_delta = 0.0f;
		m_smoothDelta = 0.0f;

		memset(&m_frameTime, 0, sizeof(float) * TIMER_SAMPLE_COUNT);
		m_sampleCount = 0;

		m_frameRate = 0;
		m_frameCount = 0;
		m_timeElapsed = 0.0f;
		m_time = 0.0f;
		m_smoothTime = 0.0f;

		m_targetFrameRate = 0.0f;
		m_targetFrameDelay = 0.0f;

		// Initialize the performance timer variables.
		m_lastTime = std::chrono::steady_clock::now();
		m_timeScale = 1.0f;
	}

	void CTimer::Tick()
	{
		auto currentTime = std::chrono::steady_clock::now();

		// Time elapsed since last tick.
		std::chrono::duration<float> timeCounter = currentTime - m_lastTime;
		float timeElapsed = timeCounter.count() * m_timeScale;

		if(m_targetFrameRate > 0.0f)
		{
			// Wait for target frame rate to be reached before moving on (only if 'm_targetFrameRate' > 0.0f).
			if(timeElapsed < m_targetFrameDelay)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>((m_targetFrameDelay - timeElapsed) * 1000)));

				currentTime = std::chrono::steady_clock::now();

				timeCounter = currentTime - m_lastTime;
				timeElapsed = timeCounter.count() * m_timeScale;
			}
		}

		m_lastTime = currentTime;

		// Update timers.
		if(fabsf(timeElapsed - m_delta) < m_maxTimeStep)
		{
			if(m_sampleCount < TIMER_SAMPLE_COUNT)
			{
				m_sampleCount++;
			}

			m_delta = timeElapsed;
			m_time += m_delta;

			// Update frame time buffer.
			for(int i = m_sampleCount - 1; i >= 1; --i)
			{
				m_frameTime[i] = m_frameTime[i - 1];
			}

			m_frameTime[0] = timeElapsed;

			// Calculate the smoothed out time delta.
			m_smoothDelta = 0.0f;

			for(u32 i = 0; i < m_sampleCount; ++i)
			{
				m_smoothDelta += m_frameTime[i];
			}

			if(m_sampleCount > 0)
			{
				m_smoothDelta /= m_sampleCount;
			}
			
			m_smoothTime += m_smoothDelta;
		}

		// Calculate frame rate.
		m_frameCount++;
		m_timeElapsed += timeElapsed;

		if(m_timeElapsed > 1.0f)
		{
			m_frameRate = m_frameCount;
			m_frameCount = 0;
			m_timeElapsed = 0.0f;
		}
	}
};
