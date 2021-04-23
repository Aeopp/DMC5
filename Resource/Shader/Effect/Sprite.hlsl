matrix World;
matrix ViewProjection;
float3 LightDirection = float3(0.f, -1.f, 0.f);

float _SliceAmount = 0.f;

float2 _MinTexUV = float2(0.f, 0.f);
float2 _MaxTexUV = float2(1.f, 1.f);


texture ALB0Map;
sampler ALB0 = sampler_state
{
    texture = ALB0Map;
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
    AddressU = Wrap;
    AddressV = Wrap;
};

texture Msk0Map;
sampler Msk0 = sampler_state
{
    texture = Msk0Map;
    minfilter = point;
    magfilter = point;
    mipfilter = point;
    sRGBTexture = false;
    AddressU = Wrap;
    AddressV = Wrap;
};

texture NoiseMap;
sampler Noise = sampler_state
{
    texture = NoiseMap;
    minfilter = linear;
    magfilter = linear;
    mipfilter = linear;
    sRGBTexture = true;
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

VsOut VsMain0(VsIn In)
{
    VsOut Out = (VsOut) 0;
    
    matrix WVP = World;
    WVP = mul(WVP, ViewProjection);
    
    Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    Out.Normal = normalize(mul(float4(In.Normal.xyz, 0.f), World));
    Out.Tangent = normalize(mul(float4(In.BiNormal.xyz, 0.f), World));
    Out.BiNormal = normalize(mul(float4(In.Position.xyz, 0.f), World));
    Out.UV = In.UV;
        
    return Out;
};

VsOut VsMain1(VsIn In)
{
    VsOut Out = (VsOut) 0;
    
    matrix WVP = World;
    WVP = mul(WVP, ViewProjection);
    
    Out.Position = mul(float4(In.Position.xyz, 1.f), WVP);
    Out.UV.x = (_MaxTexUV.x - _MinTexUV.x) * In.UV.x + _MinTexUV.x;
    Out.UV.y = (_MaxTexUV.y - _MinTexUV.y) * In.UV.y + _MinTexUV.y;

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

PsOut PsMain0(PsIn In)
{
    PsOut Out = (PsOut) 0;
    
    float4 NoiseSample = tex2D(Noise, In.UV).rrrr;
    NoiseSample.rgb -= saturate(_SliceAmount);
    clip(NoiseSample);

    float4 AlbSample = tex2D(ALB0, In.UV);
    float4 NRMRSample = tex2D(NRMR0, In.UV);
    float4 MskSample = tex2D(Msk0, In.UV);
   
    float2 NormalXY = NRMRSample.xy * 2.f - 1.f;
    float NormalZ = sqrt(1 - dot(NormalXY, NormalXY));
   
    float3x3 TBN = float3x3(normalize(In.Tangent),
                            normalize(In.BiNormal),
                            normalize(In.Normal));
    
    float3 WorldNormal = normalize(mul(float3(NormalXY, NormalZ), TBN));
             
    float Diffuse = saturate(dot(WorldNormal, -normalize(LightDirection)));

    Out.Color = float4(0.55f, 0.f, 0.f, MskSample.b - 0.5f);
    Out.Color += float4(0.424f, 0.f, 0.039f, MskSample.g - 0.5f);
    saturate(Out.Color.a);
    
    return Out;
};

PsOut PsMain1(PsIn In)
{
    PsOut Out = (PsOut) 0;

    Out.Color = tex2D(ALB0, In.UV);
    Out.Color.a *= 1.f - saturate(_SliceAmount);

    return Out;
};


technique Default
{
	pass p0
	{
		alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;
        //zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = false; // 텍스쳐 안쓰면 끄는게 나은듯.....

        vertexshader = compile vs_3_0 VsMain0();
        pixelshader = compile ps_3_0 PsMain0();
    }
    pass p1
    {
        alphablendenable = true;
        srcblend = srcalpha;
        destblend = invsrcalpha;
        //zenable = false;
        zwriteenable = false;
        sRGBWRITEENABLE = true;

        vertexshader = compile vs_3_0 VsMain1();
        pixelshader = compile ps_3_0 PsMain1();
    }
};