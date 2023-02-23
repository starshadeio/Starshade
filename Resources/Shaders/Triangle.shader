//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shader/Triangle.shader
//
//-------------------------------------------------------------------------------------------------

$
	/* { pSemanticName, semanticIndex, format, inputSlot, alignedByteOffset, inputSlotClass, instanceDataStepRate } */
	input							( POSITION:0:R32G32B32_FLOAT:0:0:VERTEX:0 )
	input							( TEXCOORD:0:R32G32_FLOAT:0:12:VERTEX:0 )
	
	cbv								( DESCRIPTOR:VERTEX:DYNAMIC )
	uav								( VERTEX )

	/* { dataType, shaderVisibility } */
	texture						( STATIC:PIXEL )

	/* { filter, addressU, addressV, addressW, mipLODBias, minLOD, maxLOD, maxAnisotropy, shaderVisibility } */
	sampler						( MIN_MAG_MIP_POINT:BORDER:BORDER:BORDER:0.0f:0.0f:FLT_MAX,:0:PIXEL )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R8G8B8A8_UNORM
$

cbuffer DataBuffer : register(b0) {
	float4 Offset;
	float4 padding[15];
};

struct a2v {
	float4 Position : POSITION;
	float2 UV : TEXCOORD;
};

struct v2p {
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};

struct p2f {
	float4 Color : SV_TARGET;
};

RWBuffer<float4> g_uav : register(u0);

v2p VShader(in a2v input) {
	v2p output;

	output.Position = input.Position + Offset + g_uav[0];
	output.UV = input.UV;

	return output;
}

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

p2f PShader(in v2p input) {
	p2f output;
	output.Color = g_texture.Sample(g_sampler, input.UV);
	return output;
}
