//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shaders/Includes/PostSky.inc
//
//-------------------------------------------------------------------------------------------------

#define SUN_DIR float3(0.20739034f, 0.51847585f, -0.82956136f)
#define SUN_SIZE 0.0625f
#define SUN_COLOR float3(1.0f, 0.95f, 0.7f)

inline float4 ProcSky(in float3 dir) {
	float sunStrength = min(1.0f, max(SUN_SIZE - acos(max(dot(dir, SUN_DIR), 0.0f)), 0.0f) / (SUN_SIZE * 0.1f));

	return lerp(lerp(float4(0.7f, 0.8f, 1.0f, 1.0f), float4(0.2f, 0.3f, 0.8f, 1.0f), asin(dir.y) / 3.14159265f + 0.5f), float4(SUN_COLOR.rgb, 1.0f), sunStrength);
}

//-------------------------------------------------------------------------------------------------
// Fog.
//-------------------------------------------------------------------------------------------------

inline float LinearFog(in float depth, in float2 range) {
	return saturate((range.y - depth) / (range.y - range.x));
}

inline float ExpFog(in float depth, in float density) {
	return min(1.0f, 1.0f / exp(density * depth));
}

inline float Exp2Fog(in float depth, in float density) {
	return min(1.0f, 1.0f / exp(pow(abs(density * depth), 2.0f)));
}
