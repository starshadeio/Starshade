//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shaders/PostSky.shader
//
//-------------------------------------------------------------------------------------------------

$
	cbv								( DESCRIPTOR:PIXEL:DYNAMIC )

	texture						( STATIC_EXECUTE:PIXEL )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R32G32B32A32_FLOAT
$

#include "Includes/ProcSky.hlsli"

cbuffer DataBuffer : register(b0) {
	float4x4 View;
	float Aspect;
	float Fov;
	float SkyInterp;
	float padding0;
	float4 padding1[11];
};

// Vertex output structure.
struct v2p {
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

// Pixel output structure.
struct p2f {
	float4 Color : SV_TARGET0;
};

// Vertex shader function.
v2p VShader(in uint i : SV_VertexID) {
	v2p output;
	
	// Full screen triangle.
	output.Position = float4(((i & 1) - 0.25f), ((i >> 1) & 1) - 0.25f, 0.0f, 0.25f);
	output.TexCoord = float2((i & 1) * 2.0f, 1.0f - ((i >> 1) & 1) * 2.0f);

	return output;
}

Texture2D depthTexture : register(t0);

// Pixel shader function.
p2f PShader(in v2p input) {	
	p2f output;

	float half_height = tan(Fov * 0.5f);
	float half_width = Aspect * half_height;
	float3 u = -View._11_21_31;
	float3 v = View._12_22_32;
	float3 w = View._13_23_33;
	float3 lower_left_corner = -half_width * u - half_height * v - w;
	float3 horizontal = u * half_width * 2.0f;
	float3 vertical = v * half_height * 2.0f;

	float3 dir = -normalize(lower_left_corner + horizontal * input.TexCoord.x + vertical * input.TexCoord.y);
	output.Color = lerp(float4(0.005f, 0.005f, 0.005f, 1.0f), ProcSky(dir), SkyInterp);

	return output;
}
