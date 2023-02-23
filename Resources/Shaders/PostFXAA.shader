//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shaders/PostFXAA.shader
//
//-------------------------------------------------------------------------------------------------

$
	cbv ( DESCRIPTOR:PIXEL:DYNAMIC )
	texture ( STATIC_EXECUTE:PIXEL )
	sampler ( MIN_MAG_MIP_LINEAR:BORDER:BORDER:BORDER:0.0f:0.0f:FLT_MAX,:0:PIXEL )
	vertex_entry VShader
	vertex_version vs_5_0
	pixel_entry PShader
	pixel_version ps_5_0
	topology TRIANGLELIST
	color R32G32B32A32_FLOAT
$

cbuffer DataBuffer : register(b0)
{
	float4 ScreenSize;
	float4 padding2[15];
};

struct v2p
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD;
};

struct p2f
{
	float4 Color : SV_TARGET0;
};

#define FxaaSat(x) saturate(x)
struct FxaaTex { SamplerState smpl; Texture2D tex; };
#define FxaaTexTop(t, p) t.tex.SampleLevel(t.smpl, p, 0.0)
#define FxaaTexOff(t, p, o, r) t.tex.SampleLevel(t.smpl, p, 0.0, o)
#define FxaaTexAlpha4(t, p) t.tex.GatherAlpha(t.smpl, p)
#define FxaaTexOffAlpha4(t, p, o) t.tex.GatherAlpha(t.smpl, p, o)
#define FxaaTexGreen4(t, p) t.tex.GatherGreen(t.smpl, p)
#define FxaaTexOffGreen4(t, p, o) t.tex.GatherGreen(t.smpl, p, o)
float FxaaLuma(float4 rgba) { return rgba.w; }

