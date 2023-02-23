//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CShaderFileBlock.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CShaderFileBlock.h"
#include "CShader.h"
#include <Math/CMathFNV.h>
#include <unordered_map>
#include <cassert>

namespace Graphics
{
	static const std::unordered_map<std::string, CShaderFileBlock::BLOCK_TYPE> TYPE_MAP = {
		{ "cbuffer", CShaderFileBlock::BLOCK_TYPE_CBV},
		{ "RWBuffer", CShaderFileBlock::BLOCK_TYPE_UAV},
		{ "Texture1D", CShaderFileBlock::BLOCK_TYPE_TEXTURE },
		{ "Texture1DArray", CShaderFileBlock::BLOCK_TYPE_TEXTURE },
		{ "Texture2D", CShaderFileBlock::BLOCK_TYPE_TEXTURE },
		{ "Texture2DArray", CShaderFileBlock::BLOCK_TYPE_TEXTURE },
		{ "Texture3D", CShaderFileBlock::BLOCK_TYPE_TEXTURE },
		{ "TextureCube", CShaderFileBlock::BLOCK_TYPE_TEXTURE },
		{ "TextureCubeArray", CShaderFileBlock::BLOCK_TYPE_TEXTURE },
		{ "SamplerState", CShaderFileBlock::BLOCK_TYPE_SAMPLER},
	};

	static const std::unordered_map<std::string, u32> TYPE_SIZE_MAP = {
		{ "float", 4 },
		{ "float1", 4 },
		{ "float2", 8 },
		{ "float3", 12 },
		{ "float4", 16 },
		{ "float2x2", 16 },
		{ "float3x3", 36 },
		{ "float4x4", 64 },
		{ "float3x4", 48 },
		{ "float4x3", 48 },
		{ "int", 4 },
		{ "int1", 4 },
		{ "int2", 8 },
		{ "int3", 12 },
		{ "int4", 16 },
	};

	CShaderFileBlock::CShaderFileBlock()
	{
		Clear();
	}

	bool CShaderFileBlock::TryInitialize(const std::string& tp)
	{
		size_t offset = 0;
		for(; offset < tp.size(); ++offset)
		{
			if(tp[offset] == '<') { break; }
		}

		auto elem = TYPE_MAP.find(tp.substr(0, offset));
		if(elem == TYPE_MAP.end()) return false;

		m_blockType = elem->second;

		m_root.SetTarget(tp);
		m_root.Continue();

		return true;
	}

	bool CShaderFileBlock::ProcessCharacter(char ch)
	{
		if(ch > 0x20)
		{

			if(ch == ';' || ch == '{')
			{ // Move to next triple.
				if(!m_word.Empty()) { m_pTarget->SetTarget(m_word); }
				m_pTarget = nullptr;

				if(ch == ';' && m_variables.empty())
				{
					// Braceless block.
					return false;
				}
			}
			else if(ch == '}')
			{
				return false;
			}

			else if(m_pTarget == nullptr || m_pTarget->GetTargetIndex() < 2 || ch != ':')
			{
				m_word.AddChar(ch);
			}
		}
		else if(!m_word.Empty())
		{
			// Set current triple element's value.
			if(m_pTarget == nullptr)
			{
				m_variables.push_back(Util::CompilerTuple<3, std::string, char>());
				m_pTarget = &m_variables[m_variables.size() - 1];
			}

			m_pTarget->SetTarget(m_word);
			m_pTarget->Continue();
		}

		return true;
	}

	void CShaderFileBlock::Finalize(class CShader* pShader)
	{
		switch(m_blockType)
		{
			case BLOCK_TYPE_CBV:
				FinalizeCBV(pShader);
				break;
			case BLOCK_TYPE_UAV:
				FinalizeUAV(pShader);
				break;
			case BLOCK_TYPE_TEXTURE:
				FinalizeTexture(pShader);
				break;
			case BLOCK_TYPE_SAMPLER:
				FinalizeSampler(pShader);
				break;
			default:
				break;
		}
	}

	void CShaderFileBlock::Clear()
	{
		m_blockType = BLOCK_TYPE_UNKNOWN;

		m_variables.clear();
		m_root.Clear();
		m_pTarget = &m_root;
	}

	//-----------------------------------------------------------------------------------------------
	// Finalization methods.
	//-----------------------------------------------------------------------------------------------

