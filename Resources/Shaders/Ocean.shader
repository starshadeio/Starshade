//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: NetRunner App
//
// File: ../Resources/Shaders/IslandChunk.shader
//
//-------------------------------------------------------------------------------------------------

$
	/* { pSemanticName, semanticIndex, format, inputSlot, alignedByteOffset, inputSlotClass, instanceDataStepRate } */
	input							( POSITION:0:R32G32B32_FLOAT:0:0:VERTEX:0 )
	input							( NORMAL:0:R32G32B32_FLOAT:0:12:VERTEX:0 )
	
	cbv								( DESCRIPTOR:VERTEX:FIXED )
	cbv								( DESCRIPTOR:VERTEX|DOMAIN|PIXEL:DYNAMIC )
	cbv								( DESCRIPTOR:DOMAIN:DYNAMIC )

	hull_entry				HShader
	hull_version			hs_5_0
	domain_entry			DShader
	domain_version		ds_5_0
	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					PATCH
	color							R32G32B32A32_FLOAT|R32G32B32A32_FLOAT|R32G32B32A32_FLOAT
	depth							D32_FLOAT
$

#include "Includes/ProcSky.hlsli"

cbuffer DataBuffer : register(b0) {
	float TessellationScale;
	float TessellationMax;
	float2 padding0;
	float4 paddint1[15];
};

cbuffer DynamicBuffer : register(b1) {
	float4 CameraPosition;
	float4 CameraForward;
	float4 Timer;
	float4 padding2[14];
};

cbuffer MatrixBuffer : register(b2) {
	float4x4 WVP;
	float4 padding3[12];
};

//
// Output/Input structures.
//

struct a2v {
	float4 Position : POSITION;
	float4 Normal : NORMAL;
};

struct v2h {
	float4 Position : POSITION;
	float4 Normal : NORMAL;
	float1 TessFactor : TEXCOORD;
};

struct h2d {
	float4 Position : POSITION;
	float4 Normal : NORMAL;
};

struct d2p {
	float4 Position : SV_POSITION;
	float4 Normal : NORMAL;
	float4 VertexPos : POSITION;
};

struct p2f {
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Position : SV_TARGET2;
};

//
// Vertex shader.
//

v2h VShader(in a2v input) {
	v2h output;
	
	input.Position.w = 1.0f;
	input.Normal.w = 0.0f;

	output.Position = input.Position;
	output.Normal = input.Normal;

	output.TessFactor = (1.0f / (1.0f + max(0.0f, dot(input.Position.xyz - CameraPosition.xyz, CameraForward.xyz)) * TessellationScale)) * TessellationMax;

	return output;
}

//
// Hull shader.
//

struct c2d {
	float EdgeTess[4] : SV_TessFactor;
	float InsideTess[2] : SV_InsideTessFactor;
};

// Patch constant function.
c2d PatchConstantFunction(InputPatch<v2h, 4> inputPatch, uint patchId : SV_PrimitiveID) {
	c2d output;
	
	//output.EdgeTess[0] = (inputPatch[1].TessFactor + inputPatch[2].TessFactor) * 0.5f;
	//output.EdgeTess[1] = (inputPatch[2].TessFactor + inputPatch[0].TessFactor) * 0.5f;
	//output.EdgeTess[2] = (inputPatch[0].TessFactor + inputPatch[1].TessFactor) * 0.5f;
	//output.InsideTess = (inputPatch[0].TessFactor + inputPatch[1].TessFactor + inputPatch[2].TessFactor + inputPatch[3].TessFactor) * 0.25f;

	output.EdgeTess[0] = (inputPatch[0].TessFactor + inputPatch[3].TessFactor) * 0.5f;
	output.EdgeTess[1] = (inputPatch[0].TessFactor + inputPatch[1].TessFactor) * 0.5f;
	output.EdgeTess[2] = (inputPatch[1].TessFactor + inputPatch[2].TessFactor) * 0.5f;
	output.EdgeTess[3] = (inputPatch[3].TessFactor + inputPatch[2].TessFactor) * 0.5f;

	output.InsideTess[0] = output.InsideTess[1] = (inputPatch[0].TessFactor + inputPatch[1].TessFactor + inputPatch[2].TessFactor + inputPatch[3].TessFactor) * 0.25f;

	return output;
}

// Hull shader function.
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
h2d HShader(in InputPatch<v2h, 4> inputPatch, in uint pointId : SV_OutputControlPointID, in uint patchId : SV_PrimitiveID) {
	h2d output;
	
	output.Position = inputPatch[pointId].Position;
	output.Normal = inputPatch[pointId].Normal;

	return output;
}

