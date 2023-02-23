//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CShaderFileProp.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CShaderFileProp.h"
#include "CShader.h"
#include <Utilities/CDebugError.h>
#include <Utilities/CConvertUtil.h>

namespace Graphics
{
	//-----------------------------------------------------------------------------------------------
	// Processors mappings.
	//-----------------------------------------------------------------------------------------------

	// Formats.
	static const std::unordered_map<std::string, GFX_FORMAT> FORMAT_MAP {
		{"UNKNOWN", GFX_FORMAT_UNKNOWN },
		{"R32G32B32A32_TYPELESS", GFX_FORMAT_R32G32B32A32_TYPELESS },
		{"R32G32B32A32_FLOAT", GFX_FORMAT_R32G32B32A32_FLOAT },
		{"R32G32B32A32_UINT", GFX_FORMAT_R32G32B32A32_UINT },
		{"R32G32B32A32_SINT", GFX_FORMAT_R32G32B32A32_SINT },
		{"R32G32B32_TYPELESS", GFX_FORMAT_R32G32B32_TYPELESS },
		{"R32G32B32_FLOAT", GFX_FORMAT_R32G32B32_FLOAT },
		{"R32G32B32_UINT", GFX_FORMAT_R32G32B32_UINT },
		{"R32G32B32_SINT", GFX_FORMAT_R32G32B32_SINT },
		{"R16G16B16A16_TYPELESS", GFX_FORMAT_R16G16B16A16_TYPELESS },
		{"R16G16B16A16_FLOAT", GFX_FORMAT_R16G16B16A16_FLOAT },
		{"R16G16B16A16_UNORM", GFX_FORMAT_R16G16B16A16_UNORM },
		{"R16G16B16A16_UINT", GFX_FORMAT_R16G16B16A16_UINT },
		{"R16G16B16A16_SNORM", GFX_FORMAT_R16G16B16A16_SNORM },
		{"R16G16B16A16_SINT", GFX_FORMAT_R16G16B16A16_SINT },
		{"R32G32_TYPELESS", GFX_FORMAT_R32G32_TYPELESS },
		{"R32G32_FLOAT", GFX_FORMAT_R32G32_FLOAT },
		{"R32G32_UINT", GFX_FORMAT_R32G32_UINT },
		{"R32G32_SINT", GFX_FORMAT_R32G32_SINT },
		{"R32G8X24_TYPELESS", GFX_FORMAT_R32G8X24_TYPELESS },
		{"D32_FLOAT_S8X24_UINT", GFX_FORMAT_D32_FLOAT_S8X24_UINT },
		{"R32_FLOAT_X8X24_TYPELESS", GFX_FORMAT_R32_FLOAT_X8X24_TYPELESS },
		{"X32_TYPELESS_G8X24_UINT", GFX_FORMAT_X32_TYPELESS_G8X24_UINT },
		{"R10G10B10A2_TYPELESS", GFX_FORMAT_R10G10B10A2_TYPELESS },
		{"R10G10B10A2_UNORM", GFX_FORMAT_R10G10B10A2_UNORM },
		{"R10G10B10A2_UINT", GFX_FORMAT_R10G10B10A2_UINT },
		{"R11G11B10_FLOAT", GFX_FORMAT_R11G11B10_FLOAT },
		{"R8G8B8A8_TYPELESS", GFX_FORMAT_R8G8B8A8_TYPELESS },
		{"R8G8B8A8_UNORM", GFX_FORMAT_R8G8B8A8_UNORM },
		{"R8G8B8A8_UNORM_SRGB", GFX_FORMAT_R8G8B8A8_UNORM_SRGB },
		{"R8G8B8A8_UINT", GFX_FORMAT_R8G8B8A8_UINT },
		{"R8G8B8A8_SNORM", GFX_FORMAT_R8G8B8A8_SNORM },
		{"R8G8B8A8_SINT", GFX_FORMAT_R8G8B8A8_SINT },
		{"R16G16_TYPELESS", GFX_FORMAT_R16G16_TYPELESS },
		{"R16G16_FLOAT", GFX_FORMAT_R16G16_FLOAT },
		{"R16G16_UNORM", GFX_FORMAT_R16G16_UNORM },
		{"R16G16_UINT", GFX_FORMAT_R16G16_UINT },
		{"R16G16_SNORM", GFX_FORMAT_R16G16_SNORM },
		{"R16G16_SINT", GFX_FORMAT_R16G16_SINT },
		{"R32_TYPELESS", GFX_FORMAT_R32_TYPELESS },
		{"D32_FLOAT", GFX_FORMAT_D32_FLOAT },
		{"R32_FLOAT", GFX_FORMAT_R32_FLOAT },
		{"R32_UINT", GFX_FORMAT_R32_UINT },
		{"R32_SINT", GFX_FORMAT_R32_SINT },
		{"R24G8_TYPELESS", GFX_FORMAT_R24G8_TYPELESS },
		{"D24_UNORM_S8_UINT", GFX_FORMAT_D24_UNORM_S8_UINT },
		{"R24_UNORM_X8_TYPELESS", GFX_FORMAT_R24_UNORM_X8_TYPELESS },
		{"X24_TYPELESS_G8_UINT", GFX_FORMAT_X24_TYPELESS_G8_UINT },
		{"R8G8_TYPELESS", GFX_FORMAT_R8G8_TYPELESS },
		{"R8G8_UNORM", GFX_FORMAT_R8G8_UNORM },
		{"R8G8_UINT", GFX_FORMAT_R8G8_UINT },
		{"R8G8_SNORM", GFX_FORMAT_R8G8_SNORM },
		{"R8G8_SINT", GFX_FORMAT_R8G8_SINT },
		{"R16_TYPELESS", GFX_FORMAT_R16_TYPELESS },
		{"R16_FLOAT", GFX_FORMAT_R16_FLOAT },
		{"D16_UNORM", GFX_FORMAT_D16_UNORM },
		{"R16_UNORM", GFX_FORMAT_R16_UNORM },
		{"R16_UINT", GFX_FORMAT_R16_UINT },
		{"R16_SNORM", GFX_FORMAT_R16_SNORM },
		{"R16_SINT", GFX_FORMAT_R16_SINT },
		{"R8_TYPELESS", GFX_FORMAT_R8_TYPELESS },
		{"R8_UNORM", GFX_FORMAT_R8_UNORM },
		{"R8_UINT", GFX_FORMAT_R8_UINT },
		{"R8_SNORM", GFX_FORMAT_R8_SNORM },
		{"R8_SINT", GFX_FORMAT_R8_SINT },
		{"A8_UNORM", GFX_FORMAT_A8_UNORM },
		{"R1_UNORM", GFX_FORMAT_R1_UNORM },
		{"R9G9B9E5_SHAREDEXP", GFX_FORMAT_R9G9B9E5_SHAREDEXP },
		{"R8G8_B8G8_UNORM", GFX_FORMAT_R8G8_B8G8_UNORM },
		{"G8R8_G8B8_UNORM", GFX_FORMAT_G8R8_G8B8_UNORM },
		{"BC1_TYPELESS", GFX_FORMAT_BC1_TYPELESS },
		{"BC1_UNORM", GFX_FORMAT_BC1_UNORM },
		{"BC1_UNORM_SRGB", GFX_FORMAT_BC1_UNORM_SRGB },
		{"BC2_TYPELESS", GFX_FORMAT_BC2_TYPELESS },
		{"BC2_UNORM", GFX_FORMAT_BC2_UNORM },
		{"BC2_UNORM_SRGB", GFX_FORMAT_BC2_UNORM_SRGB },
		{"BC3_TYPELESS", GFX_FORMAT_BC3_TYPELESS },
		{"BC3_UNORM", GFX_FORMAT_BC3_UNORM },
		{"BC3_UNORM_SRGB", GFX_FORMAT_BC3_UNORM_SRGB },
		{"BC4_TYPELESS", GFX_FORMAT_BC4_TYPELESS },
		{"BC4_UNORM", GFX_FORMAT_BC4_UNORM },
		{"BC4_SNORM", GFX_FORMAT_BC4_SNORM },
		{"BC5_TYPELESS", GFX_FORMAT_BC5_TYPELESS },
		{"BC5_UNORM", GFX_FORMAT_BC5_UNORM },
		{"BC5_SNORM", GFX_FORMAT_BC5_SNORM },
		{"B5G6R5_UNORM", GFX_FORMAT_B5G6R5_UNORM },
		{"B5G5R5A1_UNORM", GFX_FORMAT_B5G5R5A1_UNORM },
		{"B8G8R8A8_UNORM", GFX_FORMAT_B8G8R8A8_UNORM },
		{"B8G8R8X8_UNORM", GFX_FORMAT_B8G8R8X8_UNORM },
		{"R10G10B10_XR_BIAS_A2_UNORM", GFX_FORMAT_R10G10B10_XR_BIAS_A2_UNORM },
		{"B8G8R8A8_TYPELESS", GFX_FORMAT_B8G8R8A8_TYPELESS },
		{"B8G8R8A8_UNORM_SRGB", GFX_FORMAT_B8G8R8A8_UNORM_SRGB },
		{"B8G8R8X8_TYPELESS", GFX_FORMAT_B8G8R8X8_TYPELESS },
		{"B8G8R8X8_UNORM_SRGB", GFX_FORMAT_B8G8R8X8_UNORM_SRGB },
		{"BC6H_TYPELESS", GFX_FORMAT_BC6H_TYPELESS },
		{"BC6H_UF16", GFX_FORMAT_BC6H_UF16 },
		{"BC6H_SF16", GFX_FORMAT_BC6H_SF16 },
		{"BC7_TYPELESS", GFX_FORMAT_BC7_TYPELESS },
		{"BC7_UNORM", GFX_FORMAT_BC7_UNORM },
		{"BC7_UNORM_SRGB", GFX_FORMAT_BC7_UNORM_SRGB },
		{"AYUV", GFX_FORMAT_AYUV },
		{"Y410", GFX_FORMAT_Y410 },
		{"Y416", GFX_FORMAT_Y416 },
		{"NV12", GFX_FORMAT_NV12 },
		{"P010", GFX_FORMAT_P010 },
		{"P016", GFX_FORMAT_P016 },
		{"420_OPAQUE", GFX_FORMAT_420_OPAQUE },
		{"YUY2", GFX_FORMAT_YUY2 },
		{"Y210", GFX_FORMAT_Y210 },
		{"Y216", GFX_FORMAT_Y216 },
		{"NV11", GFX_FORMAT_NV11 },
		{"AI44", GFX_FORMAT_AI44 },
		{"IA44", GFX_FORMAT_IA44 },
		{"P8", GFX_FORMAT_P8 },
		{"A8P8", GFX_FORMAT_A8P8 },
		{"B4G4R4A4_UNORM", GFX_FORMAT_B4G4R4A4_UNORM },
		{"P208", GFX_FORMAT_P208 },
		{"V208", GFX_FORMAT_V208 },
		{"V408", GFX_FORMAT_V408 },
		{"FORCE_UINT", GFX_FORMAT_FORCE_UINT },
	};

