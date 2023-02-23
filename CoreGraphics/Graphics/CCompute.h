//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CCompute.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCOMPUTE_H
#define CCOMPUTE_H

#include <Objects/CVComponent.h>
#include <Globals/CGlobals.h>

namespace Graphics
{
	class CCompute : public CVComponent
	{
	public:
		friend class CComputeSystem;

	public:
		struct Data
		{
			u32 threadX;
			u32 threadY;
			u32 threadZ;
			class CMaterial* pMaterial;
		};

	protected:
		CCompute(const CVObject* pObject);
		CCompute(const CCompute&) = delete;
		CCompute(CCompute&&) = delete;
		CCompute& operator = (const CCompute&) = delete;
		CCompute& operator = (CCompute&&) = delete;

	public:
		virtual ~CCompute();

		void Initialize() override;
		void Release() override;

		// Accessors.
		inline const Data& GetData() const { return m_data; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	protected:
		virtual void PostInitialize() = 0;
		virtual void Dispatch();

	protected:
		Data m_data;
	};
};

#endif
