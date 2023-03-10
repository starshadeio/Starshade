//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CGraphicsData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGRAPHICSDATA_H
#define CGRAPHICSDATA_H

#include <Math/CMathColor.h>
#include <Globals/CGlobals.h>
#include <Utilities/CMacroUtil.h>

namespace Graphics {
	//-----------------------------------------------------------------------------------------------
	// API info.
	//-----------------------------------------------------------------------------------------------

	struct AdapterData {
		wchar_t description[128];
		u32 index;
		u32 vendorId;
		u32 deviceId;
		u32 subSysId;
		u32 revision;
		size_t dedicatedVideoMemory;
		size_t dedicatedSystemMemory;
		size_t sharedSystemMemory;
	};

	enum ADAPTER_ORDER { 
		ADAPTER_ORDER_EMPTY,
		ADAPTER_ORDER_SORTED,
		ADAPTER_ORDER_UNSORTED
	};
	
	//-----------------------------------------------------------------------------------------------
	// Screenshot info.
	//-----------------------------------------------------------------------------------------------
	
	enum IMAGE_FILETYPE
	{
		IMAGE_FILETYPE_DDS,
		IMAGE_FILETYPE_BMP,
		IMAGE_FILETYPE_ICO,
		IMAGE_FILETYPE_GIF,
		IMAGE_FILETYPE_JPEG,
		IMAGE_FILETYPE_PNG,
		IMAGE_FILETYPE_TIFF,
		IMAGE_FILETYPE_WMP,
	};

	//-----------------------------------------------------------------------------------------------
	// Format info.
	//-----------------------------------------------------------------------------------------------

