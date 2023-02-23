//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: ../Resources/Shaders/VoxelEditor/NodeGrid.shader
//
//-------------------------------------------------------------------------------------------------

$
	/* { pSemanticName, semanticIndex, format, inputSlot, alignedByteOffset, inputSlotClass, instanceDataStepRate } */
	input							( POSITION:0:R32G32B32_FLOAT:0:0:VERTEX:0 )
	input							( NORMAL:0:R32G32B32_FLOAT:0:12:VERTEX:0 )
	
	cbv								( DESCRIPTOR:PIXEL:FIXED )
	cbv								( CONSTANTS:VERTEX:DYNAMIC )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					LINELIST
	antialiased_lines	TRUE
	color							R32G32B32A32_FLOAT
	depth							D32_FLOAT
	depth_write				FALSE

	blend							( TRUE:FALSE:SRC_COLOR:INV_SRC_COLOR:ADD:SRC_ALPHA:INV_SRC_ALPHA:ADD:NOOP:R|G|B|A )
$

cbuffer DataBuffer : register(b0)
{
	float4 Color;
	float4 padding0[15];
};

cbuffer DrawBuffer : register(b1)
{
	float4x4 VP;
	float4x4 World;
	float4 CameraPosition;
};

struct a2v
{
	float4 Position : POSITION;
	float4 Normal : NORMAL;
};

struct v2p
{
	float4 Position : SV_POSITION;
	float Dot : TEXCOORD;
};

struct p2f
{
	float4 Color : SV_TARGET;
};

v2p VShader(in a2v input)
{
	v2p output;
	
	input.Position.w = 1.0f;
	output.Position = mul(input.Position, World);

	output.Dot = dot(input.Normal.xyz, CameraPosition - output.Position.xyz);

	output.Position = mul(output.Position, VP);

	return output;
}

p2f PShader(in v2p input)
{
	if(input.Dot <= 0.0f) discard;

	p2f output;
	
	output.Color = Color;

	return output;
}
