//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: ../Resources/Shaders/VoxelEditor/Gizmo.shader
//
//-------------------------------------------------------------------------------------------------

$
	/* { pSemanticName, semanticIndex, format, inputSlot, alignedByteOffset, inputSlotClass, instanceDataStepRate } */
	input							( POSITION:0:R32G32B32_FLOAT:0:0:VERTEX:0 )
	input							( NORMAL:0:R32G32B32_FLOAT:0:12:VERTEX:0 )

	input							( INSTCOLOR:0:R32G32B32A32_FLOAT:1:0:INSTANCE:1 )
	input							( INSTMATRIX:0:R32G32B32A32_FLOAT:1:16:INSTANCE:1 )
	input							( INSTMATRIX:1:R32G32B32A32_FLOAT:1:32:INSTANCE:1 )
	input							( INSTMATRIX:2:R32G32B32A32_FLOAT:1:48:INSTANCE:1 )
	input							( INSTMATRIX:3:R32G32B32A32_FLOAT:1:64:INSTANCE:1 )
	input							( INSTPOS:0:R32G32B32_FLOAT:1:80:INSTANCE:1 )
#ifdef LINE
	input							( INSTWIDTH:0:R32_FLOAT:1:92:INSTANCE:1 )
#endif
	
	cbv								( CONSTANTS:VERTEX:DYNAMIC )
#ifdef LINE
	cbv								( DESCRIPTOR:VERTEX:DYNAMIC )
#endif

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	cull							NONE
	color							R32G32B32A32_FLOAT
	depth							D32_FLOAT

	blend							( TRUE:FALSE:SRC_ALPHA:INV_SRC_ALPHA:ADD:SRC_ALPHA:INV_SRC_ALPHA:ADD:NOOP:R|G|B|A )
	//blend							( TRUE:FALSE:ONE:ONE:ADD:ONE:ZERO:ADD:NOOP:R|G|B|A )
$

cbuffer MatrixBuffer : register(b0)
{
	float4x4 VP;
	float4 CameraPosition;
	float4 CameraForward;
};

#ifdef LINE
cbuffer DataBuffer : register(b1)
{
	float4 ScreenSize;
	float4 padding0[15];
};
#endif

struct a2v
{
#ifdef LINE
	uint Id : SV_VertexID;
#endif

	float4 Position : POSITION;
	float3 Normal : NORMAL;

	float4 InstColor : INSTCOLOR;
	float4 InstM0 : INSTMATRIX0;
	float4 InstM1 : INSTMATRIX1;
	float4 InstM2 : INSTMATRIX2;
	float4 InstM3 : INSTMATRIX3;
	float3 Origin : INSTPOS;
	
#ifdef LINE
	float InstWidth : INSTWIDTH;
#endif
};

struct v2p
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
#ifdef TRIANGLE
	float3 Normal : NORMAL;
#endif
};

struct p2f
{
	float4 Color : SV_TARGET;
};

v2p VShader(in a2v input)
{
	v2p output;

	// Position.
	float4x4 world = float4x4(input.InstM0, input.InstM1, input.InstM2, input.InstM3);
	
	input.Position.w = 1.0f;
	float dist = dot(input.Origin.xyz - CameraPosition.xyz, CameraForward.xyz);

	output.Position = mul(input.Position, world);
	output.Position.xyz *= dist;
	output.Position.xyz += input.Origin;
	output.Position = mul(output.Position, VP);

#ifdef LINE
	output.Position /= output.Position.w;

	float3 lineNormal = normalize(mul(input.Normal.xyz, (float3x3)world));

	float4 normal = mul(float4(input.Position.xyz, 1.0f), world);
	normal.xyz += lineNormal;
	normal.xyz *= dist;
	normal.xyz += input.Origin;
	normal = normalize(mul(normal, VP));
	normal /= normal.w;

	float2 edge = normalize(normal.xy - output.Position.xy);
	edge = float2(-edge.y, edge.x);
	edge *= ScreenSize.zw;

	output.Position.xy -= edge * ((input.Id % 2) * 2.0f - 1.0f) * input.InstWidth;// * sign(dot(mul(input.Normal.xyz, (float3x3)world), CameraForward.xyz));
#endif
#ifdef TRIANGLE
	// Normal.
	output.Normal = mul(input.Normal, (float3x3)world);
#endif

	// Color.
	output.Color = input.InstColor;

	return output;
}

p2f PShader(in v2p input)
{
	p2f output;

	output.Color = input.Color;

#ifdef TRIANGLE
#ifdef LIT
	output.Color.rgb *= dot(normalize(input.Normal), normalize(float3(0.5f, 0.75f, -1.0f))) * 0.5f + 0.5f;
#endif
#endif

	return output;
}
