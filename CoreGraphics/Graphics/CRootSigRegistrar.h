//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CRootSigRegistrar.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CROOTSIGREGISTRAR_H
#define CROOTSIGREGISTRAR_H

#include "CRootSignature.h"
#include <Utilities/CDSTrie.h>

namespace Graphics
{
	class CRootSigRegistrar
	{
	public:
		CRootSigRegistrar();
		~CRootSigRegistrar();
		CRootSigRegistrar(const CRootSigRegistrar&) = delete;
		CRootSigRegistrar(CRootSigRegistrar&&) = delete;
		CRootSigRegistrar& operator = (const CRootSigRegistrar&) = delete;
		CRootSigRegistrar& operator = (CRootSigRegistrar&&) = delete;

		void Release();

		CRootSignature* Get(const CRootSignature::Data& data);

	private:
		CRootSignature* CreateRootSignature();

		void OnDeleteElement(CRootSignature** ppRootSig);

	private:
		Util::CDSTrieGeneric<u32, CRootSignature*> m_lookUpTree;
	};
};

#endif