	// Textures.
	static const std::unordered_map<std::string, GFX_FILTER> FILTER_MAP {
		{ "MIN_MAG_MIP_POINT", GFX_FILTER_MIN_MAG_MIP_POINT },
		{ "MIN_MAG_POINT_MIP_LINEAR", GFX_FILTER_MIN_MAG_POINT_MIP_LINEAR },
		{ "MIN_POINT_MAG_LINEAR_MIP_POINT", GFX_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT },
		{ "MIN_POINT_MAG_MIP_LINEAR", GFX_FILTER_MIN_POINT_MAG_MIP_LINEAR },
		{ "MIN_LINEAR_MAG_MIP_POINT ", GFX_FILTER_MIN_LINEAR_MAG_MIP_POINT },
		{ "MIN_LINEAR_MAG_POINT_MIP_LINEAR", GFX_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR },
		{ "MIN_MAG_LINEAR_MIP_POINT", GFX_FILTER_MIN_MAG_LINEAR_MIP_POINT },
		{ "MIN_MAG_MIP_LINEAR", GFX_FILTER_MIN_MAG_MIP_LINEAR },
		{ "ANISOTROPIC", GFX_FILTER_ANISOTROPIC },
		{ "COMPARISON_MIN_MAG_MIP_POINT", GFX_FILTER_COMPARISON_MIN_MAG_MIP_POINT },
		{ "COMPARISON_MIN_MAG_POINT_MIP_LINEAR", GFX_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR },
		{ "COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT", GFX_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT },
		{ "COMPARISON_MIN_POINT_MAG_MIP_LINEAR", GFX_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR },
		{ "COMPARISON_MIN_LINEAR_MAG_MIP_POINT", GFX_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT },
		{ "COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR", GFX_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR },
		{ "COMPARISON_MIN_MAG_LINEAR_MIP_POINT", GFX_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT },
		{ "COMPARISON_MIN_MAG_MIP_LINEAR", GFX_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR },
		{ "COMPARISON_ANISOTROPIC", GFX_FILTER_COMPARISON_ANISOTROPIC },
		{ "MINIMUM_MIN_MAG_MIP_POINT", GFX_FILTER_MINIMUM_MIN_MAG_MIP_POINT },
		{ "MINIMUM_MIN_MAG_POINT_MIP_LINEAR", GFX_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR },
		{ "MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT", GFX_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT },
		{ "MINIMUM_MIN_POINT_MAG_MIP_LINEAR", GFX_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR },
		{ "MINIMUM_MIN_LINEAR_MAG_MIP_POINT", GFX_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT },
		{ "MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR", GFX_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR },
		{ "MINIMUM_MIN_MAG_LINEAR_MIP_POINT", GFX_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT },
		{ "MINIMUM_MIN_MAG_MIP_LINEAR", GFX_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR },
		{ "MINIMUM_ANISOTROPIC", GFX_FILTER_MINIMUM_ANISOTROPIC },
		{ "MAXIMUM_MIN_MAG_MIP_POINT", GFX_FILTER_MAXIMUM_MIN_MAG_MIP_POINT },
		{ "MAXIMUM_MIN_MAG_POINT_MIP_LINEAR", GFX_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR },
		{ "MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT", GFX_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT },
		{ "MAXIMUM_MIN_POINT_MAG_MIP_LINEAR", GFX_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR },
		{ "MAXIMUM_MIN_LINEAR_MAG_MIP_POINT", GFX_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT },
		{ "MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR", GFX_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR },
		{ "MAXIMUM_MIN_MAG_LINEAR_MIP_POINT", GFX_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT },
		{ "MAXIMUM_MIN_MAG_MIP_LINEAR", GFX_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR },
		{ "MAXIMUM_ANISOTROPIC", GFX_FILTER_MAXIMUM_ANISOTROPIC },
	};