	enum GFX_FORMAT {
		GFX_FORMAT_UNKNOWN,
		GFX_FORMAT_R32G32B32A32_TYPELESS,
		GFX_FORMAT_R32G32B32A32_FLOAT,
		GFX_FORMAT_R32G32B32A32_UINT,
		GFX_FORMAT_R32G32B32A32_SINT,
		GFX_FORMAT_R32G32B32_TYPELESS,
		GFX_FORMAT_R32G32B32_FLOAT,
		GFX_FORMAT_R32G32B32_UINT,
		GFX_FORMAT_R32G32B32_SINT,
		GFX_FORMAT_R16G16B16A16_TYPELESS,
		GFX_FORMAT_R16G16B16A16_FLOAT,
		GFX_FORMAT_R16G16B16A16_UNORM,
		GFX_FORMAT_R16G16B16A16_UINT,
		GFX_FORMAT_R16G16B16A16_SNORM,
		GFX_FORMAT_R16G16B16A16_SINT,
		GFX_FORMAT_R32G32_TYPELESS,
		GFX_FORMAT_R32G32_FLOAT,
		GFX_FORMAT_R32G32_UINT,
		GFX_FORMAT_R32G32_SINT,
		GFX_FORMAT_R32G8X24_TYPELESS,
		GFX_FORMAT_D32_FLOAT_S8X24_UINT,
		GFX_FORMAT_R32_FLOAT_X8X24_TYPELESS,
		GFX_FORMAT_X32_TYPELESS_G8X24_UINT,
		GFX_FORMAT_R10G10B10A2_TYPELESS,
		GFX_FORMAT_R10G10B10A2_UNORM,
		GFX_FORMAT_R10G10B10A2_UINT,
		GFX_FORMAT_R11G11B10_FLOAT,
		GFX_FORMAT_R8G8B8A8_TYPELESS,
		GFX_FORMAT_R8G8B8A8_UNORM,
		GFX_FORMAT_R8G8B8A8_UNORM_SRGB,
		GFX_FORMAT_R8G8B8A8_UINT,
		GFX_FORMAT_R8G8B8A8_SNORM,
		GFX_FORMAT_R8G8B8A8_SINT,
		GFX_FORMAT_R16G16_TYPELESS,
		GFX_FORMAT_R16G16_FLOAT,
		GFX_FORMAT_R16G16_UNORM,
		GFX_FORMAT_R16G16_UINT,
		GFX_FORMAT_R16G16_SNORM,
		GFX_FORMAT_R16G16_SINT,
		GFX_FORMAT_R32_TYPELESS,
		GFX_FORMAT_D32_FLOAT,
		GFX_FORMAT_R32_FLOAT,
		GFX_FORMAT_R32_UINT,
		GFX_FORMAT_R32_SINT,
		GFX_FORMAT_R24G8_TYPELESS,
		GFX_FORMAT_D24_UNORM_S8_UINT,
		GFX_FORMAT_R24_UNORM_X8_TYPELESS,
		GFX_FORMAT_X24_TYPELESS_G8_UINT,
		GFX_FORMAT_R8G8_TYPELESS,
		GFX_FORMAT_R8G8_UNORM,
		GFX_FORMAT_R8G8_UINT,
		GFX_FORMAT_R8G8_SNORM,
		GFX_FORMAT_R8G8_SINT,
		GFX_FORMAT_R16_TYPELESS,
		GFX_FORMAT_R16_FLOAT,
		GFX_FORMAT_D16_UNORM,
		GFX_FORMAT_R16_UNORM,
		GFX_FORMAT_R16_UINT,
		GFX_FORMAT_R16_SNORM,
		GFX_FORMAT_R16_SINT,
		GFX_FORMAT_R8_TYPELESS,
		GFX_FORMAT_R8_UNORM,
		GFX_FORMAT_R8_UINT,
		GFX_FORMAT_R8_SNORM,
		GFX_FORMAT_R8_SINT,
		GFX_FORMAT_A8_UNORM,
		GFX_FORMAT_R1_UNORM,
		GFX_FORMAT_R9G9B9E5_SHAREDEXP,
		GFX_FORMAT_R8G8_B8G8_UNORM,
		GFX_FORMAT_G8R8_G8B8_UNORM,
		GFX_FORMAT_BC1_TYPELESS,
		GFX_FORMAT_BC1_UNORM,
		GFX_FORMAT_BC1_UNORM_SRGB,
		GFX_FORMAT_BC2_TYPELESS,
		GFX_FORMAT_BC2_UNORM,
		GFX_FORMAT_BC2_UNORM_SRGB,
		GFX_FORMAT_BC3_TYPELESS,
		GFX_FORMAT_BC3_UNORM,
		GFX_FORMAT_BC3_UNORM_SRGB,
		GFX_FORMAT_BC4_TYPELESS,
		GFX_FORMAT_BC4_UNORM,
		GFX_FORMAT_BC4_SNORM,
		GFX_FORMAT_BC5_TYPELESS,
		GFX_FORMAT_BC5_UNORM,
		GFX_FORMAT_BC5_SNORM,
		GFX_FORMAT_B5G6R5_UNORM,
		GFX_FORMAT_B5G5R5A1_UNORM,
		GFX_FORMAT_B8G8R8A8_UNORM,
		GFX_FORMAT_B8G8R8X8_UNORM,
		GFX_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
		GFX_FORMAT_B8G8R8A8_TYPELESS,
		GFX_FORMAT_B8G8R8A8_UNORM_SRGB,
		GFX_FORMAT_B8G8R8X8_TYPELESS,
		GFX_FORMAT_B8G8R8X8_UNORM_SRGB,
		GFX_FORMAT_BC6H_TYPELESS,
		GFX_FORMAT_BC6H_UF16,
		GFX_FORMAT_BC6H_SF16,
		GFX_FORMAT_BC7_TYPELESS,
		GFX_FORMAT_BC7_UNORM,
		GFX_FORMAT_BC7_UNORM_SRGB,
		GFX_FORMAT_AYUV,
		GFX_FORMAT_Y410,
		GFX_FORMAT_Y416,
		GFX_FORMAT_NV12,
		GFX_FORMAT_P010,
		GFX_FORMAT_P016,
		GFX_FORMAT_420_OPAQUE,
		GFX_FORMAT_YUY2,
		GFX_FORMAT_Y210,
		GFX_FORMAT_Y216,
		GFX_FORMAT_NV11,
		GFX_FORMAT_AI44,
		GFX_FORMAT_IA44,
		GFX_FORMAT_P8,
		GFX_FORMAT_A8P8,
		GFX_FORMAT_B4G4R4A4_UNORM,
		GFX_FORMAT_P208,
		GFX_FORMAT_V208,
		GFX_FORMAT_V408,
		GFX_FORMAT_FORCE_UINT
	};
	
