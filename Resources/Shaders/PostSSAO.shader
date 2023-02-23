//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: ../Resources/Shaders/VoxelEditor/PostEdge.shader
//
//-------------------------------------------------------------------------------------------------

$
	cbv								( DESCRIPTOR:PIXEL:FIXED )
	cbv								( DESCRIPTOR:PIXEL:DYNAMIC )

	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	//texture						( STATIC_EXECUTE:PIXEL )

	/* { filter, addressU, addressV, addressW, mipLODBias, minLOD, maxLOD, maxAnisotropy, shaderVisibility } */
	sampler						( MIN_MAG_MIP_LINEAR:CLAMP:CLAMP:CLAMP:0.0f:0.0f:FLT_MAX,:0:PIXEL )
	//sampler						( MIN_MAG_MIP_LINEAR:WRAP:WRAP:BORDER:0.0f:0.0f:FLT_MAX,:0:PIXEL )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R32_FLOAT
$

cbuffer ConstantBuffer : register(b0)
{
	//float RandomSizeInv;
	float Radius;
	float Intensity;
	float Scale;
	float Bias;
	//float3 padding0;
	float4 padding1[15];
};

cbuffer DataBuffer : register(b1)
{
	float4x4 ProjInv;
	float4x4 View;
	float4 ScreenSize;
	float4 padding2[7];
};

// Vertex output structure.
struct v2p
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};

// Pixel output structure.
struct p2f
{
	float Color : SV_TARGET0;
};

// Vertex shader function.
v2p VShader(in uint i : SV_VertexID)
{
	v2p output;
	
	// Full screen triangle.
	output.Position = float4(((i & 1) - 0.25f), ((i >> 1) & 1) - 0.25f, 0.0f, 0.25f);
	output.UV = float2((i & 1) * 2.0f, 1.0f - ((i >> 1) & 1) * 2.0f);

	return output;
}

Texture2D normalTexture : register(t0);
Texture2D depthTexture : register(t1);
//Texture2D randomTexture : register(t2);
SamplerState linearSampler : register(s0);
//SamplerState wrapSampler : register(s1);

float3 GetPosition(in float2 uv)
{
	float4 position = float4(float2(uv.x, 1.0f - uv.y) * 2.0f - 1.0f, depthTexture.SampleLevel(linearSampler, uv, 0.0f).r, 1.0f);
	position = mul(position, ProjInv);
	//position.xyz /= position.w;
	//position = mul(position, ViewInv);
	return position.xyz / position.w;
}

/*float3 GetNormal(in float2 uv)
{
	return normalTexture.SampleLevel(linearSampler, uv, 0.0f).xyz;
}*/

float2 hash22(in float2 p) {
  float3 p3 = frac(float3(p.xyx) * float3(0.1031f, 0.1030f, 0.0973f));
  p3 += dot(p3, p3.yzx + 33.33 );
  return frac((p3.xx + p3.yz) * p3.zy);
}

float2 GetRandom(in float2 uv)
{
	return normalize(/*randomTexture.SampleLevel(wrapSampler, uv * ScreenSize.xy * RandomSizeInv, 0.0f).xy*/hash22(uv) * 2.0f - 1.0f);
}

float AmbientOcclusion(in float2 uv, in float2 offsetUV, in float3 p, in float3 n)
{
	const float3 diff = GetPosition(uv + offsetUV) - p;
	const float3 v = normalize(diff);
	const float d = length(diff) * Scale;
	return max(0.0f, dot(n, v) - Bias) * (1.0f / (1.0f + d)) * Intensity;
}

#define COS_45 0.70710678f

// Pixel shader function.
p2f PShader(in v2p input)
{	
	p2f output;

	const float2 vec[4] = { float2(1.0f, 0.0f), float2(-1.0f, 0.0f), float2(0.0f, 1.0f), float2(0.0f, -1.0f) };
	const float3 p = GetPosition(input.UV);
	const float3 n = mul(normalTexture[input.Position.xy].xyz, (float3x3)View);
	const float2 rand = GetRandom(input.Position.xy);
	const float rad = Radius / max(1.0f, p.z);
	float ao = 0.0f;

	const int iterations = 4;
	for(int j = 0; j < iterations; ++j)
	{
		float2 coord1 = reflect(vec[j], rand) * rad;
		float2 coord2 = float2(coord1.x * COS_45 - coord1.y * COS_45, coord1.x * COS_45 + coord1.y * COS_45);

		ao += AmbientOcclusion(input.UV, coord1 * 0.25f, p, n);
		ao += AmbientOcclusion(input.UV, coord2 * 0.5f, p, n);
		ao += AmbientOcclusion(input.UV, coord1 * 0.75f, p, n);
		ao += AmbientOcclusion(input.UV, coord2, p, n);
	}

	output.Color = 1.0f - ao / float(iterations * 4);

	return output;
}