	static const std::unordered_map<std::string, TEXTURE_ADDRESS_MODE> TEXTURE_ADDRESS_MAP = {
		{ "WRAP", TEXTURE_ADDRESS_MODE_WRAP },
		{ "MIRROR", TEXTURE_ADDRESS_MODE_MIRROR },
		{ "CLAMP", TEXTURE_ADDRESS_MODE_CLAMP },
		{ "BORDER", TEXTURE_ADDRESS_MODE_BORDER },
		{ "MIRROR_ONCE", TEXTURE_ADDRESS_MODE_MIRROR_ONCE },
	};

	// Blending.
	static const std::unordered_map<std::string, GFX_BLEND> BLEND_MAP = {
		{ "ZERO", GFX_BLEND_ZERO },
		{ "ONE", GFX_BLEND_ONE },
		{ "SRC_COLOR", GFX_BLEND_SRC_COLOR },
		{ "INV_SRC_COLOR", GFX_BLEND_INV_SRC_COLOR },
		{ "SRC_ALPHA", GFX_BLEND_SRC_ALPHA },
		{ "INV_SRC_ALPHA", GFX_BLEND_INV_SRC_ALPHA },
		{ "DEST_ALPHA", GFX_BLEND_DEST_ALPHA },
		{ "INV_DEST_ALPHA", GFX_BLEND_INV_DEST_ALPHA },
		{ "DEST_COLOR", GFX_BLEND_DEST_COLOR },
		{ "INV_DEST_COLOR", GFX_BLEND_INV_DEST_COLOR },
		{ "SRC_ALPHA_SAT", GFX_BLEND_SRC_ALPHA_SAT },
		{ "BLEND_FACTOR", GFX_BLEND_BLEND_FACTOR },
		{ "INV_BLEND_FACTOR", GFX_BLEND_INV_BLEND_FACTOR },
		{ "SRC1_COLOR", GFX_BLEND_SRC1_COLOR },
		{ "INV_SRC1_COLOR", GFX_BLEND_INV_SRC1_COLOR },
		{ "SRC1_ALPHA", GFX_BLEND_SRC1_ALPHA },
		{ "INV_SRC1_ALPHA", GFX_BLEND_INV_SRC1_ALPHA },
		{ "ALPHA_FACTOR", GFX_BLEND_ALPHA_FACTOR },
		{ "INV_ALPHA_FACTOR", GFX_BLEND_INV_ALPHA_FACTOR },
	};

