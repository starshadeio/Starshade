//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: ../Resources/Shaders/VoxelEditor/CyberNode.shader
//
//-------------------------------------------------------------------------------------------------

$
	/* { pSemanticName, semanticIndex, format, inputSlot, alignedByteOffset, inputSlotClass, instanceDataStepRate } */
	input							( POSITION:0:R32G32B32_FLOAT:0:0:VERTEX:0 )
	
	cbv								( DESCRIPTOR:PIXEL:FIXED )
	cbv								( CONSTANTS:VERTEX:DYNAMIC )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R32G32B32A32_FLOAT
	depth							D32_FLOAT
$

cbuffer DataBuffer : register(b0)
{
	float4 Color;
	float4 padding0[15];
};

cbuffer DrawBuffer : register(b1)
{
	float4x4 WVP;
};

struct a2v
{
	float4 Position : POSITION;
};

struct v2p
{
	float4 Position : SV_POSITION;
};

struct p2f
{
	float4 Color : SV_TARGET;
};

v2p VShader(in a2v input)
{
	v2p output;
	
	input.Position.w = 1.0f;
	output.Position = mul(input.Position, WVP);

	return output;
}

p2f PShader(in v2p input)
{
	p2f output;
	output.Color = Color;
	return output;
}
