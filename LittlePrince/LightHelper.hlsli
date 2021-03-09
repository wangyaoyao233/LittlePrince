#include "Common.hlsli"

struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float Pad;
};

struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Position;
    float Range;
    
    float3 Att;
    float Pad;
};

struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;

    float3 Position;
    float Range;

    float3 Direction;
    float Spot;

    float3 Att;
    float Pad;
};


cbuffer LightBuffer : register(b7)
{
    DirectionalLight dirLight[10];
    PointLight pointLight[10];
    SpotLight spotLight[10];
    int numDirLight;
    int numPointLight;
    int numSpotLight;
    float Pad;
}

// 计算平行光/方向光函数(光向量是与光照射方向相反的单位向量)
void ComputeDirectionalLight(MATERIAL mat, DirectionalLight L, float3 normal, float3 toEye,
        out float4 ambient, out float4 diffuse, out float4 spec)
{
    //初始化输出
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //光向量与照射方向相反
    float3 lightVec = -L.Direction;
    lightVec = normalize(lightVec);
    
    //添加环境光 la * ma
    ambient = mat.Ambient * L.Ambient;
    
    //添加漫反射光和镜面光
    // kd = max(L*n, 0)
    float diffuseFactor = dot(lightVec, normal);
    
    //展开, 避免动态分支
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 r = reflect(-lightVec, normal);
        // ks = max(R * toEye, 0)^p
        float specFactor = pow(max(dot(r, toEye), 0.0f), mat.Shininess);
        
        // kd * ld * md
        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        // ks * ls * ms
        spec = specFactor * mat.Specular * L.Specular;
    }
}


// 计算点光
void ComputePointLight(MATERIAL mat, PointLight L, float3 pos, float3 normal, float3 toEye,
        out float4 ambient, out float4 diffuse, out float4 spec)
{
    //初始化输出
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //从表面到光源的向量
    float3 lightVec = L.Position - pos;
    
    //表面到光线距离
    float d = length(lightVec);
    
    //灯光范围测试
    if (d > L.Range)
    {
        return;
    }
    
    //标准化光向量
    lightVec /= d;
    
    //环境光计算
    ambient = mat.Ambient * L.Ambient;
    
    //漫反射和镜面计算
    float diffuseFactor = dot(lightVec, normal);
    
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 r = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(r, toEye), 0.0f), mat.Shininess);
        
        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec = specFactor * mat.Specular * L.Specular;
    }
    
    //光的衰减
    float att = 1.0f / dot(L.Att, float3(1.0f, d, d * d));
    
    diffuse *= att;
    spec *= att;
}

// 计算聚光灯
void ComputeSpotLight(MATERIAL mat, SpotLight L, float3 pos, float3 normal, float3 toEye,
        out float4 ambient, out float4 diffuse, out float4 spec)
{
        //初始化输出
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    //从表面到光源的向量
    float3 lightVec = L.Position - pos;
    
    //表面到光线距离
    float d = length(lightVec);
    
    //灯光范围测试
    if (d > L.Range)
    {
        return;
    }
    
    //标准化光向量
    lightVec /= d;
    
    //环境光计算
    ambient = mat.Ambient * L.Ambient;
    
    //漫反射和镜面计算
    float diffuseFactor = dot(lightVec, normal);
    
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 r = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(r, toEye), 0.0f), mat.Shininess);
        
        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec = specFactor * mat.Specular * L.Specular;
    }
    
    //计算汇聚因子和衰弱系数
    float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);
    float att = spot / dot(L.Att, float3(1.0f, d, d * d));
    
    ambient *= spot;
    diffuse *= att;
    spec *= att;
}




// 计算总的光线
float4 ComputeCommonLights(PS_IN In, float4 texColor, float3 normal, float3 toEyeW)
{
  // 初始化为0
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    int i; //cnt
    
    // 计算方向光
    for (i = 0; i < numDirLight; ++i)
    {
        ComputeDirectionalLight(Material, dirLight[i], normal, toEyeW, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }
    
         
    // 计算点光
    for (i = 0; i < numPointLight; ++i)
    {
        ComputePointLight(Material, pointLight[i], In.WorldPosition.xyz, normal, toEyeW, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }
        
    
    // 计算聚光灯
    for (i = 0; i < numSpotLight; ++i)
    {
        ComputeSpotLight(Material, spotLight[i], In.WorldPosition.xyz, normal, toEyeW, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }
    
    float4 litColor = texColor * (ambient + diffuse) + spec;
    litColor.a = Material.Diffuse.a * In.Diffuse.a;
    
    return litColor;
}