	static const std::unordered_map<std::string, GFX_BLEND_OP> BLEND_OP_MAP = {
		{ "ADD", GFX_BLEND_OP_ADD },
		{ "SUBTRACT", GFX_BLEND_OP_SUBTRACT },
		{ "REV_SUBTRACT", GFX_BLEND_OP_REV_SUBTRACT },
		{ "MIN", GFX_BLEND_OP_MIN },
		{ "MAX", GFX_BLEND_OP_MAX },
	};

	static const std::unordered_map<std::string, GFX_LOGIC_OP> LOGIC_OP_MAP = {
		{ "CLEAR", GFX_LOGIC_OP_CLEAR },
		{ "SET", GFX_LOGIC_OP_SET },
		{ "COPY", GFX_LOGIC_OP_COPY },
		{ "COPY_INVERTED", GFX_LOGIC_OP_COPY_INVERTED },
		{ "NOOP", GFX_LOGIC_OP_NOOP },
		{ "INVERT", GFX_LOGIC_OP_INVERT },
		{ "AND", GFX_LOGIC_OP_AND },
		{ "NAND", GFX_LOGIC_OP_NAND },
		{ "OR", GFX_LOGIC_OP_OR },
		{ "XOR", GFX_LOGIC_OP_XOR },
		{ "EQUIV", GFX_LOGIC_OP_EQUIV },
		{ "AND_REVERSE", GFX_LOGIC_OP_AND_REVERSE },
		{ "AND_INVERTED", GFX_LOGIC_OP_AND_INVERTED },
		{ "OR_REVERSE", GFX_LOGIC_OP_OR_REVERSE },
		{ "OR_INVERTED", GFX_LOGIC_OP_OR_INVERTED },
	};

