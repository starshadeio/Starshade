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
	
	cbv								( DESCRIPTOR:VERTEX:DYNAMIC )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R32G32B32A32_FLOAT|R32G32B32A32_FLOAT|R32G32B32A32_FLOAT
	depth							D32_FLOAT
$

cbuffer MatrixBuffer : register(b0) {
	float4x4 WVP;
	float4 padding0[12];
};

struct a2v {
	float4 Position : POSITION;
	float4 Normal : NORMAL;
};

struct v2p {
	float4 Position : SV_POSITION;
	float4 VertexPos : POSITION;
	float4 Normal : NORMAL;
};

struct p2f {
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Position : SV_TARGET2;
};

v2p VShader(in a2v input) {
	v2p output;
	
	input.Position.w = 1.0f;
	input.Normal.w = 0.0f;
	output.Position = mul(input.Position, WVP);
	output.VertexPos = input.Position;
	output.Normal = input.Normal;

	return output;
}

p2f PShader(in v2p input) {
	p2f output;
	
	input.Normal.xyz = normalize(input.Normal.xyz);
	input.VertexPos.xyz /= input.VertexPos.w;

	output.Color = lerp(float4(0.125f, 0.125f, 0.125f, 1.0f), float4(0.75f, 0.7f, 0.5f, 1.0f), max(input.Normal.y, 0.0f));
	output.Normal = float4(input.Normal.xyz, 1.0f);
	output.Position = float4(input.VertexPos.xyz, 1.0f);

	return output;
}
