uniform matrix World;
uniform matrix ViewProjection;
uniform float3 LightDirection = float3(0.f, -1.f, 0.f);

uniform float exposure_corr = 1.f;
uniform float _BrightScale = 1.f;

uniform float3 _ExtraColor = float3(0.f, 0.f, 0.f);

texture ALB0Map;
sampler ALB0 = sampler_state
{
    texture = ALB0Map;
    minfilter = anisotropic;
    magfilter = anisotropic;
    mipfilter = anisotropic;
    sRGBTexture = true;
    MaxAnisotropy = 4;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture NRMR0Map;
sampler NRMR0 = sampler_state
{
    texture = NRMR0Map;
    minfilter = point;
    magfilter = point;
    mipfilter = point;
    sRGBTexture = false;
    AddressU = Wrap;
    AddressV = Wrap;
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
};

VsOut VsMain(VsIn In)
{
    VsOut Out = (VsOut) 0;
 
    float4x4 WVP = mul(World, ViewProjection);
    Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    Out.Normal = mul(float4(In.Normal.xyz, 0.f), World);
    Out.Tangent = mul(float4(In.Tangent.xyz, 0.f), World);
    Out.BiNormal = mul(float4(In.BiNormal.xyz, 0.f), World);
    Out.UV = In.UV;

    return Out;
};


struct PsIn
{
    float2 UV : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
    float3 BiNormal : TEXCOORD3;
};

struct PsOut
{
    float4 Color : COLOR0;
};

PsOut PsMain(PsIn In)
{
    PsOut Out = (PsOut) 0;

    float2 NormalXY = tex2D(NRMR0, In.UV).xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));
    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
    float Diffuse = saturate(dot(WorldNormal, -normalize(LightDirection)));
    
    Out.Color = float4(saturate(tex2D(ALB0, In.UV).rgb + _ExtraColor) * _BrightScale * exposure_corr, 0.4f);
      
    return Out;
};


technique Default
{
    pass p0
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        zenable = true;
        zwriteenable = true;
        sRGBWRITEENABLE = false;

        vertexshader = compile vs_3_0 VsMain();
        pixelshader = compile ps_3_0 PsMain();
    }
};