	static const std::unordered_map<std::string, COLOR_WRITE_ENABLE_FLAGS> COLOR_WRITE_MAP = {
		{ "R", COLOR_WRITE_ENABLE_RED },
		{ "G", COLOR_WRITE_ENABLE_GREEN },
		{ "B", COLOR_WRITE_ENABLE_BLUE },
		{ "A", COLOR_WRITE_ENABLE_ALPHA },
		{ "RGBA", COLOR_WRITE_ENABLE_ALL },
	};

	// Geometry.
	static const std::unordered_map<std::string, PRIMITIVE_TOPOLOGY_TYPE> TOPOLOGY_MAP = {
		{ "POINTLIST", PRIMITIVE_TOPOLOGY_TYPE_POINTLIST },
		{ "LINELIST", PRIMITIVE_TOPOLOGY_TYPE_LINELIST },
		{ "TRIANGLELIST", PRIMITIVE_TOPOLOGY_TYPE_TRIANGLELIST },
		{ "TRIANGLESTRIP", PRIMITIVE_TOPOLOGY_TYPE_TRIANGLESTRIP },
		{ "PATCH", PRIMITIVE_TOPOLOGY_TYPE_PATCH },
	};
	
	static const std::unordered_map<std::string, FILL_MODE> FILL_MODE_MAP = {
		{ "SOLID", FILL_MODE_SOLID },
		{ "WIREFRAME", FILL_MODE_WIREFRAME },
	};
	
	static const std::unordered_map<std::string, CULL_MODE> CULL_MODE_MAP = {
		{ "NONE", CULL_MODE_NONE },
		{ "FRONT", CULL_MODE_FRONT },
		{ "BACK", CULL_MODE_BACK },
	};

	static const std::unordered_map<std::string, INPUT_CLASSIFICATION> INPUT_CLASSIFICATION_MAP {
		{ "VERTEX", INPUT_PER_VERTEX_DATA },
		{ "INSTANCE", INPUT_PER_INSTANCE_DATA },
	};

	// Shader.
	const std::unordered_map<std::string, CONSTANT_BUFFER_TYPE> CONSTANT_BUFFER_TYPE_MAP {
		{ "DESCRIPTOR", CBT_ROOT_DESCRIPTOR },
		{ "CONSTANTS", CBT_ROOT_CONSTANTS },
	};

	const std::unordered_map<std::string, TEXTURE_DATA_TYPE> TEXTURE_DATA_TYPE_MAP {
		{ "STATIC", TDT_STATIC },
		{ "STATIC_EXECUTE", TDT_STATIC_WHILE_SET_AT_EXECUTE },
		{ "VOLATILE", TDT_VOLATILE },
	};

	static const std::unordered_map<std::string, SHADER_VISIBILITY> VISIBILITY_MAP {
		{ "VERTEX", SHADER_VISIBILITY_VERTEX },
		{ "HULL", SHADER_VISIBILITY_HULL },
		{ "DOMAIN", SHADER_VISIBILITY_DOMAIN },
		{ "GEOMETRY", SHADER_VISIBILITY_GEOMETRY },
		{ "PIXEL", SHADER_VISIBILITY_PIXEL },
		{ "ALL", SHADER_VISIBILITY_ALL },
	};

