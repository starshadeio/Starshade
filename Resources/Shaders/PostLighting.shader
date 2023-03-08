//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shaders/PostLighting.shader
//
//-------------------------------------------------------------------------------------------------

$
	cbv								( DESCRIPTOR:PIXEL:DYNAMIC )

	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
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
	float4x4 ProjInv;
	float4x4 ViewInv;
	float4 padding[8];
};

// Vertex output structure.
struct v2p {
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
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
	output.UV = float2((i & 1) * 2.0f, 1.0f - ((i >> 1) & 1) * 2.0f);

	return output;
}

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D uiTexture : register(t2);
Texture2D depthTexture : register(t3);
Texture2D skyTexture : register(t4);
Texture2D edgeTexture : register(t5);
Texture2D ssaoTexture : register(t6);

// Pixel shader function.
p2f PShader(in v2p input) {	
	p2f output;

	float4 color;
	float fog;

	if(dot(normalTexture[input.Position.xy].xyz, normalTexture[input.Position.xy].xyz) > 0.5f) {
		float3 lightDir = SUN_DIR;
		float d = saturate(dot(normalTexture[input.Position.xy].xyz, lightDir) * 0.5f + 0.5f) * 0.75f + 0.25f;

		float4 position = float4(float2(input.UV.x, (1.0f - input.UV.y)) * 2.0f - 1.0f, depthTexture[input.Position.xy].r, 1.0f);
		position = mul(position, ProjInv);
		position.xyz /= position.w;

		fog = 1.0f - Exp2Fog(position.z, 0.01f);

		/*position = mul(position, ViewInv);
		position.xyz /= position.w;*/
		
		color = float4(saturate(colorTexture[input.Position.xy].rgb * d/* + SUN_COLOR * spec.x * 0.625f*/), colorTexture[input.Position.xy].a);

		//output.Color = lerp(skyTexture[input.Position.xy], color, colorTexture[input.Position.xy].a);
	} else {
		fog = 0.0f;
		color = colorTexture[input.Position.xy];
		//output.Color = lerp(skyTexture[input.Position.xy], colorTexture[input.Position.xy], colorTexture[input.Position.xy].a);
	}
	
	float4 skyColor = skyTexture[input.Position.xy];
	color = lerp(skyColor, color, colorTexture[input.Position.xy].a);

	color.rgb *= edgeTexture[input.Position.xy].r;
	color.rgb *= ssaoTexture[input.Position.xy].r;

	output.Color.rgb = lerp(color.rgb, skyColor.rgb, fog);
	output.Color.rgb = lerp(output.Color.rgb, uiTexture[input.Position.xy].rgb, uiTexture[input.Position.xy].a);

	return output;
}
