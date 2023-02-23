//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shader/UIImage.shader
//
//-------------------------------------------------------------------------------------------------

$
	/* { pSemanticName, semanticIndex, format, inputSlot, alignedByteOffset, inputSlotClass, instanceDataStepRate } */
	input							( POSITION:0:R32G32_FLOAT:0:0:VERTEX:0 )
	input							( TEXCOORD:0:R32G32_FLOAT:0:8:VERTEX:0 )

	input							( INSTTEXCOORD:0:R32G32B32A32_FLOAT:1:0:INSTANCE:1 )
	input							( INSTCOLOR:0:R32G32B32A32_FLOAT:1:16:INSTANCE:1 )
	input							( INSTMATRIX:0:R32G32B32A32_FLOAT:1:32:INSTANCE:1 )
	input							( INSTMATRIX:1:R32G32_FLOAT:1:48:INSTANCE:1 )
	
	cbv								( DESCRIPTOR:VERTEX:DYNAMIC )
	cbv								( CONSTANTS:VERTEX:DYNAMIC )

	/* { dataType, shaderVisibility } */
#ifdef VIEW
	texture						( STATIC_EXECUTE:PIXEL )
#else
	texture						( STATIC:PIXEL )
#endif

	/* { filter, addressU, addressV, addressW, mipLODBias, minLOD, maxLOD, maxAnisotropy, shaderVisibility } */
#ifdef TEX_POINT
	sampler						( MIN_MAG_MIP_POINT:CLAMP:CLAMP:CLAMP:0.0f:0.0f:FLT_MAX:0:PIXEL )
#endif
#ifdef TEX_LINEAR
	sampler						( MIN_MAG_MIP_LINEAR:CLAMP:CLAMP:CLAMP:0.0f:0.0f:FLT_MAX:0:PIXEL )
#endif

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R32G32B32A32_FLOAT|R32G32B32A32_FLOAT|R32G32B32A32_FLOAT|R32G32B32A32_FLOAT
	depth_write				FALSE
	
	//blend							( TRUE:FALSE:ONE:ZERO:ADD:ONE:ZERO:ADD:NOOP:R|G|B|A )
	blend							( TRUE:FALSE:SRC_ALPHA:INV_SRC_ALPHA:ADD:SRC_ALPHA:INV_SRC_ALPHA:ADD:NOOP:R|G|B|A )
$

#include "Includes/Color.hlsli"

cbuffer DataBuffer : register(b0)
{
	float4 ScreenSize;
	float4 padding0[15];
};

cbuffer MatrixBuffer : register(b1)
{
	float4x4 WVP;
};

struct a2v
{
	float2 Position : POSITION;
	float2 TexCoord : TEXCOORD;
	float4 InstTexCoord : INSTTEXCOORD;
	float4 InstColor : INSTCOLOR;
	float4 InstM0 : INSTMATRIX0;
	float2 InstM1 : INSTMATRIX1;
};

struct v2p
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float4 Color : COLOR;
};

struct p2f
{
	float4 UI : SV_TARGET0;
};

v2p VShader(in a2v input)
{
	v2p output;
	
	// Position.
	output.Position = float4(input.Position.xy, 0.0f, 1.0f);
	output.Position = mul(output.Position, WVP);
	output.Position.xy = mul(float3(output.Position.xy, 1.0f),
		float3x2(input.InstM0, input.InstM1));
	output.Position.xy *= ScreenSize.zw * 2.0f;

	// Texture Coordinates.
	output.TexCoord = input.TexCoord * input.InstTexCoord.zw + input.InstTexCoord.xy;

	// Color.
	output.Color = input.InstColor;

	return output;
}

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

p2f PShader(in v2p input)
{
	p2f output;

	output.UI = g_texture.Sample(g_sampler, input.TexCoord) * input.Color;
#ifndef VIEW
	output.UI.rgb = LinearToGamma(output.UI.rgb);
#endif

	return output;
}
