//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMaterial.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMaterial.h"
#include "CShader.h"
#include "CConstantBuffer.h"
#include "CUAVBuffer.h"
#include "CTexture.h"
#include "../Factory/CFactory.h"
#include "../Resources/CResourceManager.h"
#include <Utilities/CFileSystem.h>
#include <Utilities/CMemoryFree.h>
#include <Utilities/CDebugError.h>

namespace Graphics
{
	CMaterial::CMaterial() :
		m_pRawBufferData(nullptr),
		m_pBufferDataList(nullptr),
		m_pConstantBufferList(nullptr),
		m_pUAVBufferList(nullptr),
		m_pTextureList(nullptr),
		m_pShader(nullptr) {
	}

	CMaterial::~CMaterial() { }

	void CMaterial::ParseFile()
	{
		CMaterialFile::Data data { };
		data.pMaterial = this;
		m_materialFile.SetData(data);
#ifdef PRODUCTION_BUILD
		LoadProductionFile();
#endif

		m_materialFile.ParseFile(m_data.filename);
	}

	void CMaterial::SaveProductionFile()
	{
		std::wstring path;
		std::wstring filename;
		std::wstring extension;
		Util::CFileSystem::Instance().SplitDirectoryFilenameExtension(m_data.filename.c_str(), path, filename, extension);

		path = Resources::CManager::Instance().GetProductionPath() + path.substr(wcslen(Resources::CManager::Instance().GetFilePath()));
		ASSERT_R(Util::CFileSystem::Instance().NewPath((path).c_str()));

		std::ofstream file(path + filename + extension, std::ios::binary);
		ASSERT_R(file.is_open());

		size_t sz = m_materialFile.GetCode().size();
		file.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
		file.write(m_materialFile.GetCode().data(), sizeof(char) * sz);

		file.close();
	}
	
	void CMaterial::LoadProductionFile()
	{
		std::ifstream file(m_data.filename, std::ios::binary);
		ASSERT_R(file.is_open());

		size_t sz;
		file.read(reinterpret_cast<char*>(&sz), sizeof(sz));
		std::string code;
		code.resize(sz);
		file.read(code.data(), sizeof(char) * sz);
		m_materialFile.SetCode(std::move(code));

		file.close();
	}

	// Method for building data buffers for use with shaders.
	void CMaterial::BuildBuffers(CShader* pShader)
	{
		m_pShader = pShader;

		{ // Allocate for constant buffers.
			u32 totalSize = 0;
			for(u32 i = 0; i < m_pShader->GetCBVDataCount(); ++i)
			{
				totalSize += m_pShader->GetCBVData(i).size;
			}

			m_pRawBufferData = new u8[totalSize];
			memset(m_pRawBufferData, 0, totalSize);
			m_pBufferDataList = new u8 * [m_pShader->GetCBVDataCount()];
			u8* pNext = m_pRawBufferData;

			m_pConstantBufferList = new CConstantBuffer * [m_pShader->GetCBVDataCount()];
			for(u32 i = 0; i < m_pShader->GetCBVDataCount(); ++i)
			{
				m_pConstantBufferList[i] = CFactory::Instance().CreateConstantBuffer();

				const auto& cbvData = m_pShader->GetCBVData(i);

				m_pBufferDataList[i] = pNext;
				pNext += cbvData.size;
			}
		}

		{ // Allocate for uav buffers.
			m_pUAVBufferList = new CUAVBuffer * [m_pShader->GetUAVDataCount()];
			memset(m_pUAVBufferList, 0, sizeof(void*) * m_pShader->GetUAVDataCount());
		}

		{ // Allocate for textures.
			m_pTextureList = new CTexture * [m_pShader->GetTextureDataCount()];
			memset(m_pTextureList, 0, sizeof(void*) * m_pShader->GetTextureDataCount());
		}
	}

	void CMaterial::Initialize()
	{
		// Initialize constant buffers.
		for(u32 i = 0; i < m_pShader->GetCBVDataCount(); ++i)
		{
			const auto& cbvData = m_pShader->GetCBVData(i);

			CConstantBuffer::Data data { };
			data.bFixed = cbvData.bFixed;
			data.bRootConstant = cbvData.type == CBT_ROOT_CONSTANTS;
			data.bufferSize = cbvData.size;
			data.pBufferData = m_pBufferDataList[i];

			m_pConstantBufferList[i]->SetData(data);
			m_pConstantBufferList[i]->Initialize();
		}
	}