	static const std::unordered_map<std::string, bool> FIXED_MAP {
		{ "FIXED", true },
		{ "DYNAMIC", false },
	};

	static const std::unordered_map<std::string, SHADER_FEATURE_LEVEL> SHADER_FEATURE_LEVEL_MAP = {
		{ "1_1", SHADER_FEATURE_LEVEL_1_1 },
		{ "2_0", SHADER_FEATURE_LEVEL_2_0 },
		{ "3_0", SHADER_FEATURE_LEVEL_3_0 },
		{ "4_0", SHADER_FEATURE_LEVEL_4_0 },
		{ "4_1", SHADER_FEATURE_LEVEL_4_1 },
		{ "5_0", SHADER_FEATURE_LEVEL_5_0 },
		{ "5_1", SHADER_FEATURE_LEVEL_5_1 },
		{ "6_0", SHADER_FEATURE_LEVEL_6_0 },
	};

	// Utilities.
	static const std::unordered_map<std::string, float> FLOAT_BUILTIN_MAP = {
		{ "FLT_MIN", FLT_MIN },
		{ "FLT_MAX", FLT_MAX },
	};

	static float ProcessFloat(const std::string& str)
	{
		auto elem = FLOAT_BUILTIN_MAP.find(str);
		if(elem != FLOAT_BUILTIN_MAP.end())
		{
			return elem->second;
		}
		else
		{
			return static_cast<float>(atof(str.c_str()));
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Processors.
	//-----------------------------------------------------------------------------------------------

	static void ShaderInput(CShader* pShader, const std::string& input)
	{
		CShader::InputData inputData { };

		Util::SplitString(input, ':', [&](u32 index, const std::string& str) {
			switch(index)
			{
				case 0:
					inputData.semanticName = str;
					break;
				case 1:
					inputData.semanticIndex = atoi(str.c_str());
					break;
				case 2:
					inputData.format = FORMAT_MAP.find(str)->second;
					break;
				case 3:
					inputData.inputSlot = atoi(str.c_str());
					break;
				case 4:
					inputData.alignedByteOffset = atoi(str.c_str());
					break;
				case 5:
					inputData.inputSlotClass = INPUT_CLASSIFICATION_MAP.find(str)->second;
					break;
				case 6:
					inputData.instanceDataStepRate = atoi(str.c_str());
					break;
				default:
					break;
			}
			});

		pShader->AddInputData(std::move(inputData));
	}

	static void ShaderCBV(CShader* pShader, const std::string& data)
	{
		CShader::CBVData cbvData { };

		Util::SplitString(data, ':', [&](u32 index, const std::string& str) {
			switch(index)
			{
				case 0:
					cbvData.type = CONSTANT_BUFFER_TYPE_MAP.find(str)->second;
					break;
				case 1:
				{
					cbvData.visibilityCount = Util::SplitString(str, '|', [&](u32 index, const std::string& vis) {
						cbvData.visibility |= VISIBILITY_MAP.find(vis)->second;
					});
				} break;
				case 2:
					cbvData.bFixed = FIXED_MAP.find(str)->second;
					break;
				default:
					break;
			}
			});

		pShader->AddCBVData(std::move(cbvData));
	}

	static void ShaderUAV(CShader* pShader, const std::string& data)
	{
		CShader::UAVData uavData { };

		Util::SplitString(data, ':', [&](u32 index, const std::string& str) {
			switch(index)
			{
				case 0:
				{
					uavData.visibilityCount = Util::SplitString(str, '|', [&](u32 index, const std::string& vis) {
						uavData.visibility |= VISIBILITY_MAP.find(vis)->second;
					});
				} break;
				default:
					break;
			}
			});

		pShader->AddUAVData(std::move(uavData));
	}

	static void ShaderTexture(CShader* pShader, const std::string& data)
	{
		CShader::TextureData textureData { };

		Util::SplitString(data, ':', [&](u32 index, const std::string& str) {
			switch(index)
			{
				case 0:
					textureData.type = TEXTURE_DATA_TYPE_MAP.find(str)->second;
					break;
				case 1:
				{
					textureData.visibilityCount = Util::SplitString(str, '|', [&](u32 index, const std::string& vis) {
						textureData.visibility |= VISIBILITY_MAP.find(vis)->second;
					});
				} break;
				default:
					break;
			}
			});

		pShader->AddTextureData(std::move(textureData));
	}

	static void ShaderSampler(CShader* pShader, const std::string& data)
	{
		CShader::SamplerData samplerData { };

		Util::SplitString(data, ':', [&](u32 index, const std::string& str) {
			switch(index)
			{
				case 0:
					samplerData.sampler.filter = FILTER_MAP.find(str)->second;
					break;
				case 1:
					samplerData.sampler.addressU = TEXTURE_ADDRESS_MAP.find(str)->second;
					break;
				case 2:
					samplerData.sampler.addressV = TEXTURE_ADDRESS_MAP.find(str)->second;
					break;
				case 3:
					samplerData.sampler.addressW = TEXTURE_ADDRESS_MAP.find(str)->second;
					break;
				case 4:
					samplerData.sampler.mipLODBias = ProcessFloat(str);
					break;
				case 5:
					samplerData.sampler.minLOD = ProcessFloat(str);
					break;
				case 6:
					samplerData.sampler.maxLOD = ProcessFloat(str);
					break;
				case 7:
					samplerData.sampler.maxAnisotropy = atoi(str.c_str());
					break;
				case 8:
				{
					Util::SplitString(str, '|', [&](u32 index, const std::string& vis) {
						samplerData.sampler.shaderVisibility |= VISIBILITY_MAP.find(vis)->second;
						});
				} break;
				default:
					break;
			}
			});

		pShader->AddSamplerData(std::move(samplerData));
	}

	static void ShaderBlend(CShader* pShader, const std::string& data)
	{
		CShader::BlendData blendData { };

		Util::SplitString(data, ':', [&](u32 index, const std::string& str) {
			switch(index)
			{
				case 0:
					blendData.bBlendEnable = Util::StringToBool(str);
					break;
				case 1:
					blendData.bLogicOpEnable = Util::StringToBool(str);
					break;
				case 2:
					blendData.srcBlend = BLEND_MAP.find(str)->second;
					break;
				case 3:
					blendData.destBlend = BLEND_MAP.find(str)->second;
					break;
				case 4:
					blendData.blendOp = BLEND_OP_MAP.find(str)->second;
					break;
				case 5:
					blendData.srcBlendAlpha = BLEND_MAP.find(str)->second;
					break;
				case 6:
					blendData.destBlendAlpha = BLEND_MAP.find(str)->second;
					break;
				case 7:
					blendData.blendOpAlpha = BLEND_OP_MAP.find(str)->second;
					break;
				case 8:
					blendData.logicOp = LOGIC_OP_MAP.find(str)->second;
					break;
				case 9:
				{
					Util::SplitString(str, '|', [&](u32 index, const std::string& channel) {
						blendData.renderTargetWriteMask |= COLOR_WRITE_MAP.find(channel)->second;
						});
				} break;
				default:
					break;
			}
			});

		pShader->AddBlendData(std::move(blendData));
	}

	static void ShaderEntry(CShader* pShader, SHADER_TYPE shaderType, const std::string& entry)
	{
		pShader->SetEntryPoint(shaderType, entry);
	}

	static void ShaderVersion(CShader* pShader, SHADER_TYPE shaderType, const std::string& shaderVersion)
	{
		pShader->SetVersion(shaderType, SHADER_FEATURE_LEVEL_MAP.find(shaderVersion.substr(3))->second);
	}

	// Pipeline info functions.
	static void ShaderTopology(CShader* pShader, const std::string& topology)
	{
		pShader->PipelineInfoRef().topologyType = TOPOLOGY_MAP.find(topology)->second;
	}
	
	static void ShaderFillMode(CShader* pShader, const std::string& fillMode)
	{
		pShader->PipelineInfoRef().fillMode = FILL_MODE_MAP.find(fillMode)->second;
	}
	
	static void ShaderCullMode(CShader* pShader, const std::string& cullMode)
	{
		pShader->PipelineInfoRef().cullMode = CULL_MODE_MAP.find(cullMode)->second;
	}

	static void ShaderAntialiasedLines(CShader* pShader, const std::string& boolean)
	{
		pShader->PipelineInfoRef().bAntialiasedLines = Util::StringToBool(boolean);
	}

	static void ShaderColor(CShader* pShader, const std::string& color)
	{
		pShader->PipelineInfoRef().renderTargetCount = (Util::SplitString(color, '|', [&](u32 index, const std::string& format) {
			pShader->PipelineInfoRef().rtvFormats[index] = FORMAT_MAP.find(format)->second;
			}));
	}

	static void ShaderDepth(CShader* pShader, const std::string& depth)
	{
		pShader->PipelineInfoRef().dsvFormat = FORMAT_MAP.find(depth)->second;
	}

	static void ShaderDepthRead(CShader* pShader, const std::string& boolean)
	{
		pShader->PipelineInfoRef().bDepthRead = Util::StringToBool(boolean);
	}

	static void ShaderDepthWrite(CShader* pShader, const std::string& boolean)
	{
		pShader->PipelineInfoRef().bDepthWrite = Util::StringToBool(boolean);
	}

	// Funcitonal mappings.
	const std::unordered_map<std::string, std::function<void(CShader*, const std::string&)>> PROP_MAP = {
		{ "input", [](CShader* pShader, const std::string& val) { ShaderInput(pShader, val); } },
		{ "cbv", [](CShader* pShader, const std::string& val) { ShaderCBV(pShader, val); } },
		{ "uav", [](CShader* pShader, const std::string& val) { ShaderUAV(pShader, val); } },
		{ "texture", [](CShader* pShader, const std::string& val) { ShaderTexture(pShader, val); } },
		{ "sampler", [](CShader* pShader, const std::string& val) { ShaderSampler(pShader, val); } },

		{ "vertex_entry", [](CShader* pShader, const std::string& val) { ShaderEntry(pShader, SHADER_TYPE_VS, val); } },
		{ "hull_entry", [](CShader* pShader, const std::string& val) { ShaderEntry(pShader, SHADER_TYPE_HS, val); } },
		{ "domain_entry", [](CShader* pShader, const std::string& val) { ShaderEntry(pShader, SHADER_TYPE_DS, val); } },
		{ "geometry_entry", [](CShader* pShader, const std::string& val) { ShaderEntry(pShader, SHADER_TYPE_GS, val); } },
		{ "pixel_entry", [](CShader* pShader, const std::string& val) { ShaderEntry(pShader, SHADER_TYPE_PS, val); } },
		{ "compute_entry", [](CShader* pShader, const std::string& val) { ShaderEntry(pShader, SHADER_TYPE_CS, val); } },

		{ "vertex_version", [](CShader* pShader, const std::string& val) { ShaderVersion(pShader, SHADER_TYPE_VS, val); } },
		{ "hull_version", [](CShader* pShader, const std::string& val) { ShaderVersion(pShader, SHADER_TYPE_HS, val); } },
		{ "domain_version", [](CShader* pShader, const std::string& val) { ShaderVersion(pShader, SHADER_TYPE_DS, val); } },
		{ "geometry_version", [](CShader* pShader, const std::string& val) { ShaderVersion(pShader, SHADER_TYPE_GS, val); } },
		{ "pixel_version", [](CShader* pShader, const std::string& val) { ShaderVersion(pShader, SHADER_TYPE_PS, val); } },
		{ "compute_version", [](CShader* pShader, const std::string& val) { ShaderVersion(pShader, SHADER_TYPE_CS, val); } },

		{ "topology", [](CShader* pShader, const std::string& val) { ShaderTopology(pShader, val); } },
		{ "fill", [](CShader* pShader, const std::string& val) { ShaderFillMode(pShader, val); } },
		{ "cull", [](CShader* pShader, const std::string& val) { ShaderCullMode(pShader, val); } },
		{ "antialiased_lines", [](CShader* pShader, const std::string& val) { ShaderAntialiasedLines(pShader, val); } },
		{ "color", [](CShader* pShader, const std::string& val) { ShaderColor(pShader, val); } },
		{ "depth", [](CShader* pShader, const std::string& val) { ShaderDepth(pShader, val); } },
		{ "depth_read", [](CShader* pShader, const std::string& val) { ShaderDepthRead(pShader, val); } },
		{ "depth_write", [](CShader* pShader, const std::string& val) { ShaderDepthWrite(pShader, val); } },
		{ "blend", [](CShader* pShader, const std::string& val) { ShaderBlend(pShader, val); } },
	};

	// Static method for processing a single property.
	bool CShaderFileProp::ProcessProperty(const Util::CompilerTuple<2, std::string, char>& prop, CShader* pShader)
	{
		auto elem = PROP_MAP.find(prop.GetElement(0));

		if(elem != PROP_MAP.end())
		{
			elem->second(pShader, prop.GetElement(1));
			return true;
		}

		return false;
	}
};
