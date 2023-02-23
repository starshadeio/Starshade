//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shaders/PostToneMapping.shader
//
//-------------------------------------------------------------------------------------------------

$
	texture						( STATIC_EXECUTE:PIXEL )

	vertex_entry			VShader
	vertex_version		vs_5_0
	pixel_entry				PShader
	pixel_version			ps_5_0

	topology					TRIANGLELIST
	color							R32G32B32A32_FLOAT
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
	
	// Full screen triangle.
	output.Position = float4(((i & 1) - 0.25f), ((i >> 1) & 1) - 0.25f, 0.0f, 0.25f);

	return output;
}

//-------------------------------------------------------------------------------------------------
// Tone Mapping operators.
// Source: https://64.github.io/tonemapping/
//-------------------------------------------------------------------------------------------------

#ifdef NONE
	float3 ToneMapping(in float3 c) {
		return c;
	}
#endif

#ifdef REINHARD_SIMPLE
	float3 ToneMapping(in float3 c) {
		return c / (1.0f + c);
	}
#endif

#ifdef REINHARD_RGB
	float3 ToneMapping(in float3 c, float whitePt = 1.0f) {
		const float3 numerator = c * (1.0f + (c / (whitePt * whitePt)));
		return numerator / (1.0f + c);
	}
#endif

#ifdef REINHARD_LUMINANCE
	float3 ToneMapping(in float3 c, float whitePt = 1.0f) {
		const float orgL = Luminosity(c);
		const float numerator = orgL * (1.0f + (orgL / (whitePt * whitePt)));
		const float newL = numerator / (1.0f + orgL);
		return ChangeLuminosity(c, newL);
	}
#endif
	
#ifdef REINHARD_JODIE
	float3 ToneMapping(in float3 c) {
		const float l = Luminosity(c);
		const float3 tc = c / (1.0f + c);
		return lerp(c / (1.0f + l), tc, tc);
	}
#endif
	
#ifdef UNCHARTED2_FILMIC
	// Uncharted 2.
	float3 Uncharted2_Partial_Tonemap(in float3 x) {
		#define UC2_A 0.15f
		#define UC2_B 0.50f
		#define UC2_C 0.10f
		#define UC2_D 0.20f
		#define UC2_E 0.02f
		#define UC2_F 0.30f
		return ((x * (UC2_A * x + UC2_C * UC2_B) + UC2_D * UC2_E) / (x * (UC2_A * x + UC2_B) + UC2_D * UC2_F)) - UC2_E / UC2_F;
	}

	float3 ToneMapping(in float3 c) {
		#define exposureBias 2.0f
		#define W float3(11.2f, 11.2f, 11.2f)

		const float3 cur = Uncharted2_Partial_Tonemap(c * exposureBias);
		const float3 whiteScale = float3(1.0f, 1.0f, 1.0f) / Uncharted2_Partial_Tonemap(W);
		return cur * whiteScale;
	}
#endif

#ifdef ACES_FITTED
	// ACES
	static float3x3 ACES_Input_Matrix = float3x3(
		0.59719f, 0.35458f, 0.04823f,
		0.07600f, 0.90834f, 0.01566f,
		0.02840f, 0.13383f, 0.83777f
	);

	static float3x3 ACES_Output_Matrix = float3x3(
			1.60475f, -0.53108f, -0.07367f,
		-0.10208f,  1.10813f, -0.00605f,
		-0.00327f, -0.07276f,  1.07602f
	);

	/*inline float3 RTT_ODT_Fit(in float3 c) {
		const float3 a = c * (c + 0.0245786f) - 0.000090537f;
		const float3 b = c * (0.983729f * c + 0.4329510f) + 0.238081f;
		return a / b;
	}*/

	inline float3 ToneMapping(in float3 c) {
		c = mul(ACES_Input_Matrix, c);

		const float3 a = c * (c + 0.0245786f) - 0.000090537f;
		const float3 b = c * (0.983729f * c + 0.4329510f) + 0.238081f;

		return mul(ACES_Output_Matrix, a / b);
	}
#endif

#ifdef ACES_APPROX
	inline float3 ToneMapping(in float3 x) {
		#define ACES_A 2.51f
		#define ACES_B 0.03f
		#define ACES_C 2.43f
		#define ACES_D 0.59f
		#define ACES_E 0.14f
		x *= 0.6f;
		return clamp((x * (ACES_A * x + ACES_B)) / (x * (ACES_C * x + ACES_D) + ACES_E), 0.0f, 1.0f);
	}
#endif

Texture2D colorTexture : register(t0);

// Pixel shader function.
p2f PShader(in v2p input) {	
	p2f output;

	float4 color = colorTexture[input.Position.xy];
	output.Color = float4(ToneMapping(color.rgb), color.a);

	output.Color.rgb = LinearToGamma(output.Color.rgb);
	output.Color.a = dot(output.Color.rgb, float3(0.299f, 0.587f, 0.114f));

	return output;
}