	void CMaterial::PostInitialize()
	{
		for(u32 i = 0; i < m_pShader->GetCBVDataCount(); ++i)
		{
			m_pConstantBufferList[i]->PostInitialize();
		}
	}

	void CMaterial::Bind()
	{
		m_pShader->Bind();

		u32 index = 0;
		u32 i, j;

		// Bind constant buffers.
		for(i = 0; i < m_pShader->GetCBVDataCount(); ++i)
		{
			for(j = 0; j < m_pShader->GetCBVData(i).visibilityCount; j++)
			{
				m_pConstantBufferList[i]->Bind(index, m_pShader->IsCompute());
			}
		}

		// Bind UAVs.
		for(i = 0; i < m_pShader->GetUAVDataCount(); ++i)
		{
			if(m_pUAVBufferList[i] == nullptr) { index += m_pShader->GetUAVData(i).visibilityCount; continue; }
			for(j = 0; j < m_pShader->GetUAVData(i).visibilityCount; j++)
			{
				m_pUAVBufferList[i]->Bind(index, m_pShader->IsCompute());
			}
		}

		// Bind textures.
		for(i = 0; i < m_pShader->GetTextureDataCount(); ++i)
		{
			if(m_pTextureList[i] == nullptr) { index += m_pShader->GetTextureData(i).visibilityCount; continue; }
			for(j = 0; j < m_pShader->GetTextureData(i).visibilityCount; j++)
			{
				m_pTextureList[i]->Bind(index, m_pShader->IsCompute());
			}
		}
	}

	void CMaterial::Release()
	{
		SAFE_DELETE_ARRAY(m_pTextureList);

		for(u32 i = 0; i < m_pShader->GetCBVDataCount(); ++i)
		{
			SAFE_RELEASE_DELETE(m_pConstantBufferList[i]);
		}

		SAFE_DELETE_ARRAY(m_pConstantBufferList);

		SAFE_DELETE_ARRAY(m_pBufferDataList);
		SAFE_DELETE_ARRAY(m_pRawBufferData);
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	void CMaterial::SetValue(u32 bufferHash, u32 valueHash, const std::string& value)
	{
		const u32 bufferIndex = m_pShader->GetCBVIndex(bufferHash);
		u8* pPtr = m_pBufferDataList[bufferIndex] + m_pShader->GetCBVData(bufferIndex).offsetMap.find(valueHash)->second;

		std::string var = "";
		for(u32 i = 0; i <= value.size(); ++i)
		{
			if(i == value.size() || value[i] == ',')
			{
				if(var[var.length() - 1] == 'f')
				{
					*(float*)pPtr = float(atof(var.c_str()));
					pPtr += sizeof(float);
				}
				else
				{
					*(int*)pPtr = atoi(var.c_str());
					pPtr += sizeof(int);
				}
				var.clear();
			}
			else
			{
				if(value[i] <= 0x20) { continue; }
				var += value[i];
			}
		}

		m_pConstantBufferList[bufferIndex]->SetAsDirty();
	}

	void CMaterial::SetFloat(u32 bufferHash, u32 valueHash, const float* pFlt, size_t elemCount)
	{
		const u32 bufferIndex = m_pShader->GetCBVIndex(bufferHash);
		u8* pPtr = m_pBufferDataList[bufferIndex] + m_pShader->GetCBVData(bufferIndex).offsetMap.find(valueHash)->second;

		memcpy(pPtr, pFlt, sizeof(float) * elemCount);
		m_pConstantBufferList[bufferIndex]->SetAsDirty();
	}

	void CMaterial::SetInteger(u32 bufferHash, u32 valueHash, const int* pInt, size_t elemCount)
	{
		const u32 bufferIndex = m_pShader->GetCBVIndex(bufferHash);
		u8* pPtr = m_pBufferDataList[bufferIndex] + m_pShader->GetCBVData(bufferIndex).offsetMap.find(valueHash)->second;

		memcpy(pPtr, pInt, sizeof(int) * elemCount);
		m_pConstantBufferList[bufferIndex]->SetAsDirty();
	}

	void CMaterial::SetUAV(u32 uavHash, class CUAVBuffer* pUAV)
	{
		const u32 index = m_pShader->GetUAVIndex(uavHash);
		m_pUAVBufferList[index] = pUAV;
	}

	void CMaterial::SetTexture(u32 textureHash, CTexture* pTexture)
	{
		const u32 index = m_pShader->GetTextureIndex(textureHash);
		m_pTextureList[index] = pTexture;
	}
};
