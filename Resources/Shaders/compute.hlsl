//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Shader/compute.hlsl
//
//-------------------------------------------------------------------------------------------------

struct c2a {
	uint3 GroupThreadID : SV_GroupThreadID; // Range given by numthreads(...)
	uint3 GroupID : SV_GroupID; // Range given by Dispatch(...)
	uint GroupIndex : SV_GroupIndex; // (SV_GroupIndex = SV_GroupThreadID.z * numthreads.x * numthreads.y) + (SV_GroupThreadID.y * numthreads.x) + (SV_GroupThreadID.x)
	uint3 DispatchThreadID : SV_DispatchThreadID; // Sum of: SV_GroupID * numthreads and GroupThreadID
};

RWBuffer<float4> g_uav_in : register(u0);
RWBuffer<float4> g_uav_out : register(u1);

[numthreads(1, 1, 1)]
void CShader(in c2a input) {
	const uint index = input.GroupID.x * 1 + input.GroupIndex;
	g_uav_out[index] = float4(fmod((g_uav_in[index].x + 1.25f) + 0.005f, 2.5f) - 1.25f, 0.0f, 0.0f, 0.0f);
}
