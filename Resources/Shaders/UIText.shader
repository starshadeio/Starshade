//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shaders/UIText.shader
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
	cbv								( DESCRIPTOR:ALL:FIXED )

	/* { dataType, shaderVisibility } */
	texture						( STATIC:PIXEL )

	/* { filter, addressU, addressV, addressW, mipLODBias, minLOD, maxLOD, maxAnisotropy, shaderVisibility } */
	sampler						( MIN_MAG_MIP_LINEAR:CLAMP:CLAMP:CLAMP:0.0f:0.0f:FLT_MAX:0:PIXEL )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R32G32B32A32_FLOAT|R32G32B32A32_FLOAT|R32G32B32A32_FLOAT|R32G32B32A32_FLOAT
	depth_write				FALSE
	
	blend							( TRUE:FALSE:SRC_ALPHA:INV_SRC_ALPHA:ADD:SRC_ALPHA:INV_SRC_ALPHA:ADD:NOOP:R|G|B|A )
$

#include "Includes/Color.hlsli"

cbuffer DataBuffer : register(b0) {
	float4 ScreenSize;
	float4 padding0[15];
};

cbuffer MatrixBuffer : register(b1) {
	float4x4 WVP;
};

cbuffer FontBuffer : register(b2)
{
#ifdef OUTLINE
	float2 OutlineMin;
	float2 OutlineMax;
	float4 OutlineColor;
#endif
#ifdef OUTER_GLOW
	float2 GlowOffsetUV;
	float2 GlowRange;
	float4 GlowColor;
#endif
#ifdef SOFT_EDGES
	float2 SoftEdgeRange;
	float2 padding1;
#else
	float AlphaCutoff;
	float3 padding1;
#endif
	float4 padding2[10];
};

struct a2v {
	float2 Position : POSITION;
	float2 TexCoord : TEXCOORD;
	float4 InstTexCoord : INSTTEXCOORD;
	float4 InstColor : INSTCOLOR;
	float4 InstM0 : INSTMATRIX0;
	float2 InstM1 : INSTMATRIX1;
};

struct v2p {
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float4 Color : COLOR;
#ifdef OUTER_GLOW
	float2 GlowOffset : TEXCOORD1;
#endif
};

struct p2f {
	float4 Color : SV_TARGET0;
};

v2p VShader(in a2v input) {
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
	
#ifdef OUTER_GLOW
	output.GlowOffset = output.TexCoord + GlowOffsetUV * input.InstUVAdj.zw;
#endif

	return output;
}

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

p2f PShader(in v2p input) {
	p2f output;

	output.Color.rgb = input.Color.rgb;
	output.Color.a = g_texture.Sample(g_sampler, input.TexCoord).r;
	float alphaMask = output.Color.a;

#ifdef OUTLINE
	if(alphaMask >= OutlineMin.x && alphaMask <= OutlineMax.y) {
		float blend;
		if(alphaMask <= OutlineMin.y) {
			blend = smoothstep(OutlineMin.x, OutlineMin.y, alphaMask);
		} else {
			blend = smoothstep(OutlineMax.y, OutlineMax.x, alphaMask);
		}

		output.Color = lerp(output.Color, OutlineColor, blend);
	}
#endif

#ifdef SOFT_EDGES
	output.Color.a = smoothstep(SoftEdgeRange.x, SoftEdgeRange.y,	alphaMask);
#else
	output.Color.a = step(AlphaCutoff, alphaMask);
#endif

#ifdef OUTER_GLOW
	float glowTexel = fontTexture.Sample(fontSampler, input.GlowOffset).r;
	float4 glowColor = GlowColor * smoothstep(GlowRange.x, GlowRange.y, glowTexel);
	output.Color = lerp(glowColor, output.Color, alphaMask);
#endif
	
	output.Color.a *= input.Color.a;
	output.Color.rgb = LinearToGamma(output.Color.rgb);
	
	return output;
}
