//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CMaterial.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATERIAL_H
#define CMATERIAL_H

#include "CMaterialFile.h"
#include <Globals/CGlobals.h>
#include <string>

namespace Graphics
{
	class CMaterial
	{
	public:
		struct Data
		{
			std::wstring filename;
		};

	public:
		CMaterial();
		~CMaterial();
		CMaterial(const CMaterial&) = delete;
		CMaterial(CMaterial&&) = delete;
		CMaterial& operator = (const CMaterial&) = delete;
		CMaterial& operator = (CMaterial&&) = delete;

		void ParseFile();
		void SaveProductionFile();
		void LoadProductionFile();
		void BuildBuffers(class CShader* pShader);
		void Initialize();
		void PostInitialize();
		void Bind();
		void Release();

		void SetValue(u32 bufferHash, u32 valueHash, const std::string& value);
		void SetFloat(u32 bufferHash, u32 valueHash, const float* pFlt, size_t elemCount);
		void SetInteger(u32 bufferHash, u32 valueHash, const int* pInt, size_t elemCount);
		void SetUAV(u32 uavHash, class CUAVBuffer* pUAV);
		void SetTexture(u32 textureHash, class CTexture* pTexture);

		// Accessors.
		inline const CShader* GetShader() const { return m_pShader; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		Data m_data;
		CMaterialFile m_materialFile;

		u8* m_pRawBufferData;
		u8** m_pBufferDataList;
		class CConstantBuffer** m_pConstantBufferList;
		class CUAVBuffer** m_pUAVBufferList;
		class CTexture** m_pTextureList;
		class CShader* m_pShader;
	};
};

#endif
