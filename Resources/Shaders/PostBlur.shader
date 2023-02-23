//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: ../Resources/Shaders/VoxelEditor/PostBlur.shader
//
//-------------------------------------------------------------------------------------------------

$
	texture						( VOLATILE:PIXEL )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST

#ifdef CHANNEL_1
	color							R32_FLOAT
#endif
#ifdef CHANNEL_2
	color							R32G32_FLOAT
#endif
#ifdef CHANNEL_3
	color							R32G32B32_FLOAT
#endif
#ifdef CHANNEL_4
	color							R32G32B32A32_FLOAT
#endif
$

// Vertex output structure.
struct v2p
{
	float4 Position : SV_POSITION;
};

// Pixel output structure.
struct p2f
{
#ifdef CHANNEL_1
	float Color : SV_TARGET0;
#endif
#ifdef CHANNEL_2
	float2 Color : SV_TARGET0;
#endif
#ifdef CHANNEL_3
	float3 Color : SV_TARGET0;
#endif
#ifdef CHANNEL_4
	float4 Color : SV_TARGET0;
#endif
};

// Vertex shader function.
v2p VShader(in uint i : SV_VertexID)
{
	v2p output;
	
	// Full screen triangle.
	output.Position = float4(((i & 1) - 0.25f), ((i >> 1) & 1) - 0.25f, 0.0f, 0.25f);

	return output;
}

#ifdef CHANNEL_1
	Texture2D<float> inputTexture : register(t0);
#endif
#ifdef CHANNEL_2
	Texture2D<float2> inputTexture : register(t0);
#endif
#ifdef CHANNEL_3
	Texture2D<float3> inputTexture : register(t0);
#endif
#ifdef CHANNEL_4
	Texture2D<float4> inputTexture : register(t0);
#endif

#ifdef SIGMA_0
	#define WEIGHT0 1.0f
#endif
#ifdef SIGMA_1
	// Sigma (0.5)
	#define WEIGHT0 0.682689f
	#define WEIGHT1 0.157305f
#endif
#ifdef SIGMA_2
	// Sigma (1.0)
	#define WEIGHT0 0.382928f
	#define WEIGHT1 0.241732f
	#define WEIGHT2 0.060598f
#endif
#ifdef SIGMA_3
	// Sigma (1.5)
	#define WEIGHT0 0.261824f
	#define WEIGHT1 0.211357f
	#define WEIGHT2 0.111165f
	#define WEIGHT3 0.038078f
#endif
#ifdef SIGMA_4
	// Sigma (2.0)
	#define WEIGHT0 0.20236f
	#define WEIGHT1 0.179044f
	#define WEIGHT2 0.124009f
	#define WEIGHT3 0.067234f
	#define WEIGHT4 0.028532f
#endif
#ifdef SIGMA_5
	// Sigma (2.5)
	#define WEIGHT0 0.163053f
	#define WEIGHT1 0.150677f
	#define WEIGHT2 0.118904f
	#define WEIGHT3 0.080127f
	#define WEIGHT4 0.046108f
	#define WEIGHT5 0.022657f
#endif

// Pixel shader function.
p2f PShader(in v2p input)
{	
	p2f output;
	
	output.Color = inputTexture[input.Position.xy] * WEIGHT0;

#ifdef HBLUR
	#ifdef SIGMA_1
		output.Color += inputTexture[input.Position.xy + float2(-1.0f, 0.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2( 1.0f, 0.0f)] * WEIGHT1;
	#endif
	#ifdef SIGMA_2
		output.Color += inputTexture[input.Position.xy + float2(-2.0f, 0.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2(-1.0f, 0.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2( 1.0f, 0.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2( 2.0f, 0.0f)] * WEIGHT2;
	#endif
	#ifdef SIGMA_3
		output.Color += inputTexture[input.Position.xy + float2(-3.0f, 0.0f)] * WEIGHT3;
		output.Color += inputTexture[input.Position.xy + float2(-2.0f, 0.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2(-1.0f, 0.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2( 1.0f, 0.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2( 2.0f, 0.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2( 3.0f, 0.0f)] * WEIGHT3;
	#endif
	#ifdef SIGMA_4
		output.Color += inputTexture[input.Position.xy + float2(-4.0f, 0.0f)] * WEIGHT4;
		output.Color += inputTexture[input.Position.xy + float2(-3.0f, 0.0f)] * WEIGHT3;
		output.Color += inputTexture[input.Position.xy + float2(-2.0f, 0.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2(-1.0f, 0.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2( 1.0f, 0.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2( 2.0f, 0.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2( 3.0f, 0.0f)] * WEIGHT3;
		output.Color += inputTexture[input.Position.xy + float2( 4.0f, 0.0f)] * WEIGHT4;
	#endif
	#ifdef SIGMA_5
		output.Color += inputTexture[input.Position.xy + float2(-5.0f, 0.0f)] * WEIGHT5;
		output.Color += inputTexture[input.Position.xy + float2(-4.0f, 0.0f)] * WEIGHT4;
		output.Color += inputTexture[input.Position.xy + float2(-3.0f, 0.0f)] * WEIGHT3;
		output.Color += inputTexture[input.Position.xy + float2(-2.0f, 0.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2(-1.0f, 0.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2( 1.0f, 0.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2( 2.0f, 0.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2( 3.0f, 0.0f)] * WEIGHT3;
		output.Color += inputTexture[input.Position.xy + float2( 4.0f, 0.0f)] * WEIGHT4;
		output.Color += inputTexture[input.Position.xy + float2( 5.0f, 0.0f)] * WEIGHT5;
	#endif
#endif
		
#ifdef VBLUR
	#ifdef SIGMA_1
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -1.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  1.0f)] * WEIGHT1;
	#endif
	#ifdef SIGMA_2
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -2.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -1.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  1.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  2.0f)] * WEIGHT2;
	#endif
	#ifdef SIGMA_3
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -3.0f)] * WEIGHT3;
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -2.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -1.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  1.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  2.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  3.0f)] * WEIGHT3;
	#endif
	#ifdef SIGMA_4
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -4.0f)] * WEIGHT4;
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -3.0f)] * WEIGHT3;
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -2.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -1.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  1.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  2.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  3.0f)] * WEIGHT3;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  4.0f)] * WEIGHT4;
	#endif
	#ifdef SIGMA_5
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -5.0f)] * WEIGHT5;
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -4.0f)] * WEIGHT4;
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -3.0f)] * WEIGHT3;
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -2.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2(0.0f, -1.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  1.0f)] * WEIGHT1;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  2.0f)] * WEIGHT2;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  3.0f)] * WEIGHT3;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  4.0f)] * WEIGHT4;
		output.Color += inputTexture[input.Position.xy + float2(0.0f,  5.0f)] * WEIGHT5;
	#endif
#endif

	return output;
}
