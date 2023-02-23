//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shader/Post.shader
//
//-------------------------------------------------------------------------------------------------

$
	texture						( STATIC_EXECUTE:PIXEL )
#ifdef TEXTURE_2
	texture						( STATIC_EXECUTE:PIXEL )
#endif
#ifdef TEXTURE_3
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
#endif
#ifdef TEXTURE_4
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
#endif
#ifdef TEXTURE_5
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
#endif
#ifdef TEXTURE_6
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
#endif
#ifdef TEXTURE_7
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
#endif
#ifdef TEXTURE_8
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
	texture						( STATIC_EXECUTE:PIXEL )
#endif

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R8G8B8A8_UNORM
$

#include "Includes/Color.hlsli"

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
	
	/*
		-1.0f -1.0f
		 3.0f -1.0f
		-1.0f  3.0f
	*/

	// Full screen triangle.
	output.Position = float4(((i & 1) - 0.25f), ((i >> 1) & 1) - 0.25f, 0.0f, 0.25f);

	return output;
}

Texture2D inputTexture_1 : register(t0);
#ifdef TEXTURE_2
	Texture2D inputTexture_2 : register(t1);
#endif
#ifdef TEXTURE_3
	Texture2D inputTexture_2 : register(t1);
	Texture2D inputTexture_3 : register(t2);
#endif
#ifdef TEXTURE_4
	Texture2D inputTexture_2 : register(t1);
	Texture2D inputTexture_3 : register(t2);
	Texture2D inputTexture_4 : register(t3);
#endif
#ifdef TEXTURE_5
	Texture2D inputTexture_2 : register(t1);
	Texture2D inputTexture_3 : register(t2);
	Texture2D inputTexture_4 : register(t3);
	Texture2D inputTexture_5 : register(t4);
#endif
#ifdef TEXTURE_6
	Texture2D inputTexture_2 : register(t1);
	Texture2D inputTexture_3 : register(t2);
	Texture2D inputTexture_4 : register(t3);
	Texture2D inputTexture_5 : register(t4);
	Texture2D inputTexture_6 : register(t5);
#endif
#ifdef TEXTURE_7
	Texture2D inputTexture_2 : register(t1);
	Texture2D inputTexture_3 : register(t2);
	Texture2D inputTexture_4 : register(t3);
	Texture2D inputTexture_5 : register(t4);
	Texture2D inputTexture_6 : register(t5);
	Texture2D inputTexture_7 : register(t6);
#endif
#ifdef TEXTURE_8
	Texture2D inputTexture_2 : register(t1);
	Texture2D inputTexture_3 : register(t2);
	Texture2D inputTexture_4 : register(t3);
	Texture2D inputTexture_5 : register(t4);
	Texture2D inputTexture_6 : register(t5);
	Texture2D inputTexture_7 : register(t6);
	Texture2D inputTexture_8 : register(t7);
#endif

inline float3 ApplyColor(in float3 dst, in float4 src)
{
	return lerp(dst, src.rgb, src.a);
}

// Pixel shader function.
p2f PShader(in v2p input) {	
	p2f output;

	output.Color.rgb = inputTexture_1[input.Position.xy].rgb;//ApplyColor(output.Color.rgb, inputTexture_1[input.Position.xy]);

#ifdef TEXTURE_2
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_2[input.Position.xy]);
#endif
#ifdef TEXTURE_3
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_2[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_3[input.Position.xy]);
#endif
#ifdef TEXTURE_4
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_2[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_3[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_4[input.Position.xy]);
#endif
#ifdef TEXTURE_5
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_2[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_3[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_4[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_5[input.Position.xy]);
#endif
#ifdef TEXTURE_6
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_2[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_3[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_4[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_5[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_6[input.Position.xy]);
#endif
#ifdef TEXTURE_7
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_2[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_3[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_4[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_5[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_6[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_7[input.Position.xy]);
#endif
#ifdef TEXTURE_8
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_2[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_3[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_4[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_5[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_6[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_7[input.Position.xy]);
	output.Color.rgb = ApplyColor(output.Color.rgb, inputTexture_8[input.Position.xy]);
#endif

	output.Color.a = 1.0f;
	return output;
}
