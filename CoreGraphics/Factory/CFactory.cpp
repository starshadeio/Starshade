//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Factory/CFactory.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CFactory.h"

#ifdef PLATFORM_WINDOWS
#include "../Application/CWinPlatform.h"
#include "../Application/CWinPanel.h"
#include "../Application/CWinDeviceList.h"
#endif
#ifdef PLATFORM_LINUX
#endif

#ifdef GRAPHICS_DX12
#include "../Graphics/CDX12Graphics.h"
#include "../Graphics/CDX12Worker.h"
#include "../Graphics/CDX12ShaderCompiler.h"
#include "../Graphics/CDX12Shader.h"
#include "../Graphics/CDX12ConstantBuffer.h"
#include "../Graphics/CDX12UAVBuffer.h"
#include "../Graphics/CDX12Texture.h"
#include "../Graphics/CDX12MeshRenderer_.h"
#include "../Graphics/CDX12Compute.h"
#endif
#ifdef GRAPHICS_VULKAN
#endif

#include <Utilities/CMemoryFree.h>

CFactory::CFactory() :
	m_pPlatform(nullptr),
	m_pGraphics(nullptr),
	m_pShaderCompiler(nullptr)
{
}

CFactory::~CFactory() { }

void CFactory::Release()
{
	m_rootSigRegistrar.Release();

	SAFE_DELETE(m_pShaderCompiler);
	m_pGraphics->Sync();
	SAFE_RELEASE_DELETE(m_pPlatform);
	SAFE_RELEASE_DELETE(m_pGraphics);
}

//-------------------------------------------------------------------------------------------------
// Retrievers.
//-------------------------------------------------------------------------------------------------

App::CPlatform* CFactory::GetPlatform()
{
	if(m_pPlatform) { return m_pPlatform; }

#ifdef PLATFORM_WINDOWS
	m_pPlatform = new App::CWinPlatform();
#endif
#ifdef PLATFORM_LINUX
	m_pPlatform = new App::CLinuxPlatform();
#endif

	return m_pPlatform;
}

Graphics::CGraphicsAPI* CFactory::GetGraphicsAPI()
{
	if(m_pGraphics) { return m_pGraphics; }

#ifdef GRAPHICS_DX12
	m_pGraphics = new Graphics::CDX12Graphics();
#endif
#ifdef GRAPHICS_VULKAN
	m_pGraphics = new Graphics::CVKGraphics();
#endif

	return m_pGraphics;
}

Graphics::CShaderCompiler* CFactory::GetShaderCompiler()
{
	if(m_pShaderCompiler) { return m_pShaderCompiler; }

#ifdef GRAPHICS_DX12
	m_pShaderCompiler = new Graphics::CDX12ShaderCompiler();
#endif
#ifdef GRAPHICS_VULKAN
	m_pShaderCompiler = new Graphics::CVKShaderCompiler();
#endif

	return m_pShaderCompiler;
}

//-------------------------------------------------------------------------------------------------
// Creators.
//-------------------------------------------------------------------------------------------------

//
// Object creators.
//

App::CPanel* CFactory::CreatePanel()
{
#ifdef PLATFORM_WINDOWS
	return new App::CWinPanel();
#endif
#ifdef PLATFORM_LINUX
	return new App::CLinuxPanel();
#endif

	return nullptr;
}

App::CInputDeviceList* CFactory::CreateInputDeviceList()
{
#ifdef PLATFORM_WINDOWS
	return new App::CWinDeviceList();
#endif
#ifdef PLATFORM_LINUX
	return new App::CLinuxDeviceList();
#endif

	return nullptr;
}

Graphics::CShader* CFactory::CreateShader()
{
#ifdef GRAPHICS_DX12
	return new Graphics::CDX12Shader();
#endif
#ifdef GRAPHICS_VULKAN
	return new Graphics::CVKShader();
#endif

	return nullptr;
}

Graphics::CConstantBuffer* CFactory::CreateConstantBuffer()
{
#ifdef GRAPHICS_DX12
	return new Graphics::CDX12ConstantBuffer();
#endif
#ifdef GRAPHICS_VULKAN
	return new Graphics::CVKConstantBuffer();
#endif

	return nullptr;
}

Graphics::CUAVBuffer* CFactory::CreateUAVBuffer()
{
#ifdef GRAPHICS_DX12
	return new Graphics::CDX12UAVBuffer();
#endif
#ifdef GRAPHICS_VULKAN
	return new Graphics::CVKUAVBuffer();
#endif

	return nullptr;
}

Graphics::CTexture* CFactory::CreateTexture()
{
#ifdef GRAPHICS_DX12
	return new Graphics::CDX12Texture();
#endif
#ifdef GRAPHICS_VULKAN
	return new Graphics::CVKTexture();
#endif

	return nullptr;
}

//
// Component creators.
//

Graphics::CMeshRenderer_* CFactory::CreateMeshRenderer(const CVObject* pObject)
{
#ifdef GRAPHICS_DX12
	return new Graphics::CDX12MeshRenderer_(pObject);
#endif
#ifdef GRAPHICS_VULKAN
	return new Graphics::CVKMeshRenderer(pObject);
#endif

	return nullptr;
}

Graphics::CCompute* CFactory::CreateCompute(const CVObject* pObject)
{
#ifdef GRAPHICS_DX12
	return new Graphics::CDX12Compute(pObject);
#endif
#ifdef GRAPHICS_VULKAN
	return new Graphics::CVKCompute(pObject);
#endif

	return nullptr;
}


Graphics::CGraphicsWorker* CFactory::CreateGraphicsWorker()
{
#ifdef GRAPHICS_DX12
	return new Graphics::CDX12Worker();
#endif
#ifdef GRAPHICS_VULKAN
	return new Graphics::CVKWorker();
#endif

	return nullptr;
}
