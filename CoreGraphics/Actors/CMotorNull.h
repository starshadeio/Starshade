//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CMotorNull.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMOTORNULL_H
#define CMOTORNULL_H

#include "CMotor.h"

namespace Actor
{
	class CMotorNull : public CMotor
	{
	public:
		static const u32 MOTOR_HASH;

	public:
		struct Data : BaseData
		{
			Data()
			{
				BaseData::bUsePhysics = false;
			}
		};

	public:
		CMotorNull(const CVObject* pObject);
		~CMotorNull();
		CMotorNull(const CMotorNull&) = delete;
		CMotorNull(CMotorNull&&) = delete;
		CMotorNull& operator = (const CMotorNull&) = delete;
		CMotorNull& operator = (CMotorNull&&) = delete;

		void Load() final;
		void Unload() final;

	protected:
		inline const BaseData& GetBaseData() const final { return m_data; }

	private:
		Data m_data;
	};
};

#endif