	// Method for constructing a new cbuffer from processed block.
	void CShaderFileBlock::FinalizeCBV(class CShader* pShader)
	{
		int bufferIndex = 0;
		int bufferSpace = 0;

		GatherOptionalIndices(m_root.GetElement(2), [&](int index, int val) {
			switch(index)
			{
				case 0:
					bufferIndex = val;
					break;
				case 1:
					bufferSpace = val;
					break;
				default:
					break;
			}
		});

		// Set cbuffer data from parsed text..
		CShader::CBVData* pBuffer = pShader->GetCBVDataPtr(bufferIndex);
		pBuffer->name = m_root.GetElement(1);
		pBuffer->index = bufferIndex;
		pBuffer->space = bufferSpace;

		pBuffer->size = 0;
		for(const auto& var : m_variables)
		{
			CShader::CBVData::Var v { };
			v.size = ProcessVarTypeSize(var.GetElement(0)) * ProcessVarName(var.GetElement(1), v.name);
			pBuffer->offsetMap.insert({ Math::FNV1a_32(v.name.c_str(), v.name.size()), pBuffer->size });
			pBuffer->size += v.size;
			pBuffer->varList.push_back(std::move(v));
		}

		assert(pBuffer->size);
		pBuffer->size = ((((pBuffer->size - 1) >> 4) + 1) << 4);
	}

	void CShaderFileBlock::FinalizeUAV(class CShader* pShader)
	{
		int index = 0;
		int space = 0;

		GatherOptionalIndices(m_root.GetElement(2), [&](int i, int val) {
			switch(i)
			{
				case 0:
					index = val;
					break;
				case 1:
					space = val;
					break;
				default:
					break;
			}
		});

		CShader::UAVData* pUAVData = pShader->GetUAVDataPtr(index);
		pUAVData->name = m_root.GetElement(1);
		pUAVData->index = index;
		pUAVData->space = space;
	}

	// Method for constructing a new texture from processed block.
	void CShaderFileBlock::FinalizeTexture(class CShader* pShader)
	{
		int index = 0;
		int space = 0;

		GatherOptionalIndices(m_root.GetElement(2), [&](int i, int val) {
			switch(i)
			{
				case 0:
					index = val;
					break;
				case 1:
					space = val;
					break;
				default:
					break;
			}
		});

		CShader::TextureData* pTextureData = pShader->GetTextureDataPtr(index);
		pTextureData->name = m_root.GetElement(1);
		pTextureData->index = index;
		pTextureData->space = space;
	}

	void CShaderFileBlock::FinalizeSampler(class CShader* pShader)
	{
		int index = 0;
		int space = 0;

		GatherOptionalIndices(m_root.GetElement(2), [&](int i, int val) {
			switch(i)
			{
				case 0:
					index = val;
					break;
				case 1:
					space = val;
					break;
				default:
					break;
			}
		});

		CShader::SamplerData* pSamplerData = pShader->GetSamplerDataPtr(index);
		pSamplerData->name = m_root.GetElement(1);
		pSamplerData->sampler.shaderRegister = index;
		pSamplerData->sampler.registerSpace = space;
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------

	// Method for gathering cbuffer indices. Usually for the third, and sometimes optional variable parameter.
	void CShaderFileBlock::GatherOptionalIndices(const std::string& str, std::function<void(int, int)> func)
	{
		if(str.empty()) { return; }

		int offset = 0;
		for(; offset < (int)str.size(); ++offset)
		{
			if(str[offset] == '(')
			{
				++offset;
				break;
			}
		}

		int index = 0;
		std::string val = "";
		for(; offset < (int)str.size(); ++offset)
		{
			if(str[offset] != ',' && str[offset] != ')')
			{
				if(str[offset] <= 0x20 || !isdigit(str[offset])) { continue; }
				val += str[offset];
			}
			else
			{
				func(index++, atoi(val.c_str()));
				val.clear();
			}
		}
	}

	// Method for returning the size of the current variable's type.
	u32 CShaderFileBlock::ProcessVarTypeSize(const std::string& str)
	{
		return TYPE_SIZE_MAP.find(str)->second;
	}

	// Method for processing a variable name. Return the amount of elements it contains. Will be > 0 iff it's an array.
	u32 CShaderFileBlock::ProcessVarName(const std::string& str, std::string& res)
	{
		int offset = 0;
		for(; offset < (int)str.size(); ++offset)
		{
			if(str[offset] == '[') { break; }
		}

		res = str.substr(0, offset);
		if(offset++ < (int)str.size())
		{
			int end = offset;
			for(; end < str.size(); ++end)
			{
				if(str[end] == ']') { break; }
			}

			return u32(atoi(str.substr(offset, end - offset).c_str()));
		}
		else
		{
			return 1;
		}
	}
};
