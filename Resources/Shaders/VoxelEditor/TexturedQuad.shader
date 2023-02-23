//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: ../Resources/Shaders/VoxelEditor/TexturedQuad.shader
//
//-------------------------------------------------------------------------------------------------

$
	/* { pSemanticName, semanticIndex, format, inputSlot, alignedByteOffset, inputSlotClass, instanceDataStepRate } */
	input							( POSITION:0:R32G32B32_FLOAT:0:0:VERTEX:0 )
	input							( TEXCOORD:0:R32G32_FLOAT:0:12:VERTEX:0 )
	
	cbv								( CONSTANTS:VERTEX:DYNAMIC )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R32G32B32A32_FLOAT|R32G32B32A32_FLOAT
	depth							D32_FLOAT
	depth_write				FALSE
	cull							NONE

	//blend							( TRUE:FALSE:SRC_ALPHA:INV_SRC_ALPHA:ADD:SRC_ALPHA:INV_SRC_ALPHA:ADD:NOOP:R|G|B|A )
$

cbuffer MatrixBuffer : register(b0)
{
	float4x4 WV;
	float4x4 Proj;
};

struct a2v
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD;
};

struct v2p
{
	float4 Position : SV_POSITION;
	float4 WorldPos : TEXCOORD0;
	float3 Direction : TEXCOORD1;
	float2 TexCoord : TEXCOORD2;
};

struct p2f
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
};

v2p VShader(in a2v input)
{
	v2p output;
	
	input.Position.w = 1.0f;
	output.Position = mul(input.Position, WV);
	output.WorldPos = output.Position;
	output.Direction = mul(float3(0.0f, 0.0f, 1.0f), (float3x3)WV);

	output.Position = mul(output.Position, Proj);
	output.Position.z -= 0.00001f;

	output.TexCoord = input.TexCoord;

	return output;
}

p2f PShader(in v2p input)
{
	p2f output;
	input.WorldPos /= input.WorldPos.w;

	float2 edge = abs(input.TexCoord.xy - 0.5f) * 2.0f;

	float angle = saturate(dot(input.Direction, normalize(input.WorldPos.xyz)));
	float distance = saturate(1.0f - input.WorldPos.z * 0.02f);

	float power = 1.5f + 1.5f * (distance + angle);

#ifdef BOX_QUAD
	float sdf = max(edge.x, edge.y);
	float v = pow(sdf, power);
#endif
#ifdef X_QUAD
	float2 pt1 = float2(-input.TexCoord.x, -input.TexCoord.y);
	float2 pt2 = float2(1.0f - input.TexCoord.x, -input.TexCoord.y);
	float2 dir1 = float2(1.0f, 1.0f);
	float2 dir2 = float2(-1.0f, 1.0f);

	float d1 = length(pt1 - dir1 * (dot(pt1, dir1) / dot(dir1, dir1)));
	float d2 = length(pt2 - dir2 * (dot(pt2, dir2) / dot(dir2, dir2)));

	float sdf = saturate(1.0f - min(d1, d2));
	
	float dist = 1.0f - length(edge);
	float v = step(0.85f, pow(sdf, power)) * step(0.5f, dist);
	
	float sdf2 = max(edge.x, edge.y);
	v += pow(sdf2, power);
#endif

	clip(v - 0.5f);
	v = step(0.5f, v).x;

	//output.Color = float4(1.0f - v * 0.5f, 1.0f - v * 0.3f, 1.0f - v * -0.2f, 1.0f);
	output.Color = float4(saturate(1.1f - v).xxx, 1.0f);
	//output.Color = float4(, 0.8f);


	output.Normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
	return output;
}
