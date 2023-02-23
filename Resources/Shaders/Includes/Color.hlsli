//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shader/Include/Color.hlsli
//
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Utilities.
//-------------------------------------------------------------------------------------------------

inline float ChannelLinearToGamme(in float channel) {
	if(channel > 0.0031308f) {
		return 1.055f * pow(channel, 1.0f / 2.4f) - 0.055f;
	} else {
		return 12.92f * channel;
	}
}

inline float3 LinearToGamma(in float3 rgb) {
	float gamma = 2.2f;
	return pow(rgb, 1.0f / gamma);
}

inline float3 GammaToLinear(in float3 rgb) {
	float gamma = 2.2f;
	return pow(rgb, gamma);
}

inline float Luminosity(in float3 c) {
	return 0.2126f * c.r + 0.7152f * c.g + 0.0722f * c.b;
}

inline float3 ChangeLuminosity(in float3 c, in float targetLum) {
	return c * (Luminosity(c) / targetLum);
}