	//-----------------------------------------------------------------------------------------------
	// Render info.
	//-----------------------------------------------------------------------------------------------
	
	enum DESC_HEAP_TYPE {
		DHT_RTV,
		DHT_DSV,
		DHT_CSU,
	};

	enum COLOR_WRITE_ENABLE_FLAGS : u8 {
		COLOR_WRITE_ENABLE_RED = 0x1,
		COLOR_WRITE_ENABLE_GREEN = 0x2,
		COLOR_WRITE_ENABLE_BLUE = 0x4,
		COLOR_WRITE_ENABLE_ALPHA = 0x8,
		COLOR_WRITE_ENABLE_ALL = 0xF
	};

	DEFINE_ENUMERATOR_FLAG_OPERATORS(COLOR_WRITE_ENABLE_FLAGS);
	
	enum CLEAR_MODE : u8 {
		CLEAR_NONE = 0x0,
		CLEAR_DEPTH = 0x1,
		CLEAR_STENCIL = 0x2,
		CLEAR_COLOR = 0x4,
		CLEAR_ALL = 0x7,
	};

	DEFINE_ENUMERATOR_FLAG_OPERATORS(CLEAR_MODE);
	
	struct DepthStencil {
		float depth;
		u8 stencil;
	};

	struct ClearValue {
		GFX_FORMAT format;
		Math::Color color;
		DepthStencil depthStencil;

		ClearValue() { memset(this, 0, sizeof(ClearValue)); }
	};

	struct Viewport {
		float topLeftX;
		float topLeftY;
		float width;
		float height;
		float minDepth;
		float maxDepth;
  };
	
	//-----------------------------------------------------------------------------------------------
	// Blend info.
	//-----------------------------------------------------------------------------------------------
	
	enum GFX_BLEND {
		GFX_BLEND_ZERO,
		GFX_BLEND_ONE,
		GFX_BLEND_SRC_COLOR,
		GFX_BLEND_INV_SRC_COLOR,
		GFX_BLEND_SRC_ALPHA,
		GFX_BLEND_INV_SRC_ALPHA,
		GFX_BLEND_DEST_ALPHA,
		GFX_BLEND_INV_DEST_ALPHA,
		GFX_BLEND_DEST_COLOR,
		GFX_BLEND_INV_DEST_COLOR,
		GFX_BLEND_SRC_ALPHA_SAT,
		GFX_BLEND_BLEND_FACTOR,
		GFX_BLEND_INV_BLEND_FACTOR,
		GFX_BLEND_SRC1_COLOR,
		GFX_BLEND_INV_SRC1_COLOR,
		GFX_BLEND_SRC1_ALPHA,
		GFX_BLEND_INV_SRC1_ALPHA,
		GFX_BLEND_ALPHA_FACTOR,
		GFX_BLEND_INV_ALPHA_FACTOR
	};

	enum GFX_BLEND_OP {
		GFX_BLEND_OP_ADD,
		GFX_BLEND_OP_SUBTRACT,
		GFX_BLEND_OP_REV_SUBTRACT,
		GFX_BLEND_OP_MIN,
		GFX_BLEND_OP_MAX
	};

	enum GFX_LOGIC_OP {
		GFX_LOGIC_OP_CLEAR,
		GFX_LOGIC_OP_SET,
		GFX_LOGIC_OP_COPY,
		GFX_LOGIC_OP_COPY_INVERTED,
		GFX_LOGIC_OP_NOOP,
		GFX_LOGIC_OP_INVERT,
		GFX_LOGIC_OP_AND,
		GFX_LOGIC_OP_NAND,
		GFX_LOGIC_OP_OR,
		GFX_LOGIC_OP_NOR,
		GFX_LOGIC_OP_XOR,
		GFX_LOGIC_OP_EQUIV,
		GFX_LOGIC_OP_AND_REVERSE,
		GFX_LOGIC_OP_AND_INVERTED,
		GFX_LOGIC_OP_OR_REVERSE,
		GFX_LOGIC_OP_OR_INVERTED
	};

