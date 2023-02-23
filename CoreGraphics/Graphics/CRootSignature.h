//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRootSignature.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CROOTSIGNATURE_H
#define CROOTSIGNATURE_H

#include "CGraphicsData.h"
#include <vector>

namespace Graphics
{
	class CRootSignature
	{
	public:
		struct Data
		{
			bool bCompute;
			ROOT_SIGNATURE_FLAGS flags;
			std::vector<DescriptorRange> rangeList;
			std::vector<RootParameter> paramList;
			std::vector<StaticSamplerDesc> samplerList;
		};

	protected:
		CRootSignature() { }
		CRootSignature(const CRootSignature&) = delete;
		CRootSignature(CRootSignature&&) = delete;
		CRootSignature& operator = (const CRootSignature&) = delete;
		CRootSignature& operator = (CRootSignature&&) = delete;

	public:
		virtual ~CRootSignature() { }

		virtual void Initialize() = 0;
		virtual void Bind() = 0;
		virtual void Release() = 0;

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	protected:
		Data m_data;
	};
};

#endif
