//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Physics/CForceField.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CFORCEFIELD_H
#define CFORCEFIELD_H

#include "../Objects/CVComponent.h"
#include "../Math/CSIMDVector.h"
#include "../Math/CSIMDMatrix.h"
#include "../Logic/CTransform.h"

namespace Physics
{
	class CForceField : public CVComponent
	{
	public:
		CForceField(const CVObject* pObject) : CVComponent(pObject) { }
		virtual ~CForceField() { }

		CForceField(const CForceField&) = delete;
		CForceField(CForceField&&) = delete;
		CForceField& operator = (const CForceField&) = delete;
		CForceField& operator = (CForceField&&) = delete;

		void PhysicsUpdate() { }

	private:
	};
};

#endif