float4 FxaaPixelShader(
		float2 pos,
		FxaaTex tex,
		float2 fxaaQualityRcpFrame,
		float fxaaQualitySubpix,
		float fxaaQualityEdgeThreshold,
		float fxaaQualityEdgeThresholdMin) {
	float2 posM;
	posM.x = pos.x;
	posM.y = pos.y;
	float4 rgbyM = FxaaTexTop(tex, posM);
	float4 luma4A = FxaaTexAlpha4(tex, posM);
	float4 luma4B = FxaaTexOffAlpha4(tex, posM, int2(-1, -1));
	float maxSM = max(luma4A.x, rgbyM.w);
	float minSM = min(luma4A.x, rgbyM.w);
	float maxESM = max(luma4A.z, maxSM);
	float minESM = min(luma4A.z, minSM);
	float maxWN = max(luma4B.z, luma4B.x);
	float minWN = min(luma4B.z, luma4B.x);
	float rangeMax = max(maxWN, maxESM);
	float rangeMin = min(minWN, minESM);
	float rangeMaxScaled = rangeMax * fxaaQualityEdgeThreshold;
	float range = rangeMax - rangeMin;
	float rangeMaxClamped = max(fxaaQualityEdgeThresholdMin, rangeMaxScaled);
	bool earlyExit = range < rangeMaxClamped;
	if(earlyExit)
		return rgbyM;
	float lumaNE = FxaaLuma(FxaaTexOff(tex, posM, int2(1, -1), fxaaQualityRcpFrame.xy));
	float lumaSW = FxaaLuma(FxaaTexOff(tex, posM, int2(-1, 1), fxaaQualityRcpFrame.xy));
	float lumaNS = luma4B.z + luma4A.x;
	float lumaWE = luma4B.x + luma4A.z;
	float subpixRcpRange = 1.0/range;
	float subpixNSWE = lumaNS + lumaWE;
	float edgeHorz1 = (-2.0 * rgbyM.w) + lumaNS;
	float edgeVert1 = (-2.0 * rgbyM.w) + lumaWE;
	float lumaNESE = lumaNE + luma4A.y;
	float lumaNWNE = luma4B.w + lumaNE;
	float edgeHorz2 = (-2.0 * luma4A.z) + lumaNESE;
	float edgeVert2 = (-2.0 * luma4B.z) + lumaNWNE;
	float lumaNWSW = luma4B.w + lumaSW;
	float lumaSWSE = lumaSW + luma4A.y;
	float edgeHorz4 = (abs(edgeHorz1) * 2.0) + abs(edgeHorz2);
	float edgeVert4 = (abs(edgeVert1) * 2.0) + abs(edgeVert2);
	float edgeHorz3 = (-2.0 * luma4B.x) + lumaNWSW;
	float edgeVert3 = (-2.0 * luma4A.x) + lumaSWSE;
	float edgeHorz = abs(edgeHorz3) + edgeHorz4;
	float edgeVert = abs(edgeVert3) + edgeVert4;
	float subpixNWSWNESE = lumaNWSW + lumaNESE;
	float lengthSign = fxaaQualityRcpFrame.x;
	bool horzSpan = edgeHorz >= edgeVert;
	float subpixA = subpixNSWE * 2.0 + subpixNWSWNESE;
	if(!horzSpan) luma4B.z = luma4B.x;
	if(!horzSpan) luma4A.x = luma4A.z;
	if(horzSpan) lengthSign = fxaaQualityRcpFrame.y;
	float subpixB = (subpixA * (1.0/12.0)) - rgbyM.w;
	float gradientN = luma4B.z - rgbyM.w;
	float gradientS = luma4A.x - rgbyM.w;
	float lumaNN = luma4B.z + rgbyM.w;
	float lumaSS = luma4A.x + rgbyM.w;
	bool pairN = abs(gradientN) >= abs(gradientS);
	float gradient = max(abs(gradientN), abs(gradientS));
	if(pairN) lengthSign = -lengthSign;
	float subpixC = FxaaSat(abs(subpixB) * subpixRcpRange);
	float2 posB;
	posB.x = posM.x;
	posB.y = posM.y;
	float2 offNP;
	offNP.x = (!horzSpan) ? 0.0 : fxaaQualityRcpFrame.x;
	offNP.y = ( horzSpan) ? 0.0 : fxaaQualityRcpFrame.y;
	if(!horzSpan) posB.x += lengthSign * 0.5;
	if( horzSpan) posB.y += lengthSign * 0.5;
	float2 posN;
	posN.x = posB.x - offNP.x * 1.0;
	posN.y = posB.y - offNP.y * 1.0;
	float2 posP;
	posP.x = posB.x + offNP.x * 1.0;
	posP.y = posB.y + offNP.y * 1.0;
	float subpixD = ((-2.0)*subpixC) + 3.0;
	float lumaEndN = FxaaLuma(FxaaTexTop(tex, posN));
	float subpixE = subpixC * subpixC;
	float lumaEndP = FxaaLuma(FxaaTexTop(tex, posP));
	if(!pairN) lumaNN = lumaSS;
	float gradientScaled = gradient * 1.0/4.0;
	float lumaMM = rgbyM.w - lumaNN * 0.5;
	float subpixF = subpixD * subpixE;
	bool lumaMLTZero = lumaMM < 0.0;
	lumaEndN -= lumaNN * 0.5;
	lumaEndP -= lumaNN * 0.5;
	bool doneN = abs(lumaEndN) >= gradientScaled;
	bool doneP = abs(lumaEndP) >= gradientScaled;
	if(!doneN) posN.x -= offNP.x * 1.0;
	if(!doneN) posN.y -= offNP.y * 1.0;
	bool doneNP = (!doneN) || (!doneP);
	if(!doneP) posP.x += offNP.x * 1.0;
	if(!doneP) posP.y += offNP.y * 1.0;
	
	if(doneNP) {
		if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(tex, posN.xy));
		if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(tex, posP.xy));
		if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
		if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
		doneN = abs(lumaEndN) >= gradientScaled;
		doneP = abs(lumaEndP) >= gradientScaled;
		if(!doneN) posN.x -= offNP.x * 1.0;
		if(!doneN) posN.y -= offNP.y * 1.0;
		doneNP = (!doneN) || (!doneP);
		if(!doneP) posP.x += offNP.x * 1.0;
		if(!doneP) posP.y += offNP.y * 1.0;
		if(doneNP) {
			if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(tex, posN.xy));
			if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(tex, posP.xy));
			if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
			if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
			doneN = abs(lumaEndN) >= gradientScaled;
			doneP = abs(lumaEndP) >= gradientScaled;
			if(!doneN) posN.x -= offNP.x * 1.0;
			if(!doneN) posN.y -= offNP.y * 1.0;
			doneNP = (!doneN) || (!doneP);
			if(!doneP) posP.x += offNP.x * 1.0;
			if(!doneP) posP.y += offNP.y * 1.0;
			if(doneNP) {
				if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(tex, posN.xy));
				if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(tex, posP.xy));
				if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
				if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
				doneN = abs(lumaEndN) >= gradientScaled;
				doneP = abs(lumaEndP) >= gradientScaled;
				if(!doneN) posN.x -= offNP.x * 1.0;
				if(!doneN) posN.y -= offNP.y * 1.0;
				doneNP = (!doneN) || (!doneP);
				if(!doneP) posP.x += offNP.x * 1.0;
				if(!doneP) posP.y += offNP.y * 1.0;
				if(doneNP) {
					if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(tex, posN.xy));
					if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(tex, posP.xy));
					if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
					if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
					doneN = abs(lumaEndN) >= gradientScaled;
					doneP = abs(lumaEndP) >= gradientScaled;
					if(!doneN) posN.x -= offNP.x * 1.5;
					if(!doneN) posN.y -= offNP.y * 1.5;
					doneNP = (!doneN) || (!doneP);
					if(!doneP) posP.x += offNP.x * 1.5;
					if(!doneP) posP.y += offNP.y * 1.5;
					if(doneNP) {
						if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(tex, posN.xy));
						if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(tex, posP.xy));
						if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
						if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
						doneN = abs(lumaEndN) >= gradientScaled;
						doneP = abs(lumaEndP) >= gradientScaled;
						if(!doneN) posN.x -= offNP.x * 2.0;
						if(!doneN) posN.y -= offNP.y * 2.0;
						doneNP = (!doneN) || (!doneP);
						if(!doneP) posP.x += offNP.x * 2.0;
						if(!doneP) posP.y += offNP.y * 2.0;
						if(doneNP) {
							if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(tex, posN.xy));
							if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(tex, posP.xy));
							if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
							if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
							doneN = abs(lumaEndN) >= gradientScaled;
							doneP = abs(lumaEndP) >= gradientScaled;
							if(!doneN) posN.x -= offNP.x * 2.0;
							if(!doneN) posN.y -= offNP.y * 2.0;
							doneNP = (!doneN) || (!doneP);
							if(!doneP) posP.x += offNP.x * 2.0;
							if(!doneP) posP.y += offNP.y * 2.0;
							if(doneNP) {
								if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(tex, posN.xy));
								if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(tex, posP.xy));
								if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
								if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
								doneN = abs(lumaEndN) >= gradientScaled;
								doneP = abs(lumaEndP) >= gradientScaled;
								if(!doneN) posN.x -= offNP.x * 2.0;
								if(!doneN) posN.y -= offNP.y * 2.0;
								doneNP = (!doneN) || (!doneP);
								if(!doneP) posP.x += offNP.x * 2.0;
								if(!doneP) posP.y += offNP.y * 2.0;
								if(doneNP) {
									if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(tex, posN.xy));
									if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(tex, posP.xy));
									if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
									if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
									doneN = abs(lumaEndN) >= gradientScaled;
									doneP = abs(lumaEndP) >= gradientScaled;
									if(!doneN) posN.x -= offNP.x * 2.0;
									if(!doneN) posN.y -= offNP.y * 2.0;
									doneNP = (!doneN) || (!doneP);
									if(!doneP) posP.x += offNP.x * 2.0;
									if(!doneP) posP.y += offNP.y * 2.0;
									if(doneNP) {
										if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(tex, posN.xy));
										if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(tex, posP.xy));
										if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
										if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
										doneN = abs(lumaEndN) >= gradientScaled;
										doneP = abs(lumaEndP) >= gradientScaled;
										if(!doneN) posN.x -= offNP.x * 4.0;
										if(!doneN) posN.y -= offNP.y * 4.0;
										doneNP = (!doneN) || (!doneP);
										if(!doneP) posP.x += offNP.x * 4.0;
										if(!doneP) posP.y += offNP.y * 4.0;
										if(doneNP) {
											if(!doneN) lumaEndN = FxaaLuma(FxaaTexTop(tex, posN.xy));
											if(!doneP) lumaEndP = FxaaLuma(FxaaTexTop(tex, posP.xy));
											if(!doneN) lumaEndN = lumaEndN - lumaNN * 0.5;
											if(!doneP) lumaEndP = lumaEndP - lumaNN * 0.5;
											doneN = abs(lumaEndN) >= gradientScaled;
											doneP = abs(lumaEndP) >= gradientScaled;
											if(!doneN) posN.x -= offNP.x * 8.0;
											if(!doneN) posN.y -= offNP.y * 8.0;
											doneNP = (!doneN) || (!doneP);
											if(!doneP) posP.x += offNP.x * 8.0;
											if(!doneP) posP.y += offNP.y * 8.0;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	float dstN = posM.x - posN.x;
	float dstP = posP.x - posM.x;
	if(!horzSpan) dstN = posM.y - posN.y;
	if(!horzSpan) dstP = posP.y - posM.y;
	bool goodSpanN = (lumaEndN < 0.0) != lumaMLTZero;
	float spanLength = (dstP + dstN);
	bool goodSpanP = (lumaEndP < 0.0) != lumaMLTZero;
	float spanLengthRcp = 1.0/spanLength;
	bool directionN = dstN < dstP;
	float dst = min(dstN, dstP);
	bool goodSpan = directionN ? goodSpanN : goodSpanP;
	float subpixG = subpixF * subpixF;
	float pixelOffset = (dst * (-spanLengthRcp)) + 0.5;
	float subpixH = subpixG * fxaaQualitySubpix;
	float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
	float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
	if(!horzSpan) posM.x += pixelOffsetSubpix * lengthSign;
	if( horzSpan) posM.y += pixelOffsetSubpix * lengthSign;
	return float4(FxaaTexTop(tex, posM).xyz, rgbyM.w);
}

v2p VShader(in uint i : SV_VertexID)
{
	v2p output;
	output.Position = float4(((i & 1) - 0.25f), ((i >> 1) & 1) - 0.25f, 0.0f, 0.25f);
	output.UV = float2((i & 1) * 2.0f, 1.0f - ((i >> 1) & 1) * 2.0f);
	return output;
}

Texture2D inputTexture : register(t0);
SamplerState inputSampler : register(s0);

p2f PShader(in v2p input)
{ 
	p2f output;
	FxaaTex InputFXAATex = { inputSampler, inputTexture };
	output.Color = FxaaPixelShader(input.UV.xy,  InputFXAATex,  ScreenSize.zw,  0.75f,  0.166f,  0.0833f);
	output.Color.a = 1.0f;

	return output;
}
