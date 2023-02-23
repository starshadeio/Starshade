//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Audio/CAudioSource.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CAUDIOSOURCE_H
#define CAUDIOSOURCE_H

#include <Objects/CVComponent.h>

namespace Logic
{
	class CTransform;
};

namespace Audio
{
	enum class AudioCurveType
	{
		Logarithmic,
		Linear,
		SmoothStep,
	};

	class CAudioSource : public CVComponent
	{
	public:
		struct Data
		{
			bool bLoop = false;
			AudioCurveType curveType = AudioCurveType::Logarithmic;
			float volume = 1.0f;
			float pitch = 1.0f;
			float innerRadius = 1.0f;
			float distanceScale = 1.0f;
			float spatialBlend = 1.0f;
			float spread = 1.0f;
			float dopplerScale = 0.0f;

			u64 audioClip;
		};

	public:
		CAudioSource(const CVObject* pObject);
		~CAudioSource();
		CAudioSource(const CAudioSource&) = delete;
		CAudioSource(CAudioSource&&) = delete;
		CAudioSource& operator = (const CAudioSource&) = delete;
		CAudioSource& operator = (CAudioSource&&) = delete;

		void Initialize() final;

		virtual void Play(float volumeMul = 1.0f);
		virtual void Resume();
		virtual void Pause();
		virtual void Stop();

		void Release() final;

		// Accessors.
		inline const Data& GetData() const { return m_data; }
		inline const class CAudioClip* GetClip() const { return m_pClip; }

		inline bool HasVoice() { return m_pVoice; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetVoice(class CAudioVoice* pVoice) { m_pVoice = pVoice; }
		inline void ResetVoice(class CAudioVoice* pVoice) { if(m_pVoice == pVoice) m_pVoice = nullptr; }

	private:
		Data m_data;

		class CAudioClip* m_pClip;
		class CAudioVoice* m_pVoice;
	};
};

#endif
