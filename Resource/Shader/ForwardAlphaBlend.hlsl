matrix World;
matrix View;
matrix Projection;
float3 LightDirection = float3(0, -1, 0);

texture ALBM0Map;
sampler ALBM0 = sampler_state
{
    texture = ALBM0Map;
    minfilter = anisotropic;
    magfilter = anisotropic;
    mipfilter = anisotropic;
    sRGBTexture = true;
    MaxAnisotropy = 4;
};

texture NRMR0Map;
sampler NRMR0 = sampler_state
{
    texture = NRMR0Map;
    minfilter = point;
    magfilter = point;
    mipfilter = point;
    sRGBTexture = false;
};


struct VsIn
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 BiNormal : BINORMAL;
    float2 UV : TEXCOORD0;
};

struct VsOut
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
    float3 WorldPosition : TEXCOORD4;
};


VsOut VsST(VsIn In)
{
    VsOut Out = (VsOut) 0;
    matrix WVP = mul(World, View);
    WVP = mul(WVP, Projection);
    
    Out.UV = In.UV;
    Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.f), World));
    Out.Tangent = normalize(mul(float4(In.BiNormal.xyz, 0.f), World));
    Out.BiNormal = normalize(mul(float4(In.Position.xyz, 0.f), World));

    return Out;
};


VsOut VsSK(VsIn In)
{
    VsOut Out = (VsOut) 0;
    matrix WVP = mul(World, View);
    WVP = mul(WVP, Projection);
    
    Out.UV = In.UV;
    Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.f), World));
    Out.Tangent = normalize(mul(float4(In.Tangent.xyz, 0.f), World));
    Out.BiNormal = normalize(mul(float4(In.BiNormal.xyz, 0.f), World));
    Out.WorldPosition = mul(float4(In.Position.xyz, 1.f), World);
    
    return Out;
};


struct PsIn
{
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
    float3 WorldPosition : TEXCOORD4;
};

struct PsOut
{
    float4 Color : COLOR0;
};

PsOut PsMain(PsIn In)
{
    PsOut Out = (PsOut) 0;
    float4 ALBMSample = tex2D(ALBM0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    float3 Albedo = ALBMSample.xyz;
    float Metalness = ALBMSample.w;
    float Roughness = NRMRSample.w;
    const float2 NormalXY = NRMRSample.xy *2.f-1.f;
    const float  NormalZ =  sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));
    
    const float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    float Diffuse = saturate(dot(WorldNormal, -normalize(LightDirection)));
    Out.Color.rgb  =  Diffuse *Albedo;
    return Out;
};



technique Default
{
    pass
    {
        alphablendenable = false;
        zenable = true;
        zwriteenable = true;
        sRGBWRITEENABLE = true;
        cullmode = ccw;
        fillmode = solid;
        vertexshader = compile vs_3_0 VsSK();
        pixelshader = compile ps_3_0 PsMain();
    }
}


