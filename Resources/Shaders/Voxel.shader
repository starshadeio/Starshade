//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shaders/Voxel.shader
//
//-------------------------------------------------------------------------------------------------

#ifndef WIRE
//#define WIRE
#endif

$
	/* { pSemanticName, semanticIndex, format, inputSlot, alignedByteOffset, inputSlotClass, instanceDataStepRate } */
	input							( POSITION:0:R32G32B32_FLOAT:0:0:VERTEX:0 )
	input							( NORMAL:0:R32G32B32_FLOAT:0:12:VERTEX:0 )
	input							( TANGENT:0:R32G32B32_FLOAT:0:24:VERTEX:0 )
	input							( TEXCOORD:0:R32G32B32A32_FLOAT:0:36:VERTEX:0 )
	
	cbv								( CONSTANTS:VERTEX:DYNAMIC )
	
#ifndef WIRE
	/* { dataType, shaderVisibility } */
	texture						( STATIC:PIXEL )

	/* { filter, addressU, addressV, addressW, mipLODBias, minLOD, maxLOD, maxAnisotropy, shaderVisibility } */
	sampler						( MIN_MAG_MIP_POINT:CLAMP:CLAMP:CLAMP:0.0f:0.0f:FLT_MAX:0:PIXEL )
#endif

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

#ifdef WIRE
	antialiased_lines	true
	fill							WIREFRAME
	depth_write				false

	blend							( TRUE:FALSE:SRC_ALPHA:INV_SRC_ALPHA:ADD:SRC_ALPHA:INV_SRC_ALPHA:ADD:NOOP:R|G|B|A )
#endif

	topology					TRIANGLELIST
	color							R32G32B32A32_FLOAT|R32G32B32A32_FLOAT
	depth							D32_FLOAT
$

#define TILE_SIZE_X 0.125f
#define TILE_SIZE_Y 0.03125f

cbuffer DrawBuffer : register(b0)
{
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
};

struct a2v
{
	float4 Position : POSITION;
	float4 Normal : NORMAL;
	float4 Tangent : TANGENT;
	float4 TexCoord : TEXCOORD;
};

struct v2p
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float4 TexCoord : TEXCOORD;
};

struct p2f
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
};

v2p VShader(in a2v input)
{
	v2p output;
	
	input.Position.w = 1.0f;
	output.Position = mul(input.Position, World);
	output.Position.xyz -= float3(View._41, View._42, View._43);
	output.Position.xyz = mul(output.Position.xyz, transpose((float3x3)View));
	output.Position = mul(output.Position, Proj);

	output.Normal.xyz = mul(input.Normal.xyz, (float3x3)World);

	output.TexCoord = input.TexCoord;

	return output;
}

#ifndef WIRE
Texture2D diffuse_texture : register(t0);
SamplerState g_sampler : register(s0);
#endif

p2f PShader(in v2p input)
{
	p2f output;
	
#ifndef WIRE
	float2 texCoord = frac(input.TexCoord.xy) * float2(TILE_SIZE_X - 2e-4f, TILE_SIZE_Y - 2e-4f) + input.TexCoord.zw + 1e-4f;
	output.Color = saturate(diffuse_texture.Sample(g_sampler, texCoord));
#endif

#ifdef WIRE
	output.Color = float4(0.0f, 0.0f, 0.0f, 1.0f);
#endif

	output.Normal = float4(normalize(input.Normal.xyz), 0.0f);
	return output;
}
