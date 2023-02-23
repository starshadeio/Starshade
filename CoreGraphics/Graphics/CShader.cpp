//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CShader.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CShader.h"
#include "CShaderCompiler.h"
#include "CShaderFile.h"
#include "CRootSignature.h"
#include "../Factory/CFactory.h"

namespace Graphics
{
	CShader::CShader() :
		m_pMacros(nullptr),
		m_pRootSignature(nullptr) {
	}

	CShader::~CShader() { }

	void CShader::Initialize(CShaderFile* pShaderFile)
	{
		pShaderFile->ParseShader(this);

		{ // Construct the error log path from the shader file path.
			int len = (int)pShaderFile->GetData().filename.size() - 1;
			while(len >= 0 && pShaderFile->GetData().filename[len] != '.') { --len; }
			if(len > 0)
			{
				m_errorLog = pShaderFile->GetData().filename.substr(0, len) + L".log";
			}
			else
			{
				m_errorLog = L"./Shader.txt";
			}
		}

		CompileShaders();

		{
			CRootSignature::Data data { };
			data.bCompute = m_shaderInfo[SHADER_TYPE_CS].entryPoint.size() > 0;
			data.flags = ROOT_SIGNATURE_FLAG_NONE;

			if(m_inputData.size() > 0) { data.flags |= ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; }
			if(m_shaderInfo[SHADER_TYPE_VS].entryPoint.empty()) { data.flags |= ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS; }
			if(m_shaderInfo[SHADER_TYPE_HS].entryPoint.empty()) { data.flags |= ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS; }
			if(m_shaderInfo[SHADER_TYPE_DS].entryPoint.empty()) { data.flags |= ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS; }
			if(m_shaderInfo[SHADER_TYPE_GS].entryPoint.empty()) { data.flags |= ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS; }
			if(m_shaderInfo[SHADER_TYPE_PS].entryPoint.empty()) { data.flags |= ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS; }
			data.flags |= ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS;
			data.flags |= ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;

			{ // Process constant buffers.

				// Lambda for add parameters based on visiblity.
				auto AddParameter = [&](const CBVData& cbvData, SHADER_VISIBILITY visibility) {
					RootParameter param { };
					param.shaderVisibility = visibility;

					if(cbvData.type == CBT_ROOT_DESCRIPTOR)
					{
						param.parameterType = ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

						param.descriptorTable.numDescriptorRanges = 1;
						param.descriptorTable.rangeOffset = static_cast<u32>(data.rangeList.size() - 1);
					}
					else
					{
						param.parameterType = ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;

						param.constants.shaderRegister = cbvData.index;
						param.constants.registerSpace = cbvData.space;
						param.constants.num32BitValues = cbvData.size >> 2;
					}

					data.paramList.push_back(param);
				};

				// Loop through constant buffer data to process.
				for(size_t i = 0; i < m_cbvData.size(); ++i)
				{
					auto& cbvData = m_cbvData[i];

					m_cbvMap.insert({ Math::FNV1a_32(cbvData.name.c_str(), cbvData.name.size()), static_cast<u32>(i) });

					// Validate size.
					if(cbvData.type == CBT_ROOT_DESCRIPTOR)
					{
						cbvData.size = (((cbvData.size - 1) >> 8) + 1) << 8;
					}

					DescriptorRange range { };

					range.rangeType = DESCRIPTOR_RANGE_TYPE_CBV;
					range.numDescriptors = 1;
					range.baseShaderRegister = cbvData.index;
					range.registerSpace = cbvData.space;
					range.flags = DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
					range.offsetInDescriptorsFromTableStart = UINT_MAX;
					data.rangeList.push_back(range);

					// Add parameters based on shader visibility.
					if((cbvData.visibility & SHADER_VISIBILITY_ALL) == SHADER_VISIBILITY_ALL)
					{
						AddParameter(cbvData, SHADER_VISIBILITY_ALL);
					}
					else
					{
						if(cbvData.visibility & SHADER_VISIBILITY_VERTEX)
						{
							AddParameter(cbvData, SHADER_VISIBILITY_VERTEX);
						}
						if(cbvData.visibility & SHADER_VISIBILITY_HULL)
						{
							AddParameter(cbvData, SHADER_VISIBILITY_HULL);
						}
						if(cbvData.visibility & SHADER_VISIBILITY_DOMAIN)
						{
							AddParameter(cbvData, SHADER_VISIBILITY_DOMAIN);
						}
						if(cbvData.visibility & SHADER_VISIBILITY_GEOMETRY)
						{
							AddParameter(cbvData, SHADER_VISIBILITY_GEOMETRY);
						}
						if(cbvData.visibility & SHADER_VISIBILITY_PIXEL)
						{
							AddParameter(cbvData, SHADER_VISIBILITY_PIXEL);
						}
					}
				}
			}

			{ // Process UAVs.

				// Lambda for add parameters based on visiblity.
				auto AddParameter = [&](SHADER_VISIBILITY visibility) {
					RootParameter param { };
					param.shaderVisibility = visibility;
					param.parameterType = ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

					param.descriptorTable.numDescriptorRanges = 1;
					param.descriptorTable.rangeOffset = static_cast<u32>(data.rangeList.size() - 1);

					data.paramList.push_back(param);
				};

				// Loop through unordered access buffer data to process.
				for(size_t i = 0; i < m_uavData.size(); ++i)
				{
					auto& uavData = m_uavData[i];

					m_uavMap.insert({ Math::FNV1a_32(uavData.name.c_str(), uavData.name.size()), static_cast<u32>(i) });

					DescriptorRange range { };

					range.rangeType = DESCRIPTOR_RANGE_TYPE_UAV;
					range.numDescriptors = 1;
					range.baseShaderRegister = uavData.index;
					range.registerSpace = uavData.space;
					range.flags = DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;
					range.offsetInDescriptorsFromTableStart = UINT_MAX;
					data.rangeList.push_back(range);

					// Add parameters based on shader visibility.
					if((uavData.visibility & SHADER_VISIBILITY_ALL) == SHADER_VISIBILITY_ALL)
					{
						AddParameter(SHADER_VISIBILITY_ALL);
					}
					else
					{
						if(uavData.visibility & SHADER_VISIBILITY_VERTEX)
						{
							AddParameter(SHADER_VISIBILITY_VERTEX);
						}
						if(uavData.visibility & SHADER_VISIBILITY_HULL)
						{
							AddParameter(SHADER_VISIBILITY_HULL);
						}
						if(uavData.visibility & SHADER_VISIBILITY_DOMAIN)
						{
							AddParameter(SHADER_VISIBILITY_DOMAIN);
						}
						if(uavData.visibility & SHADER_VISIBILITY_GEOMETRY)
						{
							AddParameter(SHADER_VISIBILITY_GEOMETRY);
						}
						if(uavData.visibility & SHADER_VISIBILITY_PIXEL)
						{
							AddParameter(SHADER_VISIBILITY_PIXEL);
						}
					}
				}
			}

			{ // Process texture.

				// Lambda for add parameters based on visiblity.
				auto AddParameter = [&](SHADER_VISIBILITY visibility) {
					RootParameter param { };
					param.shaderVisibility = visibility;
					param.parameterType = ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;

					param.descriptorTable.numDescriptorRanges = 1;
					param.descriptorTable.rangeOffset = static_cast<u32>(data.rangeList.size() - 1);

					data.paramList.push_back(param);
				};

				// Loop through texture data to process.
				for(size_t i = 0; i < m_textureData.size(); ++i)
				{
					auto& textureData = m_textureData[i];

					m_textureMap.insert({ Math::FNV1a_32(textureData.name.c_str(), textureData.name.size()), static_cast<u32>(i) });

					DescriptorRange range { };

					range.rangeType = DESCRIPTOR_RANGE_TYPE_SRV;
					range.numDescriptors = 1;
					range.baseShaderRegister = textureData.index;
					range.registerSpace = textureData.space;
					range.flags = ConvertGFXTextureDataTypeToDescRangeFlags(textureData.type);
					range.offsetInDescriptorsFromTableStart = UINT_MAX;
					data.rangeList.push_back(range);

					// Add parameters based on shader visibility.
					if((textureData.visibility & SHADER_VISIBILITY_ALL) == SHADER_VISIBILITY_ALL)
					{
						AddParameter(SHADER_VISIBILITY_ALL);
					}
					else
					{
						if(textureData.visibility & SHADER_VISIBILITY_VERTEX)
						{
							AddParameter(SHADER_VISIBILITY_VERTEX);
						}
						if(textureData.visibility & SHADER_VISIBILITY_HULL)
						{
							AddParameter(SHADER_VISIBILITY_HULL);
						}
						if(textureData.visibility & SHADER_VISIBILITY_DOMAIN)
						{
							AddParameter(SHADER_VISIBILITY_DOMAIN);
						}
						if(textureData.visibility & SHADER_VISIBILITY_GEOMETRY)
						{
							AddParameter(SHADER_VISIBILITY_GEOMETRY);
						}
						if(textureData.visibility & SHADER_VISIBILITY_PIXEL)
						{
							AddParameter(SHADER_VISIBILITY_PIXEL);
						}
					}
				}
			}

			{ // Process static samplers.

				// Lambda for add samplers based on visiblity.
				auto AddSampler = [&](const SamplerData& samplerData, SHADER_VISIBILITY visibility) {
					StaticSamplerDesc samplerDesc = samplerData.sampler;
					samplerDesc.shaderVisibility = visibility;
					data.samplerList.push_back(samplerDesc);
				};

				// Loop through static samplers data to process.
				for(size_t i = 0; i < m_samplerData.size(); ++i)
				{
					auto& samplerData = m_samplerData[i];
					m_samplerMap.insert({ Math::FNV1a_32(samplerData.name.c_str(), samplerData.name.size()), static_cast<u32>(i) });

					// Add parameters based on shader visibility.
					if((samplerData.sampler.shaderVisibility & SHADER_VISIBILITY_ALL) == SHADER_VISIBILITY_ALL)
					{
						AddSampler(samplerData, SHADER_VISIBILITY_ALL);
					}
					else
					{
						if(samplerData.sampler.shaderVisibility & SHADER_VISIBILITY_VERTEX)
						{
							AddSampler(samplerData, SHADER_VISIBILITY_VERTEX);
						}
						if(samplerData.sampler.shaderVisibility & SHADER_VISIBILITY_HULL)
						{
							AddSampler(samplerData, SHADER_VISIBILITY_HULL);
						}
						if(samplerData.sampler.shaderVisibility & SHADER_VISIBILITY_DOMAIN)
						{
							AddSampler(samplerData, SHADER_VISIBILITY_DOMAIN);
						}
						if(samplerData.sampler.shaderVisibility & SHADER_VISIBILITY_GEOMETRY)
						{
							AddSampler(samplerData, SHADER_VISIBILITY_GEOMETRY);
						}
						if(samplerData.sampler.shaderVisibility & SHADER_VISIBILITY_PIXEL)
						{
							AddSampler(samplerData, SHADER_VISIBILITY_PIXEL);
						}
					}
				}
			}

			m_pRootSignature = CFactory::Instance().GetRootSignature(data);
		}
	}

	void CShader::CompileShaders()
	{
		auto CompileSingle = [this](SHADER_TYPE type) {
			if(m_shaderInfo[type].entryPoint.empty()) return;

			CFactory::Instance().GetShaderCompiler()->CompileFromData(m_hlsl.c_str(), m_hlsl.size(), m_shaderInfo[type].entryPoint.c_str(),
				m_pMacros, type, m_shaderInfo[type].featureLevel, m_byteCode[type], m_errorLog.c_str());
		};

		if(m_shaderInfo[SHADER_TYPE_CS].entryPoint.size() > 0)
		{
			CompileSingle(SHADER_TYPE_CS);
		}
		else
		{
			CompileSingle(SHADER_TYPE_VS);
			CompileSingle(SHADER_TYPE_HS);
			CompileSingle(SHADER_TYPE_DS);
			CompileSingle(SHADER_TYPE_GS);
			CompileSingle(SHADER_TYPE_PS);
		}
	}
};
