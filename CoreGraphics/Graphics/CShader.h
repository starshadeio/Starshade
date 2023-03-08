//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CShader.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSHADER_H
#define CSHADER_H

#include "CGraphicsData.h"
#include "CShaderTrie.h"
#include <Utilities/CCompilerUtil.h>
#include <Math/CMathFNV.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace Graphics
{
	class CShader
	{
	protected:
		struct ShaderInfo
		{
			std::string entryPoint;
			SHADER_FEATURE_LEVEL featureLevel;
		};

		struct PipelineInfo
		{
			bool bAntialiasedLines = false;
			bool bDepthRead = true;
			bool bDepthWrite = true;
			COMPARISON_FUNC depthComparison = COMPARISON_FUNC_LESS;
			FILL_MODE fillMode = FILL_MODE_SOLID;
			CULL_MODE cullMode = CULL_MODE_BACK;
			PRIMITIVE_TOPOLOGY_TYPE topologyType;
			u32 renderTargetCount;
			GFX_FORMAT rtvFormats[8];
			GFX_FORMAT dsvFormat;
		};

	public:
		struct InputData
		{
			std::string semanticName;
			u32 semanticIndex;
			GFX_FORMAT format;
			u32 inputSlot;
			u32 alignedByteOffset;
			INPUT_CLASSIFICATION inputSlotClass;
			u32 instanceDataStepRate;
		};

		struct CBVData
		{
			struct Var
			{
				u32 size;
				std::string name;
			};

			CONSTANT_BUFFER_TYPE type;
			SHADER_VISIBILITY visibility;
			u32 visibilityCount;
			bool bFixed;
			u32 size;
			u32 index;
			u32 space;
			std::string name;
			std::vector<Var> varList;
			std::unordered_map<u32, u32> offsetMap;
		};

		struct UAVData
		{
			UNORDERED_ACCESS_TYPE type;
			SHADER_VISIBILITY visibility;
			u32 visibilityCount;
			u32 index;
			u32 space;
			std::string name;
		};

		struct TextureData
		{
			TEXTURE_DATA_TYPE type;
			SHADER_VISIBILITY visibility;
			u32 visibilityCount;
			u32 index;
			u32 space;
			std::string name;
		};

		struct SamplerData
		{
			StaticSamplerDesc sampler;
			std::string name;
		};

		struct BlendData
		{
			bool bBlendEnable;
			bool bLogicOpEnable;
			GFX_BLEND srcBlend;
			GFX_BLEND destBlend;
			GFX_BLEND_OP blendOp;
			GFX_BLEND srcBlendAlpha;
			GFX_BLEND destBlendAlpha;
			GFX_BLEND_OP blendOpAlpha;
			GFX_LOGIC_OP logicOp;
			u8 renderTargetWriteMask;
		};

	public:
		struct Data
		{
			CShaderTrie::Node* pNode;
		};

	protected:
		CShader();
		CShader(const CShader&) = delete;
		CShader(CShader&&) = delete;
		CShader& operator = (const CShader&) = delete;
		CShader& operator = (CShader&&) = delete;

	public:
		virtual ~CShader();

		virtual void Initialize(class CShaderFile* pShaderFile);
		virtual void Bind() = 0;
		virtual void Release() = 0;

		// Accessors.
		inline const std::vector<std::string>& GetMacroList() const { return m_data.pNode->macroList; }

		inline bool IsCompute() const { return m_shaderInfo[SHADER_TYPE_CS].entryPoint.size() > 0; }
		inline class CRootSignature* GetRootSignature() const { return m_pRootSignature; }

		inline size_t GetCBVDataCount() const { return m_cbvData.size(); }
		inline const CBVData& GetCBVData(u32 i) const { return m_cbvData[i]; }
		inline CBVData* GetCBVDataPtr(u32 i) { return &m_cbvData[i]; }
		inline u32 GetCBVIndex(u32 hash) const { return m_cbvMap.find(hash)->second; }

		inline size_t GetUAVDataCount() const { return m_uavData.size(); }
		inline const UAVData& GetUAVData(u32 i) const { return m_uavData[i]; }
		inline UAVData* GetUAVDataPtr(u32 i) { return &m_uavData[i]; }
		inline u32 GetUAVIndex(u32 hash) const { return m_uavMap.find(hash)->second; }

		inline size_t GetTextureDataCount() const { return m_textureData.size(); }
		inline const TextureData& GetTextureData(u32 i) const { return m_textureData[i]; }
		inline TextureData* GetTextureDataPtr(u32 i) { return &m_textureData[i]; }
		inline u32 GetTextureIndex(u32 hash) const { return m_textureMap.find(hash)->second; }

		inline size_t GetSamplerDataCount() const { return m_samplerData.size(); }
		inline const SamplerData& GetSamplerData(u32 i) const { return m_samplerData[i]; }
		inline SamplerData* GetSamplerDataPtr(u32 i) { return &m_samplerData[i]; }
		inline u32 GetSamplerIndex(u32 hash) const { return m_samplerMap.find(hash)->second; }

		inline GFX_FORMAT GetDepthFormat() const { return m_pipelineInfo.dsvFormat; }
		inline bool GetDepthWrite() const { return m_pipelineInfo.bDepthWrite; }
		inline bool GetDepthRead() const { return m_pipelineInfo.bDepthRead; }
		
		inline const std::string& GetProperies() const { return m_properties; }
		inline const std::string& GetHLSL() const { return m_hlsl; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

		inline void SetProperies(Util::CompilerWord<std::string, char>& word) { word.Move(m_properties); }
		inline void SetHLSL(Util::CompilerWord<std::string, char>& word) { word.Move(m_hlsl); }

		inline void AddInputData(InputData&& inputData) { m_inputData.push_back(std::move(inputData)); }
		inline void AddCBVData(CBVData&& cbvData) { m_cbvData.push_back(std::move(cbvData)); }
		inline void AddUAVData(UAVData&& uavData) { m_uavData.push_back(std::move(uavData)); }
		inline void AddTextureData(TextureData&& textureData) { m_textureData.push_back(std::move(textureData)); }
		inline void AddSamplerData(SamplerData&& samplerData) { m_samplerData.push_back(std::move(samplerData)); }
		inline void AddBlendData(BlendData&& blendData) { m_blendData.push_back(std::move(blendData)); }

		inline void SetEntryPoint(SHADER_TYPE type, const std::string& entry) { m_shaderInfo[type].entryPoint = entry; }
		inline void SetVersion(SHADER_TYPE type, SHADER_FEATURE_LEVEL featureLevel) { m_shaderInfo[type].featureLevel = featureLevel; }

		inline PipelineInfo& PipelineInfoRef() { return m_pipelineInfo; }

	private:
		void CompileShaders();

	protected:
		Data m_data;

		std::wstring m_errorLog;
		std::string m_properties;
		std::string m_hlsl;

		PipelineInfo m_pipelineInfo;
		ShaderInfo m_shaderInfo[SHADER_TYPE_COUNT];
		ByteCode m_byteCode[SHADER_TYPE_COUNT];

		std::vector<InputData> m_inputData;
		std::vector<CBVData> m_cbvData;
		std::vector<UAVData> m_uavData;
		std::vector<TextureData> m_textureData;
		std::vector<SamplerData> m_samplerData;
		std::vector<BlendData> m_blendData;
		std::unordered_map<u32, u32> m_cbvMap;
		std::unordered_map<u32, u32> m_uavMap;
		std::unordered_map<u32, u32> m_textureMap;
		std::unordered_map<u32, u32> m_samplerMap;

		ShaderMacro* m_pMacros;
		class CRootSignature* m_pRootSignature;
	};
};

#endif