	//-----------------------------------------------------------------------------------------------
	// Primitive info.
	//-----------------------------------------------------------------------------------------------
	
	enum INPUT_CLASSIFICATION {
		INPUT_PER_VERTEX_DATA,
		INPUT_PER_INSTANCE_DATA,
	};

	enum PRIMITIVE_TOPOLOGY {
		PRIMITIVE_TOPOLOGY_POINTLIST,
		PRIMITIVE_TOPOLOGY_LINELIST,
		PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
		PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST,
	};

	enum PRIMITIVE_TOPOLOGY_TYPE {
		PRIMITIVE_TOPOLOGY_TYPE_POINTLIST,
		PRIMITIVE_TOPOLOGY_TYPE_LINELIST,
		PRIMITIVE_TOPOLOGY_TYPE_TRIANGLELIST,
		PRIMITIVE_TOPOLOGY_TYPE_TRIANGLESTRIP,
		PRIMITIVE_TOPOLOGY_TYPE_PATCH,
	};

	enum FILL_MODE {
		FILL_MODE_WIREFRAME,
		FILL_MODE_SOLID,
	};

	enum CULL_MODE {
		CULL_MODE_NONE,
		CULL_MODE_FRONT,
		CULL_MODE_BACK,
	};

	struct InputElementDesc {
		const char* pSemanticName;
		u32 semanticIndex;
		GFX_FORMAT format;
		u32 inputSlot;
		u32 alignedByteOffset;
		INPUT_CLASSIFICATION inputSlotClass;
		u32 instanceDataStepRate;
	};

	//-----------------------------------------------------------------------------------------------
	// Shader info.
	//-----------------------------------------------------------------------------------------------

	enum SHADER_FEATURE_LEVEL {
		SHADER_FEATURE_LEVEL_1_1,
		SHADER_FEATURE_LEVEL_2_0,
		SHADER_FEATURE_LEVEL_3_0,
		SHADER_FEATURE_LEVEL_4_0,
		SHADER_FEATURE_LEVEL_4_1,
		SHADER_FEATURE_LEVEL_5_0,
		SHADER_FEATURE_LEVEL_5_1,
		SHADER_FEATURE_LEVEL_6_0,
	};

	enum SHADER_TYPE {
		SHADER_TYPE_VS,
		SHADER_TYPE_HS,
		SHADER_TYPE_DS,
		SHADER_TYPE_GS,
		SHADER_TYPE_PS,
		SHADER_TYPE_CS,
		SHADER_TYPE_COUNT,
	};

	enum SHADER_VISIBILITY : u8 {
		SHADER_VISIBILITY_NONE = 0x0,
		SHADER_VISIBILITY_VERTEX = 0x1,
		SHADER_VISIBILITY_HULL = 0x2,
		SHADER_VISIBILITY_DOMAIN = 0x4,
		SHADER_VISIBILITY_GEOMETRY = 0x8,
		SHADER_VISIBILITY_PIXEL = 0x10,
		SHADER_VISIBILITY_ALL = 0x1F,
	};

	DEFINE_ENUMERATOR_FLAG_OPERATORS(SHADER_VISIBILITY);

	struct ShaderMacro {
		const char* name;
		const char* definition;
	};

	struct ByteCode {
		char* pData;
		size_t tSize;
	};
	
	//-----------------------------------------------------------------------------------------------
	// Root signature info.
	//-----------------------------------------------------------------------------------------------
	
	enum ROOT_SIGNATURE_FLAGS {
		ROOT_SIGNATURE_FLAG_NONE = 0,
		ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT = 0x1,
		ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS = 0x2,
		ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS = 0x4,
		ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS = 0x8,
		ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS = 0x10,
		ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS = 0x20,
		ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT = 0x40,
		ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE = 0x80,
		ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS = 0x100,
		ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS = 0x200,
		ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED = 0x400,
		ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED = 0x800
	};

	DEFINE_ENUMERATOR_FLAG_OPERATORS(ROOT_SIGNATURE_FLAGS);

	enum ROOT_PARAMETER_TYPE {
		ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
		ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
		ROOT_PARAMETER_TYPE_CBV,
		ROOT_PARAMETER_TYPE_SRV,
		ROOT_PARAMETER_TYPE_UAV
	};

