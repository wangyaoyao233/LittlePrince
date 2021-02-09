#include "Common.hlsli"


void main(in VS_IN In, out PS_IN Out)
{
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    
    // wvp变换后的位置
    Out.Position = mul(In.Position, wvp);
    
    Out.TexCoord = In.TexCoord;
    Out.Diffuse = In.Diffuse;
}