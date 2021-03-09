#include "LightHelper.hlsli"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
    float4 texColor = g_Texture.Sample(g_SamplerState, In.TexCoord);
    clip(texColor.a - 0.1);
    
    In.Normal = normalize(In.Normal);
    
    float3 toEyeW = normalize(CameraPos.xyz - In.WorldPosition.xyz);
    
    outDiffuse = ComputeCommonLights(In, texColor, In.Normal.xyz, toEyeW);
}