	enum ROOT_DESCRIPTOR_FLAGS {
		ROOT_DESCRIPTOR_FLAG_NONE = 0,
		ROOT_DESCRIPTOR_FLAG_DATA_VOLATILE = 0x2,
		ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE = 0x4,
		ROOT_DESCRIPTOR_FLAG_DATA_STATIC = 0x8
	};

	DEFINE_ENUMERATOR_FLAG_OPERATORS(ROOT_DESCRIPTOR_FLAGS);

	enum DESCRIPTOR_RANGE_FLAGS {
		DESCRIPTOR_RANGE_FLAG_NONE = 0,
		DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE = 0x1,
		DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE = 0x2,
		DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE = 0x4,
		DESCRIPTOR_RANGE_FLAG_DATA_STATIC = 0x8,
		DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_STATIC_KEEPING_BUFFER_BOUNDS_CHECKS = 0x10000
	};

	DEFINE_ENUMERATOR_FLAG_OPERATORS(DESCRIPTOR_RANGE_FLAGS);

	enum DESCRIPTOR_RANGE_TYPE {
		DESCRIPTOR_RANGE_TYPE_SRV,
		DESCRIPTOR_RANGE_TYPE_UAV,
		DESCRIPTOR_RANGE_TYPE_CBV,
		DESCRIPTOR_RANGE_TYPE_SAMPLER
	};
	
	struct DescriptorRange {
    DESCRIPTOR_RANGE_TYPE rangeType;
    u32 numDescriptors;
    u32 baseShaderRegister;
    u32 registerSpace;
    DESCRIPTOR_RANGE_FLAGS flags;
    u32 offsetInDescriptorsFromTableStart;
	};

	struct RootDescriptorTable {
		u32 numDescriptorRanges;
		u32 rangeOffset;
	};

	struct RootConstants {
		u32 shaderRegister;
		u32 registerSpace;
		u32 num32BitValues;
	};

	struct RootDescriptor {
		u32 shaderRegister;
		u32 registerSpace;
		ROOT_DESCRIPTOR_FLAGS flags;
	};

	struct RootParameter {
		ROOT_PARAMETER_TYPE parameterType;

		union {
			RootDescriptorTable descriptorTable;
			RootConstants constants;
			RootDescriptor descriptor;
		};

		SHADER_VISIBILITY shaderVisibility;
	};
	
	//-----------------------------------------------------------------------------------------------
	// Constant buffer view info.
	//-----------------------------------------------------------------------------------------------
	
	enum CONSTANT_BUFFER_TYPE {
		CBT_ROOT_DESCRIPTOR,
		CBT_ROOT_CONSTANTS,
	};

	//-----------------------------------------------------------------------------------------------
	// Unordered access view info.
	//-----------------------------------------------------------------------------------------------
	
	enum UNORDERED_ACCESS_TYPE {
		UAT_RWBUFFER,
	};

	//-----------------------------------------------------------------------------------------------
	//  Texture info.
	//-----------------------------------------------------------------------------------------------

	enum GFX_FILTER {
		GFX_FILTER_MIN_MAG_MIP_POINT,
		GFX_FILTER_MIN_MAG_POINT_MIP_LINEAR,
		GFX_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
		GFX_FILTER_MIN_POINT_MAG_MIP_LINEAR,
		GFX_FILTER_MIN_LINEAR_MAG_MIP_POINT,
		GFX_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		GFX_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		GFX_FILTER_MIN_MAG_MIP_LINEAR,
		GFX_FILTER_ANISOTROPIC,
		GFX_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
		GFX_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
		GFX_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
		GFX_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
		GFX_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
		GFX_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		GFX_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		GFX_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR,
		GFX_FILTER_COMPARISON_ANISOTROPIC,
		GFX_FILTER_MINIMUM_MIN_MAG_MIP_POINT,
		GFX_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR,
		GFX_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
		GFX_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR,
		GFX_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT,
		GFX_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		GFX_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT,
		GFX_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR,
		GFX_FILTER_MINIMUM_ANISOTROPIC,
		GFX_FILTER_MAXIMUM_MIN_MAG_MIP_POINT,
		GFX_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,
		GFX_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
		GFX_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,
		GFX_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,
		GFX_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		GFX_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,
		GFX_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR,
		GFX_FILTER_MAXIMUM_ANISOTROPIC
	};

