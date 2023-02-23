//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: ../Resources/Shaders/VoxelEditor/PostOverlay.shader
//
//-------------------------------------------------------------------------------------------------

$
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R32G32B32A32_FLOAT
$

#include "../Includes/Color.hlsli"

// Vertex output structure.
struct v2p {
	float4 Position : SV_POSITION;
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

	return output;
}

Texture2D colorTexture : register(t0);
Texture2D sceneDepthTexture : register(t1);

// Pixel shader function.
p2f PShader(in v2p input) {	
	p2f output;
	
	output.Color.rgb = colorTexture[input.Position.xy].rgb;

	output.Color.rgb = LinearToGamma(output.Color.rgb);
	output.Color.a = colorTexture[input.Position.xy].a;//dot(output.Color.rgb, float3(0.299f, 0.587f, 0.114f));

	return output;
}