//
// Helper methods.
//

inline float SineWave(float2 pos, float w, float amp, float t, float phi, float2 dir, float k) {
	return 2.0f * amp * pow(sin(dot(dir, pos) * w + t * phi) * 0.5f + 0.5f, k);
}

inline float SineWaveDx(float2 pos, float w, float amp, float t, float phi, float2 dir, float k) {
	return 2.0f * amp * k * pow(sin(dot(dir, pos) * w + t * phi) * 0.5f + 0.5f, k - 1) * cos(dot(dir, pos) * w + t * phi) * (dir.x * w);
}

inline float SineWaveDy(float2 pos, float w, float amp, float t, float phi, float2 dir, float k) {
	return 2.0f * amp * k * pow(sin(dot(dir, pos) * w + t * phi) * 0.5f + 0.5f, k - 1) * cos(dot(dir, pos) * w + t * phi) * (dir.y * w);
}

inline float3 SineNormal(float2 pos, float w, float amp, float t, float phi, float2 dir, float k) {
	return normalize(float3(-SineWaveDx(pos, w, amp, t, phi, dir, k), 1.0f, -SineWaveDy(pos, w, amp, t, phi, dir, k)));
}

//
// Domain shader.
//

[domain("quad")]
d2p DShader(in c2d input, in float2 uvCoord : SV_DomainLocation, in const OutputPatch<h2d, 4> patch) {
	d2p output;

	float3 vertex = lerp(lerp(patch[0].Position.xyz, patch[1].Position.xyz, uvCoord.x), lerp(patch[3].Position.xyz, patch[2].Position.xyz, uvCoord.x), uvCoord.y);
	float3 normal = lerp(lerp(patch[0].Normal.xyz, patch[1].Normal.xyz, uvCoord.x), lerp(patch[3].Normal.xyz, patch[2].Normal.xyz, uvCoord.x), uvCoord.y);
	
	//float x = vertex.x * 0.1f - Timer.w;
	//float z = vertex.z * 0.1f - Timer.w;

	//vertex.y = pow(sin(z), 6.0f) * 2.0f;
	//normal = normalize(float3(0.0f, 1.0f, 12.0f * pow(sin(z), 5.0f) * -cos(z)));

	vertex.y = SineWave(vertex.xz, 0.125f, 0.4f, Timer.w, 0.7f, normalize(float2(0.2f, -1.0f)), 2.0f) + 
		SineWave(vertex.xz, 0.2f, 0.325f, Timer.w, 0.9f, normalize(float2(-1.0f, -0.8f)), 1.5f) +
		SineWave(vertex.xz, 0.15f, 0.4f, Timer.w, 0.8f, normalize(float2(-0.3f, 1.0f)), 2.0f);

	output.VertexPos = float4(vertex, 1.0f);
	output.Position = mul(output.VertexPos, WVP);
	output.Normal = float4(normal, 64.0f);
	
	return output;
}

//
// Pixel shader.
//

p2f PShader(in d2p input) {
	p2f output;
	
	input.VertexPos.xyz /= input.VertexPos.w;
	
	//float x = input.VertexPos.x * 0.1f - Timer.w;
	//float z = input.VertexPos.z * 0.1f - Timer.w;

	//input.Normal.xyz = normalize(float3(0.0f, 1.0f, 12.0f * pow(sin(z), 5.0f) * -cos(z)));//normalize(input.Normal.xyz);
	input.Normal.xyz = SineNormal(input.VertexPos.xz, 0.125f, 0.4f, Timer.w, 0.7f, normalize(float2(0.2f, -1.0f)), 2.0f) + 
		SineNormal(input.VertexPos.xz, 0.2f, 0.325f, Timer.w, 0.9f, normalize(float2(-1.0f, -0.8f)), 1.5f) +
		SineNormal(input.VertexPos.xz, 0.15f, 0.4f, Timer.w, 0.8f, normalize(float2(-0.3f, 1.0f)), 2.0f);

	float3 camDir = normalize(CameraPosition.xyz - input.VertexPos.xyz);
	float3 r = normalize(input.Normal.xyz * dot(input.Normal.xyz, camDir) * 2.0f - camDir);
	
	input.Normal.xyz = normalize(input.Normal.xyz);
	output.Color = lerp(float4(0.0f, 0.1f, 0.25f, 1.0f), ProcSky(r), 0.75f);
	output.Normal = float4(input.Normal.xyz, 1.0f);
	output.Position = float4(input.VertexPos.xyz, 1.0f);

	return output;
}