	enum TEXTURE_ADDRESS_MODE {
		TEXTURE_ADDRESS_MODE_WRAP,
		TEXTURE_ADDRESS_MODE_MIRROR,
		TEXTURE_ADDRESS_MODE_CLAMP,
		TEXTURE_ADDRESS_MODE_BORDER,
		TEXTURE_ADDRESS_MODE_MIRROR_ONCE
	};
	
	enum COMPARISON_FUNC {
		COMPARISON_FUNC_NEVER,
		COMPARISON_FUNC_LESS,
		COMPARISON_FUNC_EQUAL,
		COMPARISON_FUNC_LESS_EQUAL,
		COMPARISON_FUNC_GREATER,
		COMPARISON_FUNC_NOT_EQUAL,
		COMPARISON_FUNC_GREATER_EQUAL,
		COMPARISON_FUNC_ALWAYS
	};

	enum STATIC_BORDER_COLOR {
		STATIC_BORDER_COLOR_TRANSPARENT_BLACK,
		STATIC_BORDER_COLOR_OPAQUE_BLACK,
		STATIC_BORDER_COLOR_OPAQUE_WHITE
	};

	enum TEXTURE_DATA_TYPE {
		TDT_STATIC,
		TDT_STATIC_WHILE_SET_AT_EXECUTE,
		TDT_VOLATILE,
	};

	struct StaticSamplerDesc {
    GFX_FILTER filter;
    TEXTURE_ADDRESS_MODE addressU;
    TEXTURE_ADDRESS_MODE addressV;
    TEXTURE_ADDRESS_MODE addressW;
    float mipLODBias;
    u32 maxAnisotropy;
    COMPARISON_FUNC comparisonFunc;
    STATIC_BORDER_COLOR borderColor;
    float minLOD;
    float maxLOD;
    u32 shaderRegister;
    u32 registerSpace;
    SHADER_VISIBILITY shaderVisibility;
	};
	
	//-----------------------------------------------------------------------------------------------
	// Root signature utilities.
	//-----------------------------------------------------------------------------------------------
	
	inline DESCRIPTOR_RANGE_FLAGS ConvertGFXTextureDataTypeToDescRangeFlags(TEXTURE_DATA_TYPE dataType) {
		switch(dataType) {
			case TDT_STATIC:
				return DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
			case TDT_STATIC_WHILE_SET_AT_EXECUTE:
				return DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
			case TDT_VOLATILE:
				return DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE;
			default:
				return DESCRIPTOR_RANGE_FLAG_DATA_STATIC;
		}
	}

	//-----------------------------------------------------------------------------------------------
	//  Format utilities.
	//-----------------------------------------------------------------------------------------------

	inline GFX_FORMAT ConvertGFXDepthStencilFormatToTex(GFX_FORMAT format) {
		switch(format) {
		case GFX_FORMAT_D16_UNORM:
			return GFX_FORMAT_R16_TYPELESS;
		case GFX_FORMAT_D24_UNORM_S8_UINT:
			return GFX_FORMAT_R24G8_TYPELESS;
		case GFX_FORMAT_D32_FLOAT:
			return GFX_FORMAT_R32_TYPELESS;
		case GFX_FORMAT_D32_FLOAT_S8X24_UINT:
			return GFX_FORMAT_R32G8X24_TYPELESS;
		default:
			return GFX_FORMAT_UNKNOWN;
		}
	}

	inline GFX_FORMAT ConvertGFXDepthStencilFormatToSRV(GFX_FORMAT format) {
		switch(format) {
		case GFX_FORMAT_D16_UNORM:
			return GFX_FORMAT_R16_UNORM;
		case GFX_FORMAT_D24_UNORM_S8_UINT:
			return GFX_FORMAT_R24_UNORM_X8_TYPELESS;
		case GFX_FORMAT_D32_FLOAT:
			return GFX_FORMAT_R32_FLOAT;
		case GFX_FORMAT_D32_FLOAT_S8X24_UINT:
			return GFX_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		default:
			return GFX_FORMAT_UNKNOWN;
		}
	}
};

#endif
