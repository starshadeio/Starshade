//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shader/Debug2D.shader
//
//-------------------------------------------------------------------------------------------------

$
	/* { pSemanticName, semanticIndex, format, inputSlot, alignedByteOffset, inputSlotClass, instanceDataStepRate } */
	input							( POSITION:0:R32G32_FLOAT:0:0:VERTEX:0 )
	
	input							( INSTCOLOR:0:R32G32B32A32_FLOAT:1:0:INSTANCE:1 )
	input							( INSTMATRIX:0:R32G32B32A32_FLOAT:1:16:INSTANCE:1 )
	input							( INSTMATRIX:1:R32G32_FLOAT:1:32:INSTANCE:1 )
	
	cbv								( DESCRIPTOR:VERTEX:DYNAMIC )
	cbv								( CONSTANTS:VERTEX:DYNAMIC )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					LINELIST
	antialiased_lines	TRUE
	color							R32G32B32A32_FLOAT
	depth_write				FALSE

	blend							( TRUE:FALSE:SRC_COLOR:INV_SRC_COLOR:ADD:SRC_ALPHA:INV_SRC_ALPHA:ADD:NOOP:R|G|B|A )
$

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
	float4 InstColor : INSTCOLOR;
	float4 InstM0 : INSTMATRIX0;
	float2 InstM1 : INSTMATRIX1;
};

struct v2p
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
};

struct p2f
{
	float4 Color : SV_TARGET;
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

	// Color.
	output.Color = input.InstColor;

	return output;
}

p2f PShader(in v2p input)
{
	p2f output;
	
	output.Color = input.Color;

	return output;
}
