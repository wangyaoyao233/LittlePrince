#include "Common.hlsli"

void main(in VS_IN In, out PS_IN Out)
{
    matrix mvp;
    mvp = mul(World, View);
    mvp = mul(mvp, Projection);
    
    Out.WorldPosition = mul(In.Position, World);
    Out.Position = mul(In.Position, mvp);
    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.0), World));
    
    Out.TexCoord = In.TexCoord;
    Out.Diffuse = In.Diffuse;
}