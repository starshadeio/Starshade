//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shader/shader.hlsl
//
//-------------------------------------------------------------------------------------------------

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

RWBuffer<float4> g_uav : register(u0);

struct p2f {
	float4 Color : SV_TARGET;
};

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
