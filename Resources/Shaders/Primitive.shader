//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: ../Resources/Shaders/VoxelEditor/Voxel.shader
//
//-------------------------------------------------------------------------------------------------

$
	/* { pSemanticName, semanticIndex, format, inputSlot, alignedByteOffset, inputSlotClass, instanceDataStepRate } */
	input							( POSITION:0:R32G32B32_FLOAT:0:0:VERTEX:0 )
	input							( NORMAL:0:R32G32B32_FLOAT:0:12:VERTEX:0 )
#ifdef TEXTURED
	input							( TEXCOORD:0:R32G32_FLOAT:0:24:VERTEX:0 )
#endif

	cbv								( DESCRIPTOR:PIXEL:FIXED )
	cbv								( CONSTANTS:VERTEX:DYNAMIC )
	
#ifdef TEXTURED
	/* { dataType, shaderVisibility } */
	texture						( STATIC:PIXEL )

	/* { filter, addressU, addressV, addressW, mipLODBias, minLOD, maxLOD, maxAnisotropy, shaderVisibility } */
	sampler						( MIN_MAG_MIP_POINT:CLAMP:CLAMP:CLAMP:0.0f:0.0f:FLT_MAX:0:PIXEL )
#endif

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R32G32B32A32_FLOAT|R32G32B32A32_FLOAT
	depth							D32_FLOAT
$

cbuffer DataBuffer : register(b0)
{
	float4 Color;
	float4 padding0[15];
};

cbuffer MatrixBuffer : register(b1)
{
	float4x4 VP;
	float4x4 World;
};

struct a2v
{
	float4 Position : POSITION;
	float4 Normal : NORMAL;

#ifdef TEXTURED
	float2 TexCoord : TEXCOORD;
#endif
};

struct v2p
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;

#ifdef TEXTURED
	float2 TexCoord : TEXCOORD;
#endif
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
	output.Position = mul(output.Position, VP);

	output.Normal.xyz = mul(input.Normal.xyz, (float3x3)World);
	
#ifdef TEXTURED
	output.TexCoord = input.TexCoord;
#endif

	return output;
}

#ifdef TEXTURED
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);
#endif

p2f PShader(in v2p input)
{
	p2f output;

#ifdef TEXTURED
	output.Color = saturate(g_texture.Sample(g_sampler, input.TexCoord) * Color);
#endif
#ifndef TEXTURED
	output.Color = Color;
#endif

	output.Normal = float4(normalize(input.Normal.xyz), 0.0f);

	return output;
}
