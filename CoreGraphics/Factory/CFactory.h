//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Factory/CFactory.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CFACTORY_H
#define CFACTORY_H

#include "../Graphics/CRootSigRegistrar.h"

class CVObject;

namespace App
{
	class CPlatform;
	class CPanel;
	class CInputDeviceList;
};

namespace Graphics
{
	class CGraphicsAPI;
	class CGraphicsWorker;
	class CShaderCompiler;
	class CShader;
	class CConstantBuffer;
	class CUAVBuffer;
	class CTexture;

	class CMeshRenderer_;
	class CCompute;
};

class CFactory
{
private:
	CFactory();
	~CFactory();
	CFactory(const CFactory&) = delete;
	CFactory(CFactory&&) = delete;
	CFactory& operator = (const CFactory&) = delete;
	CFactory& operator = (CFactory&&) = delete;

public:
	static CFactory& Instance()
	{
		static CFactory instance;
		return instance;
	}

	void Release();

	App::CPlatform* GetPlatform();
	Graphics::CGraphicsAPI* GetGraphicsAPI();
	Graphics::CShaderCompiler* GetShaderCompiler();

	App::CPanel* CreatePanel();
	App::CInputDeviceList* CreateInputDeviceList();
	Graphics::CShader* CreateShader();
	Graphics::CConstantBuffer* CreateConstantBuffer();
	Graphics::CUAVBuffer* CreateUAVBuffer();
	Graphics::CTexture* CreateTexture();

	Graphics::CMeshRenderer_* CreateMeshRenderer(const CVObject* pObject);
	Graphics::CCompute* CreateCompute(const CVObject* pObject);
	Graphics::CGraphicsWorker* CreateGraphicsWorker();

	// Accessors.
	inline Graphics::CRootSignature* GetRootSignature(const Graphics::CRootSignature::Data& data)
	{
		return m_rootSigRegistrar.Get(data);
	}

private:
	Graphics::CRootSigRegistrar m_rootSigRegistrar;

	App::CPlatform* m_pPlatform;
	Graphics::CGraphicsAPI* m_pGraphics;
	Graphics::CShaderCompiler* m_pShaderCompiler;
};

#endif
