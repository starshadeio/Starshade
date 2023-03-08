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
	input							( ADJACENT:0:R32G32B32_FLOAT:0:12:VERTEX:0 )
	input							( NORMAL:0:R32G32B32_FLOAT:0:24:VERTEX:0 )

	input							( INSTCOLOR:0:R32G32B32A32_FLOAT:1:0:INSTANCE:1 )
	input							( INSTMATRIX:0:R32G32B32A32_FLOAT:1:16:INSTANCE:1 )
	input							( INSTMATRIX:1:R32G32B32A32_FLOAT:1:32:INSTANCE:1 )
	input							( INSTMATRIX:2:R32G32B32A32_FLOAT:1:48:INSTANCE:1 )
	input							( INSTMATRIX:3:R32G32B32A32_FLOAT:1:64:INSTANCE:1 )
	input							( INSTNORMAL:0:R32G32B32_FLOAT:1:80:INSTANCE:1 )
	input							( INSTWIDTH:0:R32_FLOAT:1:92:INSTANCE:1 )
	
	cbv								( CONSTANTS:VERTEX:DYNAMIC )
	cbv								( DESCRIPTOR:VERTEX:DYNAMIC )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	cull							NONE
	color							R32G32B32A32_FLOAT
	depth							D32_FLOAT
	depth_write				FALSE
#ifdef UNDERLAY
	depth_comparison	GREATER_EQUAL
#endif
	
	blend							( TRUE:FALSE:SRC_ALPHA:INV_SRC_ALPHA:ADD:SRC_ALPHA:INV_SRC_ALPHA:ADD:NOOP:R|G|B|A )
	//blend							( TRUE:FALSE:ONE:ONE:ADD:ONE:ZERO:ADD:NOOP:R|G|B|A )
$

cbuffer MatrixBuffer : register(b0)
{
	float4x4 World;
	float4x4 VP;
	float4x4 VPClip;
	float4 CameraPosition;
	float4 CameraForward;
};

cbuffer DataBuffer : register(b1)
{
	float4 ScreenSize;
	float4 padding0[15];
};

struct a2v
{
	uint Id : SV_VertexID;

	float4 Position : POSITION;
	float4 Adjacent : ADJACENT;
	float4 Normal : NORMAL;

	float4 InstColor : INSTCOLOR;
	float4 InstM0 : INSTMATRIX0;
	float4 InstM1 : INSTMATRIX1;
	float4 InstM2 : INSTMATRIX2;
	float4 InstM3 : INSTMATRIX3;
	float3 InstNormal : INSTNORMAL;
	
	float InstWidth : INSTWIDTH;
};

struct v2p
{
	float4 Position : SV_POSITION;
	float4 Color : COLOR;
	float DotTest : TEXCOORD;
};

struct p2f
{
	float4 Color : SV_TARGET;
};

inline float3 TestProject3D(in float3 origin, in float3 dir, in float4 plane)
{
	float d = dot(plane.xyz, dir);
	if(d < 1e-5f) return origin;
	float n = dot(plane.xyz, plane.xyz * -plane.w - origin);
	float t = n / d;
	if(t < 0.0f || t > 1.0f) return origin;

	return origin + dir * t;
}

v2p VShader(in a2v input)
{
	v2p output;
	
	float4x4 instWorld = float4x4(input.InstM0, input.InstM1, input.InstM2, input.InstM3);

	float4 leftPlane = float4(
		VPClip._11, VPClip._12, VPClip._13, VPClip._14
	);

	float4 rightPlane = float4(
		VPClip._21, VPClip._22, VPClip._23, VPClip._24
	);

	float4 bottomPlane = float4(
		VPClip._31, VPClip._32, VPClip._33, VPClip._34
	);

	float4 topPlane = float4(
		VPClip._41, VPClip._42, VPClip._43, VPClip._44
	);

	// Position.
	input.Position.w = 1.0f;
	output.Position = mul(input.Position, instWorld);
	output.Position = mul(output.Position, World);
	input.Adjacent.w = 1.0f;
	float4 adjacent = mul(input.Adjacent, instWorld);
	adjacent = mul(adjacent, World);
	
	output.Position.xyz = TestProject3D(output.Position.xyz, adjacent.xyz - output.Position.xyz, leftPlane);
	output.Position.xyz = TestProject3D(output.Position.xyz, adjacent.xyz - output.Position.xyz, rightPlane);
	output.Position.xyz = TestProject3D(output.Position.xyz, adjacent.xyz - output.Position.xyz, topPlane);
	output.Position.xyz = TestProject3D(output.Position.xyz, adjacent.xyz - output.Position.xyz, bottomPlane);
	/*output.Position.xyz = TestProject3D(output.Position.xyz, adjacent.xyz - output.Position.xyz, nearPlane);
	output.Position.xyz = TestProject3D(output.Position.xyz, adjacent.xyz - output.Position.xyz, farPlane);*/


	float4 worldPoint = output.Position;

	output.DotTest = dot(normalize(mul(input.InstNormal.xyz, (float3x3)World)), CameraPosition.xyz - output.Position.xyz);
	
	output.Position = mul(output.Position, VP);
	output.Position /= abs(output.Position.w);

	float3 lineNormal = normalize(mul(input.Normal.xyz, (float3x3)instWorld));

	float4 normal = worldPoint;
	normal.xyz += lineNormal;
	normal = mul(normal, VP);
	normal /= abs(normal.w);

	float2 edge = normalize(normal.xy - output.Position.xy);
	edge = float2(-edge.y, edge.x);
	edge *= ScreenSize.zw;
	output.Position.xy -= edge * ((input.Id & 1) * 2.0f - 1.0f) * input.InstWidth;

	// Color.
	output.Color = input.InstColor;

	return output;
}

p2f PShader(in v2p input)
{
	if(input.DotTest > 0.0f) discard;

	p2f output;

	output.Color = input.Color;
#ifdef UNDERLAY
	output.Color.a *= 0.25f;
#endif

	return output;
}
